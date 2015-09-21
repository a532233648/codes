
#include <dlfcn.h>

#include "CurlThread.h"
#include "log.h"
#include "curl_plugin.h"
#include "thread_inter.h"

typedef struct{
    int parallel_link_max;
    int timeout;
    int src_ip_num;
    char (*src_ips)[20];
}CURL_CONF_T;

typedef struct {
    int used;   //标记该结构单元是否已被使用
    CURL_REQUEST_INFO_T	request;
    CURL	*easy_handle;
    int	length;
    int	cont_size;
    int cont_len;
    char *content;
    char sip[128];
    char dip[128];
    uint16_t    sport;
    uint16_t    dport;
    time_t updatetime;
}CURL_TASKINFO_T;


typedef	struct{
	THREAD_PLUGIN_PARAM_T	tpp;
	CURL_CONF_T	config;
	void *dl_handle;
	PLUGIN_INIT_FP	*Plugin_Init;
	PLUGIN_RELEASE_FP *Plugin_Release;
	PLUGIN_RECV_TASK_FP *Plugin_Recv_Task;
	PLUGIN_RELEASE_REQUEST_FP *Plugin_Release_Request;
	PLUGIN_GET_REQUEST_FP *Plugin_Get_Request;
	PLUGIN_PRO_RESULT_FP *Plugin_Pro_Result;
	PLUGIN_PERIOD_FP *Plugin_Period;
	
	void *userdata;
	int run_task_num;
	
	CURLM *multi_handle;
	CURL_TASKINFO_T *taskinfos;
	int	cur_srcip_index;
	int cur_taskinfo_index;

}CURL_THREAD_GLOBAL_T;


static void Release_CURL_TASKINFO_T(CURL_TASKINFO_T *task){
	if(task==NULL)
		return ;
		
	if(task->easy_handle){
		curl_easy_cleanup(task->easy_handle);
		task->easy_handle = NULL;
	}
	
	if(task->content){
		free(task->content);
		task->content = NULL;
	}
}



static int Send_Result(void *handle,int ptype,NETADDR_T	*peer,int cid,char *buff, int size){
	THREAD_PLUGIN_PARAM_T *pm = (THREAD_PLUGIN_PARAM_T*)handle;
	if(pm==NULL || buff==NULL || size<=0)
		return -1;
	
	PLUGIN_INTER_STRUCT_T sendresult;
			
	memset(&sendresult,0,sizeof(PLUGIN_INTER_STRUCT_T));
	
	if(peer)
		memcpy(&sendresult.peer_addr,peer,sizeof(NETADDR_T));
	
	sendresult.task_rest_head.packet_type = ptype;
	sendresult.task_rest_head.control_id = cid;
	strcpy(sendresult.task_rest_head.plugin_type,pm->plugin_param.plugin_type);
	strcpy(sendresult.task_rest_head.plugin_name,pm->plugin_param.plugin_name);
	sendresult.task_rest_head.plugin_index = pm->plugin_param.plugin_index;
		
	sendresult.task_rest_head.cont_size = size;
	sendresult.content = (char *)malloc( size);
	if(sendresult.content){
		memcpy(sendresult.content,buff,size);
	}else{
		_LOG_WARNING("malloc failed");
		return 0;
	}
	
	if(pm->outqueue->Push(&sendresult)<=0){
		free(sendresult.content);
		return 0;
	}
	
	return 1;
}

static char *ReadConfigLine(char* &conf,char* &name,char* &value){
	if(conf==NULL || conf[0]==0)
		return NULL;
		
	char *begin = conf;
	char *enter = strchr(begin,'\n');
	
	if(enter){
		*enter = 0;	
		conf = enter+1;
	}else{
		conf += strlen(conf);
	}
	
	name = begin;
		
	char *colon = strchr(begin,':');
	if(colon){
		*colon = 0;
		value = colon+1;
	}else{
		value = NULL;
	}
	
	if(name[0]==0)
		name = NULL;
	
	return begin;
}

static int ParseConfig(char *cfgstr,CURL_CONF_T *config){
	char cfg[strlen(cfgstr)+1];
	strcpy(cfg,cfgstr);
	
	char *ptr = cfg;
	char *name = NULL;
	char *value = NULL;
	
	int ipnum=0;
	
	while(ReadConfigLine(ptr,name,value)){
		if(name==NULL || name[0]==0 || value==NULL || value[0]==0)
			continue;
		
	  	if(strcmp(name,"SRC_IP")==0){
	  		ipnum++;
	  	}
	}
	
	if(ipnum>0)
		config->src_ips = (char (*)[20])new char[ipnum][20];
	
	strcpy(cfg,cfgstr);
	ptr = cfg;
	
	while( ReadConfigLine(ptr,name,value) ){
		if( name==NULL || name[0]==0)
			continue;
			
	    if( strcmp(name,"PARALLEL_LINK_MAX") ==0){
	    	if(value==NULL || value[0]==0)
	    		goto ERROR;
	    	config->parallel_link_max = atoi(value);
	    }else if( strcmp(name,"TIMEOUT") ==0){
	    	if(value==NULL || value[0]==0)
	    		goto ERROR;
	    	config->timeout = atoi(value);
	    }else if( strcmp(name,"SRC_IP") ==0){
	    	if(value==NULL || value[0]==0)
	    		goto ERROR;
	    	strcpy(config->src_ips[config->src_ip_num],value);
	    	config->src_ip_num++;
	    }	
    }
    
    if(config->parallel_link_max<=0)
    	config->parallel_link_max = 100;
    
    if(config->timeout<=0)
    	config->timeout = 10;
    	
	return 1;

ERROR:
	_LOG_FATAL("config error for plugin");
	return -1;
		
}


static size_t OnWriteFile(void *ptr,size_t size,size_t nmemb,void *stream)
{
	CURL_TASKINFO_T *rs = (CURL_TASKINFO_T *)stream;	
	int written=size*nmemb; 
	
	if(written<=0)
		return written;
	
	if( (rs->cont_len+written)>rs->cont_size){
		int nsize = rs->cont_size+(written>10240? written:10240);
			
		rs->content = (char *)realloc(rs->content,nsize);
		if(rs->content==NULL){ 
			_LOG_WARNING("realloc failed");
			return 0;
		}else{
			rs->cont_size = nsize;
		}
	}
	
	memcpy(rs->content+rs->cont_len,ptr,written);
	rs->cont_len += written;
	
	rs->updatetime = time(NULL);
		
	return written;
}


static size_t OnReadHeader( void *ptr, size_t size, size_t nmemb, void *stream){
	CURL_TASKINFO_T *rs = (CURL_TASKINFO_T *)stream;	

	char *header = (char *)ptr;
	char *psize;
	header[size*nmemb-1] = 0;

	 if( (psize= strstr(header,"Content-Length:")) ){
		rs->length = atoi(psize+strlen("Content-Length: "));
	}
	
	rs->updatetime = time(NULL);

	return size*nmemb;
}

static int Start_Task(CURL_THREAD_GLOBAL_T *global,CURL_REQUEST_INFO_T *request)
{
	
	for(int ri=0;ri<global->config.parallel_link_max;ri++) {
		global->cur_taskinfo_index = (global->cur_taskinfo_index+1)%global->config.parallel_link_max;
		
		if (global->taskinfos[global->cur_taskinfo_index].used <=0) { 
			break;
		}
	}
	
	int i = global->cur_taskinfo_index;
	
	memset(&global->taskinfos[i],0,sizeof(CURL_TASKINFO_T));
	
	memcpy(&global->taskinfos[i].request,request,sizeof(CURL_REQUEST_INFO_T));
	
	global->taskinfos[i].easy_handle = curl_easy_init();
	if(	global->taskinfos[i].easy_handle == NULL){
		_LOG_WARNING("curl_easy_init failed");
		return -1;
	}
	
	if( strlen(request->url)>0 )
		curl_easy_setopt(global->taskinfos[i].easy_handle,CURLOPT_URL,request->url);
	
	if(request->action>0){
		curl_easy_setopt(global->taskinfos[i].easy_handle,request->action,1L);
	}
	
	if(strlen(request->sip)<=0){
		global->cur_srcip_index = (global->cur_srcip_index+1)%global->config.src_ip_num;
		strcpy(global->taskinfos[i].sip,global->config.src_ips[global->cur_srcip_index]);
	}else{
		strcpy(global->taskinfos[i].sip,request->sip);
	}
	
	curl_easy_setopt(global->taskinfos[i].easy_handle,CURLOPT_INTERFACE,global->taskinfos[i].sip);

	if(request->headlist)
		curl_easy_setopt(global->taskinfos[i].easy_handle,CURLOPT_HTTPHEADER,request->headlist);
		
	if( strlen(request->cookie)>0)
		curl_easy_setopt(global->taskinfos[i].easy_handle,CURLOPT_COOKIE,request->cookie);
	
	if(request->write_callback){
		curl_easy_setopt(global->taskinfos[i].easy_handle,CURLOPT_WRITEFUNCTION,request->write_callback);
		curl_easy_setopt(global->taskinfos[i].easy_handle,CURLOPT_WRITEDATA,request->requestdata);
	}else{
		curl_easy_setopt(global->taskinfos[i].easy_handle,CURLOPT_WRITEFUNCTION,OnWriteFile);
		curl_easy_setopt(global->taskinfos[i].easy_handle,CURLOPT_WRITEDATA,(void *)&global->taskinfos[i]);
	}
	
	if(request->read_callback){
		curl_easy_setopt(global->taskinfos[i].easy_handle,CURLOPT_READFUNCTION,request->read_callback);
		curl_easy_setopt(global->taskinfos[i].easy_handle, CURLOPT_READDATA,request->requestdata);
	}
	
	if(request->header_callback){
		curl_easy_setopt(global->taskinfos[i].easy_handle,CURLOPT_HEADERFUNCTION,request->header_callback);
		curl_easy_setopt(global->taskinfos[i].easy_handle, CURLOPT_WRITEHEADER,request->requestdata);
	}else{
		curl_easy_setopt(global->taskinfos[i].easy_handle,CURLOPT_HEADERFUNCTION,OnReadHeader);
		curl_easy_setopt(global->taskinfos[i].easy_handle, CURLOPT_WRITEHEADER,(void *)&global->taskinfos[i]);
	}

	
	if(strlen(request->proxy_ip)>0){
		if(request->proxy_type>=0)
			curl_easy_setopt(global->taskinfos[i].easy_handle,CURLOPT_PROXYTYPE,request->proxy_type);
		
		curl_easy_setopt(global->taskinfos[i].easy_handle, CURLOPT_PROXY, request->proxy_ip);
		
		if(request->proxy_port>0)
			curl_easy_setopt(global->taskinfos[i].easy_handle, CURLOPT_PROXYPORT, request->proxy_port);
		
		if(strlen(request->proxy_username)>0)
			curl_easy_setopt(global->taskinfos[i].easy_handle, CURLOPT_PROXYUSERNAME, request->proxy_username);
		
		if( strlen(request->proxy_passwd)>0)
			curl_easy_setopt(global->taskinfos[i].easy_handle, CURLOPT_PROXYPASSWORD, request->proxy_passwd);
	}		
		
	curl_easy_setopt(global->taskinfos[i].easy_handle,CURLOPT_CONNECTTIMEOUT_MS, 3000);			
	curl_easy_setopt(global->taskinfos[i].easy_handle, CURLOPT_FOLLOWLOCATION,1);
//	curl_easy_setopt(global->taskinfos[i].easy_handle, CURLOPT_REDIR_PROTOCOLS, CURLPROTO_HTTP | CURLPROTO_HTTPS);
	curl_easy_setopt(global->taskinfos[i].easy_handle, CURLOPT_FRESH_CONNECT, 1);
	curl_easy_setopt(global->taskinfos[i].easy_handle, CURLOPT_FORBID_REUSE, 1);
	curl_easy_setopt(global->taskinfos[i].easy_handle,CURLOPT_DNS_CACHE_TIMEOUT,0);
	curl_easy_setopt(global->taskinfos[i].easy_handle,CURLOPT_DNS_USE_GLOBAL_CACHE,0);
	curl_easy_setopt(global->taskinfos[i].easy_handle,CURLOPT_TCP_NODELAY, 1);
	curl_easy_setopt(global->taskinfos[i].easy_handle,CURLOPT_NOSIGNAL,1);
	curl_easy_setopt(global->taskinfos[i].easy_handle,CURLOPT_VERBOSE,1);
	
	if(request->setopt_function){
		request->setopt_function(global->taskinfos[i].easy_handle,request->requestdata);
	}
			
	curl_multi_add_handle(global->multi_handle,global->taskinfos[i].easy_handle);
	
	global->taskinfos[i].used = 1;
	
	global->run_task_num++;

	return 1;
}


static int Timeout_Task(CURL_THREAD_GLOBAL_T *global,time_t nowtime) {
		
	for(int i=0;i<global->config.parallel_link_max;i++) {
		if(global->taskinfos[i].used ==0 || (nowtime - global->taskinfos[i].updatetime)<= global->config.timeout){
			continue;
		}else{			
			global->Plugin_Pro_Result(&global->tpp,global->taskinfos[i].easy_handle,&global->taskinfos[i].request,
								CURLE_CUSTERM_TIMEOUT,global->taskinfos[i].content,global->taskinfos[i].cont_len,&global->userdata);
			
			global->Plugin_Release_Request(&global->tpp,&global->taskinfos[i].request,&global->userdata);
			curl_multi_remove_handle(global->multi_handle,global->taskinfos[i].easy_handle);
			Release_CURL_TASKINFO_T(&global->taskinfos[i]);
			global->taskinfos[i].used = 0;
			global->run_task_num--;
		}
	}
	
	return 1;
}


int InitGlobal(CURL_THREAD_GLOBAL_T *global){
	
	global->multi_handle=curl_multi_init();
	if(global->multi_handle==NULL){
		_LOG_FATAL("curl_multi_init failed");
		return -1;
	}
	
	global->taskinfos=new CURL_TASKINFO_T[global->config.parallel_link_max];
		
	if(global->taskinfos==NULL){
		_LOG_FATAL("new CURL_TASKINFO_T[] failed");
		return -1;
	}
	
	for(int i=0;i<global->config.parallel_link_max;i++) {
		memset(&global->taskinfos[i],0,sizeof(CURL_TASKINFO_T));
	}
	
	dlerror();
   	global->dl_handle = dlopen(global->tpp.plugin_param.plugin_file,RTLD_NOW);
    if(global->dl_handle==NULL){
        _LOG_FATAL("dlopen %s failed. error=%s.",global->tpp.plugin_param.plugin_file,dlerror());
        return -1;
    }

    dlerror();
    global->Plugin_Init = (PLUGIN_INIT_FP*)dlsym(global->dl_handle,"Plugin_Init");
    global->Plugin_Release = (PLUGIN_RELEASE_FP*)dlsym(global->dl_handle,"Plugin_Release");
    global->Plugin_Recv_Task = (PLUGIN_RECV_TASK_FP*)dlsym(global->dl_handle,"Plugin_Recv_Task");
    global->Plugin_Release_Request = (PLUGIN_RELEASE_REQUEST_FP*)dlsym(global->dl_handle,"Plugin_Release_Request");
    global->Plugin_Get_Request = (PLUGIN_GET_REQUEST_FP*)dlsym(global->dl_handle,"Plugin_Get_Request");
    global->Plugin_Pro_Result = (PLUGIN_PRO_RESULT_FP*)dlsym(global->dl_handle,"Plugin_Pro_Result");
    global->Plugin_Period = (PLUGIN_PERIOD_FP*)dlsym(global->dl_handle,"Plugin_Period");
   
    if( global->Plugin_Init==NULL || global->Plugin_Release==NULL ||global->Plugin_Recv_Task==NULL || 
    	global->Plugin_Release_Request ==NULL || global->Plugin_Get_Request==NULL || 
		global->Plugin_Pro_Result==NULL || global->Plugin_Period==NULL){
        _LOG_WARNING("dlsym Plugin_Init failed in %s. error=%s.",global->tpp.plugin_param.plugin_file,dlerror());
        return -1;
    }
    
    return 1;
}


static int Multi_Select(CURLM *curl_m)
{
	int ret = 0;

	struct timeval timeout_tv;
	fd_set  fd_read;
	fd_set  fd_write;
	fd_set  fd_except;
	int     max_fd = -1;
	int stillrunning;
	long curl_timeo = -1;

	// 注意这里一定要清空fdset,curl_multi_fdset不会执行fdset的清空操作  //
	//
	FD_ZERO(&fd_read);
	FD_ZERO(&fd_write);
	FD_ZERO(&fd_except);

	// 设置select超时时间  //
	timeout_tv.tv_sec = 1;
	timeout_tv.tv_usec = 0;

	curl_multi_timeout(curl_m, &curl_timeo);
	if(curl_timeo >= 0) {
		timeout_tv.tv_sec = curl_timeo / 1000;
		if(timeout_tv.tv_sec > 1) {
			timeout_tv.tv_sec = 1;
		} else {
			timeout_tv.tv_usec = (curl_timeo % 1000) * 1000;
		}
	}

	// 获取multi curl需要监听的文件描述符集合 fd_set //
	if (curl_multi_fdset(curl_m, &fd_read, &fd_write, &fd_except, &max_fd) != CURLM_OK) {
		_LOG_WARNING("curl_multi_fdset failed");
		return -1;
	}

	/**
	 * 执行监听，当文件描述符状态发生改变的时候返回
	 * 返回0，程序调用curl_multi_perform通知curl执行相应操作
	 * 返回-1，表示select错误
	 * 注意：即使select超时也需要返回0，具体可以去官网看文档说明
	 */
	int ret_code = ::select(max_fd + 1, &fd_read, &fd_write, &fd_except, &timeout_tv);
	switch(ret_code)
	{
		case -1:
			/* select error */
			_LOG_WARNING("::select error %s", strerror(errno));
			ret = -1;
			break;
		case 0:
		default:
			/* one or more of curl's file descriptors say there's data to read or write*/
			while(1) {
				CURLMcode ret;
				ret = curl_multi_perform(curl_m, &stillrunning);
				if (ret == CURLM_CALL_MULTI_PERFORM) {
					continue;
				} else {
					break;
				}
			}
			ret = 0;
			break;
	}

	return ret;
}

void* Thread_Main(void *param){
	
	int tcount = 0;
	
	time_t oldtime, nowtime;

	oldtime = time(NULL);
	nowtime = oldtime;
	
	if(param==NULL){
		_LOG_FATAL("THREAD_MAIN get NULL param");
		return NULL;
	}
	
	CURL_THREAD_GLOBAL_T	g_global;
	memset(&g_global,0,sizeof(g_global));
	
	memcpy(&g_global.tpp,param,sizeof(THREAD_PLUGIN_PARAM_T));
	
	char threadname[64];
	sprintf(threadname,"THREAD_MAIN_%s_%s_%d",
				g_global.tpp.plugin_param.plugin_type,g_global.tpp.plugin_param.plugin_name,
				g_global.tpp.plugin_param.plugin_index);
	
	ul_openlog_r(threadname,g_global.tpp.g_logstats, NULL);
	
	if(ParseConfig(g_global.tpp.plugin_param.config,&g_global.config)<=0){
		_LOG_FATAL("ParseConfig failed for plugin %s",g_global.tpp.plugin_param.plugin_name);
		return NULL;
	}
	
	if(InitGlobal(&g_global)<=0)
		goto ERROR;
	
	if(g_global.Plugin_Init(&g_global.tpp,&g_global.tpp.plugin_param,Send_Result,g_global.tpp.plugin_param.config,&g_global.userdata)<=0){
		_LOG_FATAL("Plugin_Init failed for plugin %s",g_global.tpp.plugin_param.plugin_name);
		goto ERROR;
	}
	
	while(1)
	{
		tcount++;
		
		if(tcount%100==0){
			g_global.Plugin_Period(&g_global.tpp,&g_global.userdata);
		}
		
		nowtime = time(NULL);
		if ( (nowtime - oldtime) >= g_global.config.timeout) {
			oldtime = nowtime;
			Timeout_Task(&g_global,nowtime);
		}
		
		PLUGIN_INTER_STRUCT_T *task = (PLUGIN_INTER_STRUCT_T *)g_global.tpp.inqueue->GetAt(0);
		if(task){
			if(g_global.Plugin_Recv_Task(&g_global.tpp,task,&g_global.userdata)>0){
				Release_PLUGIN_INTER_STRUCT_T(task);
				g_global.tpp.inqueue->Pop(NULL);
			}
		}
		
		if( g_global.run_task_num<g_global.config.parallel_link_max) {
			CURL_REQUEST_INFO_T	request;
			if(g_global.Plugin_Get_Request(&g_global.tpp,&request,&g_global.userdata)>0){
				if(Start_Task(&g_global,&request)<=0){
					g_global.Plugin_Pro_Result(&g_global.tpp,NULL,&request,CURLE_CUSTERM_CANNT_START,NULL,0,&g_global.userdata);
					g_global.Plugin_Release_Request(&g_global.tpp,&request,&g_global.userdata);
				}
			}
		}

		if (g_global.run_task_num <= 0) {
			usleep(1);
			continue;
		}
		
		int still_running = 0;
		
		while(1) {
			CURLMcode ret;
			ret = curl_multi_perform(g_global.multi_handle, &still_running);
			if (ret == CURLM_CALL_MULTI_PERFORM) {
				continue;
			} else {
				break;
			}
		}

		if (still_running) {
			if(-1==Multi_Select(g_global.multi_handle)) {
				usleep(1);
				//continue;
			}
		}

		while (1) {
			int msgs_left;
			CURLMsg *msg = curl_multi_info_read(g_global.multi_handle, &msgs_left);
			if (msg == NULL) {
				break;
			} else if (msg->msg == CURLMSG_DONE)  {

				/* Find out which handle this message is about */ 
				for (int idx=0; idx<g_global.config.parallel_link_max; idx++) {
					if(msg->easy_handle == g_global.taskinfos[idx].easy_handle) {
						
						g_global.Plugin_Pro_Result(&g_global.tpp,g_global.taskinfos[idx].easy_handle,&g_global.taskinfos[idx].request,
								msg->data.result,g_global.taskinfos[idx].content,g_global.taskinfos[idx].cont_len,&g_global.userdata);
						
						g_global.Plugin_Release_Request(&g_global.tpp,&g_global.taskinfos[idx].request,&g_global.userdata);		
						curl_multi_remove_handle(g_global.multi_handle,g_global.taskinfos[idx].easy_handle);
						Release_CURL_TASKINFO_T(&g_global.taskinfos[idx]);
						g_global.taskinfos[idx].used = 0;
						g_global.run_task_num--;
						
						break;
					}
				}
			}
		}
	}
	
ERROR:
	
	if(g_global.Plugin_Release){
		g_global.Plugin_Release(&g_global.tpp,&g_global.userdata);
	}
	
	if(g_global.dl_handle)
		dlclose(g_global.dl_handle);
		
	for(int i=0;i<g_global.config.parallel_link_max;i++) {
		if(g_global.taskinfos[i].used!=0) {
			if(g_global.taskinfos[i].easy_handle)
				curl_multi_remove_handle(g_global.multi_handle,g_global.taskinfos[i].easy_handle);
			
			Release_CURL_TASKINFO_T(&g_global.taskinfos[i]);
			g_global.taskinfos[i].used = 0;
			g_global.run_task_num--;
		}
	}
	
	if(g_global.multi_handle)
		curl_multi_cleanup(g_global.multi_handle);
	
	if(g_global.config.src_ips)
		delete[] g_global.config.src_ips;
	
	return NULL;
}


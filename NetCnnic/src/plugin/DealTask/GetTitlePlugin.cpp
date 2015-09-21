/*************************************************************************
    > File Name: GetTitlePlugin.cpp
    > Author: lxf
    > Mail: blueskytoday@126.com 
    > Created Time: Fri 08 May 2015 08:33:30 AM CST
 ************************************************************************/

#include <map>
#include <iostream>
#include <curl/curl.h>
#include <string>
#include <iconv.h>

#include "log.h"
#include "GetTitlePlugin.h"
#include "CFIFOQueue.h"
using namespace std;

#define	MAX_REQUEST_SIZE	1024
#define	MAX_TASK_SIZE		10

const int RETRY_TIMES = 2;

typedef struct{
	CFIFOQueue *task_queue;
	CFIFOQueue *result_queue;
	CURL_REQUEST_INFO_T		requests[MAX_REQUEST_SIZE];
	PLUGIN_SEND_RESULT_FP	*Send_Result;
	uint32_t	maxid;
	uint32_t	requestid;
	time_t		requesttime;
	PLUGIN_INTER_STRUCT_T pst;

	map<string,string> task_info;	//record the filename of task
	/*
	 * 0	http:80
	 * 1	+=http:8080
	 * 2	+=http:3128
	 * 3	+=http:8081
	 * 4	+=http:9080
	 * 5	+=https
	 */
	map<string,int> task_stat;	//record the stat of task
	map<string,int> task_times;	//record the times of task
}GETTITLE_GLOBAL_T;

int Setopt_Function(CURL *urlhandle,void *userdata){
	cout<<"[Debug]in "<<__func__<<endl;
	//char *curl_error = new char[CURL_ERROR_SIZE];
	curl_easy_setopt(urlhandle, CURLOPT_ACCEPT_ENCODING, "gzip,deflate");
	//curl_easy_setopt(urlhandle, CURLOPT_ERRORBUFFER, curl_error);
	return 1;
}

/*
 * Desc:init plugin
 */
int Plugin_Init(void *handle,PLUGIN_PARAM_T *param,PLUGIN_SEND_RESULT_FP send_rst,char *config,void **userdata){
	cout<<"[Debug]in "<<__func__<<endl;
	//return 1;
	GETTITLE_GLOBAL_T *gettitle_data = new GETTITLE_GLOBAL_T;
	if (NULL == gettitle_data){
		_LOG_FATAL("in %s:new gettitle_data failed.", __func__);
		return -1;
	}
	//bzero(gettitle_data, sizeof(GETTITLE_GLOBAL_T)-sizeof(map<string,string>));

	if (!param){
		_LOG_FATAL("in %s:param is null.", __func__);
		return -1;
	}

	gettitle_data->task_queue = new CFIFOQueue(MAX_TASK_SIZE,sizeof(PLUGIN_INTER_STRUCT_T));
	gettitle_data->result_queue = new CFIFOQueue(MAX_TASK_SIZE,sizeof(PLUGIN_INTER_STRUCT_T));

	gettitle_data->Send_Result = send_rst;
	*userdata = gettitle_data;

	return 1;
}

/*
 * Desc:do when plugin exit
 */
int Plugin_Release(void *handle,void **userdata){
	cout<<"[MYDebug]in "<<__func__<<endl;
	GETTITLE_GLOBAL_T *gettitle_data = (GETTITLE_GLOBAL_T *)(*userdata);
	if (gettitle_data){
		if (gettitle_data->task_queue){
			PLUGIN_INTER_STRUCT_T task;
			while (gettitle_data->task_queue->Pop(&task)>0){
				Release_PLUGIN_INTER_STRUCT_T(&task);
			}
			delete gettitle_data->task_queue;
		}
		if (gettitle_data->result_queue){
			PLUGIN_INTER_STRUCT_T result;
			while (gettitle_data->result_queue->Pop(&result)>0){
				Release_PLUGIN_INTER_STRUCT_T(&result);
			}
			delete gettitle_data->result_queue;
		}
		delete gettitle_data;
	}
	return 1;
}

int Plugin_Recv_Task(void *handle,PLUGIN_INTER_STRUCT_T *task,void **userdata){
	//cout<<"[MYDebug]in "<<__func__<<":"<<task->content+sizeof(uint32_t)<<endl;
	GETTITLE_GLOBAL_T *gettitle_data = (GETTITLE_GLOBAL_T *)(*userdata);

	if(task->task_rest_head.packet_type!=PACKET_TYPE_TASK)
		return 1;
	
	if(task->task_rest_head.cont_size<=sizeof(uint32_t))
		return 1;

	uint32_t	requestid = ntohl( *((uint32_t*)task->content));
	if(requestid!=gettitle_data->requestid)
		return 1;

	char *sptr = (char *)task->content+sizeof(uint32_t);
	char *eptr = (char *)task->content+task->task_rest_head.cont_size;

	while(sptr<eptr){
		PLUGIN_INTER_STRUCT_T newtask;
		int urllen = strlen(sptr);
		//*
		memcpy(&newtask,task,sizeof(PLUGIN_INTER_STRUCT_T));
		//set pst
		memcpy(&(gettitle_data->pst),task,sizeof(PLUGIN_INTER_STRUCT_T));
		newtask.content = (char *)malloc(urllen+1);
		if(newtask.content==NULL){
			_LOG_WARNING("malloc failed");
			return -1;
		}
		//task format:filename|line
		//newd del filname|
		char *tmp_cont = new char[urllen+1];
		if (NULL == tmp_cont){
			_LOG_WARNING("new filaed.");
			return -1;
		}
		tmp_cont[urllen] = '\0';
		char *p_file, *p_line, *p_save;
		//cout<<"[Debug]sptr:"<<sptr<<endl;
		strcpy(tmp_cont, sptr);
		p_file = strtok_r(tmp_cont, "|", &p_save);
		p_line = strtok_r(NULL, "|", &p_save);
		//insert file,task
		string str_file = p_file;
		string str_line = p_line;
		//cout<<"[Debug]str:"<<str_file<<"\t"<<str_line<<endl;

		strcpy(newtask.content, p_line);
		newtask.task_rest_head.cont_size = strlen(p_line);
		/*/
		newtask.content = (char *)malloc(100);
		strcpy(newtask.content, (char *)"www.baidu.com");
		newtask.task_rest_head.cont_size = 14;
		*/
		
		if(gettitle_data->task_queue->Push(&newtask)<=0){
			_LOG_WARNING("Push task failed");
			free(newtask.content);
			return -1;
		}
		_LOG_WARNING("new task:%s", newtask.content);
		//gettitle_data->task_times[str_line] = 1;
		gettitle_data->task_info.insert(pair<string,string>(str_line, str_file));
		gettitle_data->task_stat.insert(pair<string,int>(str_line, 0));
		gettitle_data->task_times.insert(pair<string,int>(str_line, 1));
		sptr += urllen+1;
		//delete []tmp_cont;
	}

	gettitle_data->requesttime = 0;
	return 1;
}

static int times = 1;

int Plugin_Get_Request(void *handle,CURL_REQUEST_INFO_T *request,void **userdata){
	//cout<<"[MYDebug]in "<<__func__<<endl;
	GETTITLE_GLOBAL_T *gettitle_data = (GETTITLE_GLOBAL_T*)(*userdata);
	//if (0 == times)	return 0;
	
	if(gettitle_data->task_queue->GetAvilable()>0){
		time_t	now = time(NULL);
		
		if( gettitle_data->requesttime==0 || (now - gettitle_data->requesttime)>3 ){
		
			char content[sizeof(uint32_t)*2];
			//char *content = (char *)malloc(2*sizeof(uint32_t));
		
			gettitle_data->requestid++;
			*((uint32_t*)content) = htonl(gettitle_data->requestid);
			*((uint32_t*)(content+sizeof(uint32_t))) = htonl(gettitle_data->task_queue->GetAvilable());
			gettitle_data->requesttime = time(NULL);	
			
			if (gettitle_data->Send_Result(handle,PACKET_TYPE_REQUEST,NULL,0,content,sizeof(uint32_t)*2) <= 0){
				//free(content);
			}
		}
	}
	
	PLUGIN_INTER_STRUCT_T *newtask;
	
	if( (newtask=(PLUGIN_INTER_STRUCT_T *)gettitle_data->task_queue->GetAt(0))!=NULL){
		times = 0;
		memset(request,0,sizeof(CURL_REQUEST_INFO_T));
		
		gettitle_data->maxid++;
		if(gettitle_data->maxid==0)
			gettitle_data->maxid++;
		
		request->id = gettitle_data->maxid;
		memcpy(request->url,newtask->content,newtask->task_rest_head.cont_size);
		request->url[newtask->task_rest_head.cont_size]=0;
		
		request->requestdata = malloc(sizeof(PLUGIN_INTER_STRUCT_T));
		if(request->requestdata!=NULL){
			memcpy(request->requestdata,newtask,sizeof(PLUGIN_INTER_STRUCT_T));
			gettitle_data->task_queue->Pop(NULL);
			return 1;
		}else{
			_LOG_WARNING("malloc PLUGIN_INTER_STRUCT_T failed");
			return 0;
		}
	}
	/*
	memset(request,0,sizeof(CURL_REQUEST_INFO_T));
	if (times == 1){
		strcpy(request->url, "www.baidu.com");
		times = 0;
		return 1;
	}
	*/
	return 0;
	
	//return 0;
}

int Plugin_Release_Request(void *handle,CURL_REQUEST_INFO_T *request,void **userdata){
	if(request->requestdata){
		Release_PLUGIN_INTER_STRUCT_T((PLUGIN_INTER_STRUCT_T *)request->requestdata);
	}
	return 1;
}

/*
 * revert charset
*/
int code_convert(char *from_charset,char *to_charset,char *inbuf, size_t inlen,char *outbuf, size_t outlen)
{

	iconv_t cd;
	char **pin = &inbuf;
	char **pout = &outbuf;

	cd = iconv_open(to_charset,from_charset);
	if (cd==0){
		//cout<<"iconv_open error.\n";
		return -1;
	}
	memset(outbuf,0,outlen);
	if (iconv(cd, pin, &inlen,pout, &outlen)==-1) {
		//cout<<"iconv error.\n";
		return -1;
	}
	iconv_close(cd);
	return 0;
}

/*
 * get title
 */
int predeal_content(char *buff, string &str_content){
	//find charset
    //cout<<buff<<endl;
    str_content="";
	char charset[20] = "";
	char *p_charset_head, *p_charset_end;
	if (p_charset_head = strstr(buff, "charset=")){
		p_charset_head += 8;
		p_charset_end = strstr(p_charset_head, "\"");
		if (!p_charset_end || (p_charset_end - p_charset_head) >=20){
			//bad end of charset
			return 0;
		}
		strncpy(charset, p_charset_head, p_charset_end - p_charset_head);
		cout<<"charset:"<<charset<<endl;
	} else{
		//not find charset, use default utf-8
		strcpy(charset, "no-charset");
		//return 0;
	}
	char *content_head,*content_end,*temp=buff;
    char *line_head,*line_end,*line_content;
    char ip_content[1024];
    content_head=strstr(temp,"<body>");
    content_end=strstr(temp,"</body>");
    int i=0,flag=0;
    while(temp<content_end){
        if (flag==0&&(NULL != (line_head = strstr(temp, "<font size=\"2\">"))) && (NULL != (line_end = strstr(temp, "</font></td>"))) && ((line_end-line_head)>1)){
		    line_head += 15;
            int line_content_len=line_end-line_head+1;
            line_content = new char[line_content_len];
            memcpy(line_content, line_head, line_end-line_head);
	        line_content[line_end-line_head] = '\0';
            //cout<<"hello"<<line_content<<endl;
            str_content=str_content+line_content+(string)"\t";
            delete []line_content;
            flag=1;
            temp=line_end+10;
        }
        //temp=line_end+10;
        if (flag==1&&(NULL != (line_head = strstr(temp, "<font size=\"2\">"))) && (NULL != (line_end = strstr(temp, "&nbsp;</font></td>"))) && ((line_end-line_head)>1)){
		    line_head += 15;
            int line_content_len=line_end-line_head+1;
            line_content = new char[line_content_len];
            memcpy(line_content, line_head, line_end-line_head);
	        line_content[line_end-line_head] = '\0';
            if(i==0){
                strcpy(ip_content,line_content);
                //cout<<"[Debug in for pre ip_content]"<<ip_content<<endl;
            }
            str_content=str_content+line_content+(string)"\n";
	        delete []line_content;
            flag=0;
            temp=line_end+10;
            i++;
        }
        temp=temp+10;
        //cout<<"hello"<<str_content<<endl;
        //cout<<i<<endl;
        //cout<<"hirock\n\n"<<endl;
    }
    str_content = str_content + (string)"|"+ ip_content;
    //cout<<"[Debug pre ip_content]"<<ip_content<<endl;
    //cout<<"[Debug pre trans:]"<<str_content<<"[Debug pre trans:]"<<endl;
    //cout<<i<<endl;
    //cout<<"hello\n"<<str_content<<endl;
	/*
	if (0 != strcmp(charset, "utf-8")){
		cout<<"[Info]not UTF-8:"<<charset<<"\t"<<strlen(charset)<<endl;
		cout<<title<<endl;
		char *new_title = new char[title_len];
		if (-1 != code_convert(charset, "utf-8", title, title_len, new_title, title_len)){
			//code_convert failed
			str_title = new_title;
		}
		delete []new_title;
	}
	*/
	
	return 1;
}

/*
 * strip https:// :[port] from url
 */

string PredealURL(char *url){
	//cout<<"bef: "<<url<<endl;

	string str_url;
	char *p1;

	//find https://
	p1 = strstr(url, "https://");
	if (p1){
		return str_url = url+8;
	}

	p1 = strstr(url, ":");
	if (p1){
		//eg:www.baidu.com:8081
		*p1 = '\0';
		return str_url = url;
	}
	//cout<<"after: "<<url<<"\t"<<str_url<<endl;
	//p2 = strstr(url,"txtquery=");
    //return str_url = p2+9;
    return str_url=url;
}

int Plugin_Pro_Result(void *handle,CURL *urlhandle,CURL_REQUEST_INFO_T *request,int result,char *buff,int size,void **userdata){
	GETTITLE_GLOBAL_T *gettitle_data = (GETTITLE_GLOBAL_T *)(*userdata);
	//cout<<"[Debug]in "<<__func__<<endl;

	int cont_size = 0;
	char *content = NULL;

	Setopt_Function(urlhandle, *userdata);

	if(result==CURLE_OK){
		string str_ip;
		//char IP[20] = "";
		//get_title(buff);
		string str_title;
		if (!buff || !predeal_content(buff, str_title)){
			_LOG_WARNING("no title in buff.");
			//return 0;
			str_title = "failed";
		}
		char *IP = NULL;
		curl_easy_getinfo(urlhandle, CURLINFO_PRIMARY_IP, &IP);
		if (NULL != IP){
			str_ip += IP;
			//str_title = str_ip+"\t"+str_title;
		}
		
		//get task
		string tmp_line = PredealURL(request->url);
		string file;
		if (gettitle_data->task_info.find(tmp_line) != gettitle_data->task_info.end()){
			file = gettitle_data->task_info[tmp_line];
		}else{
			file = "unkonwn file";
			//return 0;
		}
		//result format:
		//		filename|line|result
        //cout<<"[Debug file tmp_line str_line]:"<<file<<"\n"<<tmp_line<<"\n"<<endl;
        string tmp_p1;
        int pos=tmp_line.find("txtquery=");
        pos += 9;
        tmp_p1=tmp_line.substr(pos);
        //cout<<"[Debug file tmp_line str_line]:"<<tmp_p1<<tmp_line<<"\n"<<endl;
        string tmp_content = file + "|" + tmp_p1 + "|" + str_title;
		cont_size = sizeof(uint32_t)*2+tmp_content.length()+1;
		content = (char *)malloc(cont_size);
		if (!content){
			_LOG_FATAL("in %s:new title failed.", __func__);
			return 0;
		}
		*((uint32_t*)content) = htonl(0);
		*((uint32_t*)(content+sizeof(uint32_t))) = htonl(0);
		//cout<<"[Debug]result:"<<tmp_content<<endl;
		bzero(content, cont_size);
		memcpy(content+sizeof(uint32_t)*2,tmp_content.c_str(),tmp_content.length());
	}else{
		_LOG_WARNING("[Error][%d]%s, buff is %s", result, request->url, buff);

		string tmp_url = request->url;
		if (RETRY_TIMES >= gettitle_data->task_times[tmp_url]){
			PLUGIN_INTER_STRUCT_T newtask = gettitle_data->pst;
			newtask.content = (char *)malloc(tmp_url.length()+1);
			strcpy(newtask.content, tmp_url.c_str());
			newtask.task_rest_head.cont_size = tmp_url.length();
			if(gettitle_data->task_queue->Push(&newtask)<=0){
				_LOG_WARNING("Push task failed");
				free(newtask.content);
				return -1;
			}
			_LOG_WARNING("new retry_task:[%d]%s", gettitle_data->task_times[tmp_url], newtask.content);
			gettitle_data->task_times[tmp_url]++;
			return 1;
		}
		string tmp_line = PredealURL(request->url);
		//cout<<"[Debug]tmp_line = "<<tmp_line<<"\t"<<gettitle_data->task_stat[tmp_line]<<endl;
		if ((gettitle_data->task_stat.find(tmp_line) != gettitle_data->task_stat.end()) &&(5 > gettitle_data->task_stat[tmp_line])){
			PLUGIN_INTER_STRUCT_T newtask = gettitle_data->pst;
            string tmp_p1;
            int pos=tmp_line.find("txtquery=");
            pos += 9;
            tmp_p1=tmp_line.substr(pos);
			//string new_url = tmp_line;
            string new_url = tmp_p1;

			if (0 == gettitle_data->task_stat[tmp_line]){
				new_url += ":8080";
				gettitle_data->task_stat[tmp_line] = 1;
			} else if (1 == gettitle_data->task_stat[tmp_line]){
				new_url += ":3128";
				gettitle_data->task_stat[tmp_line] = 2;
			} else if (2 == gettitle_data->task_stat[tmp_line]){
				new_url += ":8081";
				gettitle_data->task_stat[tmp_line] = 3;
			} else if (3 == gettitle_data->task_stat[tmp_line]){
				new_url += ":9080";
				gettitle_data->task_stat[tmp_line] = 4;
			} else if (4 == gettitle_data->task_stat[tmp_line]){
				new_url = "https://"+new_url;
				gettitle_data->task_stat[tmp_line] = 5;
			}

			newtask.content = (char *)malloc(new_url.length()+1);
			strcpy(newtask.content, new_url.c_str());
			newtask.task_rest_head.cont_size = new_url.length();
			if(gettitle_data->task_queue->Push(&newtask)<=0){
				_LOG_WARNING("Push task failed");
				free(newtask.content);
				return -1;
			}
			_LOG_WARNING("new change port task:%s", newtask.content);
			//set times
			gettitle_data->task_times[new_url] = 1;
			return 1;
		}
		//*
		//get task
		//string tmp_line = request->url;
		string file;
		if (gettitle_data->task_info.find(tmp_line) != gettitle_data->task_info.end()){
			file = gettitle_data->task_info[tmp_line];
		}else{
			file = "unkonwn file";
			//return 0;
		}
		char tmp_result[10];
		snprintf(tmp_result, 10, "%d", result);
		string result = file+"|"+tmp_line+"|error"+(string)tmp_result;
		cont_size = sizeof(uint32_t)*2 +result.length()+1;
		content = (char *)malloc(cont_size);
		/*/
		cont_size = sizeof(uint32_t)*2;
		content = (char *)malloc(cont_size);
		*/
		if(content){
			*((uint32_t*)content) = htonl(0);
			*((uint32_t*)(content+sizeof(uint32_t))) = htonl(0);
			strcpy(content+2*sizeof(uint32_t), result.c_str());
		}else{
			return 0;
		}
	}
	

	PLUGIN_INTER_STRUCT_T *task = (PLUGIN_INTER_STRUCT_T*)request->requestdata;
	if(gettitle_data->Send_Result(handle,PACKET_TYPE_RESULT,&task->peer_addr,
						task->task_rest_head.control_id,content,cont_size)<=0){
        //cout<<"[Debug write test]:"<<content<<endl;
	}
	free(content);
	return 1;
}

int Plugin_Period(void *handle,void **userdata){
	return 1;
}


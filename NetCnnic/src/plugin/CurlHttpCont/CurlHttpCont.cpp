
#include <curl/curl.h>

#include "log.h"
#include "CurlHttpCont.h"
#include "CFIFOQueue.h"

#define	MAX_REQUEST_SIZE	1024
#define	MAX_PEER_NUM		1024
#define	MAX_TASK_SIZE		10

typedef struct{
	CFIFOQueue	*task_queue;
	CFIFOQueue	*result_queue;
	PLUGIN_PARAM_T param;
	CURL_REQUEST_INFO_T		requests[MAX_REQUEST_SIZE];
	PLUGIN_SEND_RESULT_FP	*Send_Result;
	uint32_t	maxid;
	uint32_t	requestid;
	time_t		requesttime;
}CURLHTTP_GLOBAL_T;


typedef	struct{
	PLUGIN_INTER_STRUCT_T	task;
	char					encoding[64];
}USERDATA_T; 

/*int Setopt_Function(CURL *urlhandle,void *userdata){
	curl_easy_setopt(urlhandle, CURLOPT_ACCEPT_ENCODING, "gzip,deflate");
	return 1;
}*/


size_t header_callback(char *buffer,   size_t size,   size_t nitems,   void *requestdata){
	if(buffer && size*nitems>0){
		buffer[size*nitems-1]=0;
		
		char *end = strstr(buffer,"Content-Encoding:");
		if(end!=NULL){
			end+=strlen("Content-Encoding:");
			while(*end==' ')
				end++;
			
			while(end[strlen(end)-1]=='\n' ||end[strlen(end)-1]=='\r' || end[strlen(end)-1]==' ')
				end[strlen(end)-1]=0;

			USERDATA_T *ud = (USERDATA_T*)requestdata;
			strcpy(ud->encoding,end);

		}
	}
	return size*nitems;
}



int Plugin_Init(void *handle,PLUGIN_PARAM_T *param,PLUGIN_SEND_RESULT_FP *send_rst,char *config,void **userdata){
	CURLHTTP_GLOBAL_T *http_global = (CURLHTTP_GLOBAL_T*)malloc(sizeof(CURLHTTP_GLOBAL_T));
	
	if(http_global==NULL){
		_LOG_FATAL("malloc failed");
		return -1;
	}
	
	memset(http_global,0,sizeof(CURLHTTP_GLOBAL_T));
	
	memcpy(&http_global->param,param,sizeof(PLUGIN_PARAM_T));
	
	http_global->task_queue = new CFIFOQueue(MAX_TASK_SIZE,sizeof(PLUGIN_INTER_STRUCT_T));
	http_global->result_queue = new CFIFOQueue(MAX_TASK_SIZE,sizeof(PLUGIN_INTER_STRUCT_T));

	if(http_global->task_queue==NULL || http_global->task_queue==NULL){
		_LOG_FATAL("new CFIFOQueue failed");
		return -1;
	}
	
	http_global->Send_Result = send_rst;
	*userdata = http_global;
		
	return 1;
}

int Plugin_Release(void *handle,void **userdata){
	CURLHTTP_GLOBAL_T *http_global = (CURLHTTP_GLOBAL_T*)(*userdata);
	
	if(http_global){
		if(http_global->task_queue){
			PLUGIN_INTER_STRUCT_T	task;
			while(http_global->task_queue->Pop(&task)>0){
				Release_PLUGIN_INTER_STRUCT_T(&task);
			}
			delete http_global->task_queue;
		}
		if(http_global->result_queue){
			PLUGIN_INTER_STRUCT_T	result;
			while(http_global->result_queue->Pop(&result)>0){
				Release_PLUGIN_INTER_STRUCT_T(&result);
			}
			delete http_global->result_queue;
		}
		
		free(http_global);
	}
	
	return 1;
}


int Plugin_Recv_Task(void *handle,PLUGIN_INTER_STRUCT_T *task,void **userdata){
	CURLHTTP_GLOBAL_T *http_global = (CURLHTTP_GLOBAL_T*)(*userdata);
	
	if(task->task_rest_head.packet_type!=PACKET_TYPE_TASK)
		return 1;
	
	if(task->task_rest_head.cont_size<=sizeof(uint32_t))
		return 1;
	
	uint32_t	requestid = ntohl( *((uint32_t*)task->content));
	if(requestid!=http_global->requestid)
		return 1;
		
	char *sptr = (char *)task->content+sizeof(uint32_t);
	char *eptr = (char *)task->content+task->task_rest_head.cont_size;
	
	while(sptr<eptr){
		PLUGIN_INTER_STRUCT_T newtask;
		memcpy(&newtask,task,sizeof(PLUGIN_INTER_STRUCT_T));
		int urllen = strlen(sptr);
		newtask.content = (char *)malloc(urllen+1);
		if(newtask.content==NULL){
			_LOG_WARNING("malloc failed");
			return -1;
		}
		
		strcpy(newtask.content,sptr);
		
		if(http_global->task_queue->Push(&newtask)<=0){
			_LOG_WARNING("Push task failed");
			free(newtask.content);
			return -1;
		}
		sptr += urllen+1;
	}
	
	http_global->requesttime = 0;
	
	return 1;
}

int Plugin_Get_Request(void *handle,CURL_REQUEST_INFO_T *request,void **userdata){
	CURLHTTP_GLOBAL_T *http_global = (CURLHTTP_GLOBAL_T*)(*userdata);
	
	if(http_global->task_queue->GetAvilable()>0){
		time_t	now = time(NULL);
		
		if( http_global->requesttime==0 || (now-http_global->requesttime)>3 ){
		
			char	content[sizeof(uint32_t)*2];
		
			http_global->requestid++;
			*((uint32_t*)content) = htonl(http_global->requestid);
			*((uint32_t*)(content+sizeof(uint32_t))) = htonl(http_global->task_queue->GetAvilable());
			http_global->requesttime = time(NULL);	
			
			http_global->Send_Result(handle,PACKET_TYPE_REQUEST,NULL,0,content,sizeof(uint32_t)*2);
		}
	}
	
	PLUGIN_INTER_STRUCT_T *newtask;
	
	if( (newtask=(PLUGIN_INTER_STRUCT_T *)http_global->task_queue->GetAt(0))!=NULL){
		memset(request,0,sizeof(CURL_REQUEST_INFO_T));
		
		http_global->maxid++;
		if(http_global->maxid==0)
			http_global->maxid++;
		
		request->id = http_global->maxid;
		memcpy(request->url,newtask->content,newtask->task_rest_head.cont_size);
		request->url[newtask->task_rest_head.cont_size]=0;
		request->header_callback = header_callback;
		request->headlist = curl_slist_append(request->headlist,"Accept-Encoding:gzip,deflate");
		request->requestdata = malloc(sizeof(USERDATA_T));
		if(request->requestdata!=NULL){
			USERDATA_T	*ud = (USERDATA_T*)request->requestdata;
			memcpy(&ud->task,newtask,sizeof(PLUGIN_INTER_STRUCT_T));
			ud->encoding[0]=0;
			http_global->task_queue->Pop(NULL);
			return 1;
		}else{
			_LOG_WARNING("malloc PLUGIN_INTER_STRUCT_T failed");
			return 0;
		}
	}
	
	return 0;
}

int Plugin_Release_Request(void *handle,CURL_REQUEST_INFO_T *request,void **userdata){
	
	if(request->requestdata){
		USERDATA_T *ud = (USERDATA_T*)request->requestdata;

		Release_PLUGIN_INTER_STRUCT_T(&ud->task);
		free(request->requestdata);
	}
	
	return 1;
}


int Plugin_Pro_Result(void *handle,CURL *urlhandle,CURL_REQUEST_INFO_T *request,int result,char *buff,int size,void **userdata){
	
	CURLHTTP_GLOBAL_T *http_global = (CURLHTTP_GLOBAL_T*)(*userdata);
	
	int cont_size = 0;
	char *content = NULL;
	
	USERDATA_T	*ud = (USERDATA_T*)request->requestdata;

	if(result==CURLE_OK){
		long httpcode;
		char *encoding;

        curl_easy_getinfo(urlhandle, CURLINFO_RESPONSE_CODE, &httpcode);
		curl_easy_getinfo(urlhandle,CURLINFO_CONTENT_TYPE,&encoding);

		cont_size = sizeof(uint32_t)*3+size;
		content = (char *)malloc( cont_size);
		 if(content){
			*((uint32_t*)content) = htonl(httpcode);
			*((uint32_t*)(content+sizeof(uint32_t))) = htonl(size);
			if(strncmp(ud->encoding,"gzip",4)==0 || strncmp(ud->encoding,"GZIP",4)==0)
				*((uint32_t*)(content+sizeof(uint32_t)*2)) = htonl(1);
			else
				*((uint32_t*)(content+sizeof(uint32_t)*2)) = htonl(0);
			memcpy(content+sizeof(uint32_t)*3,buff,size);
		}else{
			_LOG_WARNING("malloc failed");
			return 0;
		}
	}else{
		cont_size = sizeof(uint32_t)*3;
		content = (char *)malloc(cont_size);
		if(content){
			*((uint32_t*)content) = htonl(0);
			*((uint32_t*)(content+sizeof(uint32_t))) = htonl(0);
			*((uint32_t*)(content+sizeof(uint32_t)*2)) = htonl(0);
		}else{
			return 0;
		}
 	}
	
	
	if(http_global->Send_Result(handle,PACKET_TYPE_RESULT,&ud->task.peer_addr,
						ud->task.task_rest_head.control_id,content,cont_size)<=0){
		free(content);
	}
	
	return 1;
} 


int Plugin_Period(void *handle,void **userdata){
	return 1;
}


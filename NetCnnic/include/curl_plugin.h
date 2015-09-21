#ifndef	_H_CURL_PLUGIN_H_
#define	_H_CURL_PLUGIN_H_

#include <curl/curl.h>

#include "plugin_inter.h"
#include "thread_inter.h"

#define	MAX_PATH_LEN	2048
#define	CURLE_CUSTERM_TIMEOUT	(-1)
#define	CURLE_CUSTERM_CANNT_START	(-2)

typedef	struct{
	uint32_t			id;
	char				url[MAX_PATH_LEN];
	CURLoption			action;
	char				sip[64];
	struct curl_slist	*headlist;
	char				cookie[MAX_PATH_LEN*2];
	size_t (*write_callback)(char *ptr, size_t size, size_t nmemb, void *requestdata);
	size_t (*read_callback)(char *buffer, size_t size, size_t nitems, void *requestdata);
	size_t (*header_callback)(char *buffer,   size_t size,   size_t nitems,   void *requestdata);
	int	(*setopt_function)(CURL *,void *);
	void				*requestdata;
	int					proxy_type;
	char				proxy_ip[64];
	int					proxy_port;
	char				proxy_username[128];
	char				proxy_passwd[128];
}CURL_REQUEST_INFO_T;


typedef int PLUGIN_SEND_RESULT_FP(void *handle,int ptype,NETADDR_T	*peer,int cid,char *buff, int size);

typedef int PLUGIN_INIT_FP(void *handle,PLUGIN_PARAM_T *param,PLUGIN_SEND_RESULT_FP *send_rst,char *config,void **userdata);
typedef int PLUGIN_RELEASE_FP(void *handle,void **userdata);
typedef int PLUGIN_RECV_TASK_FP(void *handle,PLUGIN_INTER_STRUCT_T *task,void **userdata);
typedef	int PLUGIN_RELEASE_REQUEST_FP(void *handle,CURL_REQUEST_INFO_T *request,void **userdata);
typedef int PLUGIN_GET_REQUEST_FP(void *handle,CURL_REQUEST_INFO_T *request,void **userdata);
typedef int PLUGIN_PRO_RESULT_FP(void *handle,CURL *urlhandle,CURL_REQUEST_INFO_T *request,int result,char *buff,int size,void **userdata);
typedef int PLUGIN_PERIOD_FP(void *handle,void **userdata);

#endif



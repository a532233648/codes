#ifndef _H_CURL_HTTP_CONT_H_
#define	 _H_CURL_HTTP_CONT_H_

#include "curl_plugin.h"

#ifdef __cplusplus
extern "C" {
#endif

PLUGIN_INIT_FP	Plugin_Init;
PLUGIN_RELEASE_FP Plugin_Release;
PLUGIN_RECV_TASK_FP Plugin_Recv_Task;
PLUGIN_RELEASE_REQUEST_FP Plugin_Release_Request;
PLUGIN_GET_REQUEST_FP Plugin_Get_Request;
PLUGIN_PRO_RESULT_FP Plugin_Pro_Result;
PLUGIN_PERIOD_FP Plugin_Period;

#ifdef __cplusplus
}
#endif

#endif

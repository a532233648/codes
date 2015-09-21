#ifndef	_H_THREAD_INTER_H_
#define	_H_THREAD_INTER_H_

#include <stdbool.h>
#include <inttypes.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "CFIFOQueue.h"

#include "log.h"


typedef	struct{
		char thread_type[32];
		char thread_file[32];
		char *config;
}THREAD_PARAM_T;


typedef struct{
	char		plugin_type[32];
	char		plugin_name[32];
	char		plugin_file[32];
	uint16_t	plugin_index;
	char 		*config;
}PLUGIN_PARAM_T;

typedef struct{
	THREAD_PARAM_T	thread_param;
	PLUGIN_PARAM_T	plugin_param;
	CFIFOQueue	*inqueue;
	CFIFOQueue	*outqueue;
	time_t		out_time;
	ul_logstat_t *g_logstats;
}THREAD_PLUGIN_PARAM_T;


#endif


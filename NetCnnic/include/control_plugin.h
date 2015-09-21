
#ifndef	_H_CONTROL_PLUGIN_H_
#define	_H_CONTROL_PLUGIN_H_

#include <inttypes.h>

#include "CFIFOQueue.h"
#include "log.h"

typedef struct{
	uint16_t	id;
	ul_logstat_t *g_logstats;
	char	so_file[64];
	char	*config;
	
	CFIFOQueue	*task_queue;
	CFIFOQueue	*result_queue;

}CONTROL_PLUGIN_INFO_T;

typedef void* PLUGIN_THREAD_FP(void *arg);

#endif

#ifndef	_H_PLUGIN_INTER_H_
#define	_H_PLUGIN_INTER_H_

#include <stdbool.h>
#include <inttypes.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "task_rest.h"

typedef	struct{
	char		ip[48];
	uint16_t	port;
}NETADDR_T;


typedef struct{
	NETADDR_T			peer_addr;
	TASK_REST_HEAD_T	task_rest_head;
	char				*content;
}PLUGIN_INTER_STRUCT_T;


#define	Release_PLUGIN_INTER_STRUCT_T(str){\
	if((str)->content){\
		free((str)->content);\
		(str)->content = NULL;\
	}\
}

#define print_PLUGIN_INTER_STRUCT_T(tis){\
	_LOG_NOTICE("<PLUGIN_INTER_STRUCT_T>\
peer_addr.ip   = %s\n\
peer_addr.port = %u\n\
task_rest_head.control_id  = %u\n\
task_rest_head.packet_type = %u\n\
task_rest_head.plugin_type = %s\n\
task_rest_head.plugin_name = %s\n\
task_rest_head.plugin_index = %u\n\
task_rest_head.cont_size = %u\n\
content = %s\n\
</PLUGIN_INTER_STRUCT_T>",\
	(tis)->peer_addr.ip,\
	(tis)->peer_addr.port,\
	(tis)->task_rest_head.control_id,\
	(tis)->task_rest_head.packet_type,\
	(tis)->task_rest_head.plugin_type,\
	(tis)->task_rest_head.plugin_name,\
	(tis)->task_rest_head.plugin_index,\
	(tis)->task_rest_head.cont_size,\
	(char *)(tis)->content==NULL? "":(char *)(tis)->content);\
}

#endif


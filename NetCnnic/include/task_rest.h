#ifndef	_H_TASK_REST_H_
#define	_H_TASK_REST_H_

#include <stdbool.h>
#include <inttypes.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "log.h"

#define	PACKET_TYPE_INIT	1
#define	PACKET_TYPE_REQUEST	2
#define	PACKET_TYPE_TASK	3
#define	PACKET_TYPE_RESULT	4

#pragma	pack(1)

typedef	struct{
	uint16_t	control_id;
	uint8_t		packet_type;
	char		plugin_type[32];
	char		plugin_name[32];
	uint16_t	plugin_index;
	uint32_t	cont_size;
}TASK_REST_HEAD_T;

#pragma pack(0)

#define	ntoh_TASK_REST_HEAD_T(trh){ \
	(trh)->control_id = ntohs((trh)->control_id);\
	(trh)->plugin_index = ntohs((trh)->plugin_index); \
	(trh)->cont_size = ntohl((trh)->cont_size); \
}


#define	hton_TASK_REST_HEAD_T(trh){ \
	(trh)->control_id = htons((trh)->control_id);\
	(trh)->plugin_index = htons((trh)->plugin_index); \
	(trh)->cont_size = htonl((trh)->cont_size); \
}

#define	print_TASK_REST_HEAD_T(trh){\
	_LOG_NOTICE("<TASK_REST_HEAD_T> \
control_id       = %d\n\
packet_type      = %d\n\
plugin_type      = %s\n\
plugin_name      = %s\n\
plugin_index     = %u\n\
plugin_cont_size = %u\n</TASK_REST_HEAD_T>",\
	(trh)->control_id,\
	(trh)->packet_type,\
	(trh)->plugin_type,\
	(trh)->plugin_name,\
	(trh)->plugin_index,\
	(trh)->cont_size);\
}


#endif


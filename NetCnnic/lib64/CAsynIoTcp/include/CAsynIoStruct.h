
#ifndef	_H_CASYNIOSTRUCT_H_
#define _H_CASYNIOSTRUCT_H_

#define	DATAMAXEVERYTIME	(1024*1024)
#define	NETDATABLOCKMAX	10240
#define	URGENTMAXSIZE		10	

#include <unistd.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <stdint.h>
#include <sys/time.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

typedef struct _UserData{
        int len;//后面实际存储的数据长度
        bool first;//是否是第一次接收(magic_num)
}UserData;

typedef struct _NoConnect{
	int no;///插入m_links[]中的下标
	bool first;///是否时第一次插入，true表示第一次，false表示第二次
}NoConnect;
typedef struct _HostAddr{
	uint32_t ip;
	uint16_t port;
} HostAddr;

typedef struct _LinkAddr{
	HostAddr	saddr;
	HostAddr	daddr;
} LinkAddr;

typedef struct _LinkAddrFd{
	int				fd;
	LinkAddr	addr;
} LinkAddrFd;

typedef struct _DataBlock{
	char 			data[NETDATABLOCKMAX];
	int 			len;//数据的实际存储长度
	int				offset;//每次可能只读/写了datablock中的一部分数据，用offset来标记上次读到的位置
	int				id;//一个buf可能需要多个block来存储，id用于标记多个buf
	struct timeval	tm;
} DataBlock;

typedef	struct _LinkIndex{
	int32_t	no;//在数组中的index
	int32_t	id;//整体分配的唯一id（针对fd）

} LinkIndex;

typedef struct _NetDataBlock{
	LinkAddr	addr;
	LinkIndex	index;
	DataBlock	db;
} NetDataBlock;

typedef	struct _LogData{
	char	type;
	char	log[1024];
} LogData;

int TimeCompare(struct timeval tv1,struct timeval tv2);

/*连接回掉函数, 成功返回大于等于0，失败连接需要关闭返回-1*/
/*link 当前连接*/
/*buff 数据*/
/*len 数据长度*/
/*threadno 所在线程id*/
/*index 连接索引 */
/*addr 连接双方地址*/
/*userdata 用户自定义数据*/
typedef int(*LinkCallbackFunction)(void *link,char *buff,int len,int threadno,LinkIndex index,LinkAddr addr,void **userdata);

#endif


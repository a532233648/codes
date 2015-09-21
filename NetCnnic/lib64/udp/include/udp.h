#ifndef __UDP_H_
#define __UDP_H_

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <pthread.h>

#include <queue>

#include "hll.h"

typedef struct _kwcs_hd
{
	unsigned int flag;
	unsigned short sid;
	unsigned short did;
}kwcs_hd_t;

#define KWCS_HD_LEN sizeof(kwcs_hd_t)

#pragma pack(1)
typedef struct stUDPPseudo_t {
	char ip[20];
	int port;
}stUDPPseudo;
#pragma pack(0)

typedef struct stIPPort_t {
	char ip[20];
	int port;
}stIPPort;

typedef struct stID_t {
	unsigned short did;
	int flag;
}stID;

typedef struct stINFO_t {
	char ip[20];
	int port;
	char message[65535];
	int len;
}stINFO;

typedef struct stUDP_t {
	int queuesize;
	int retrytime;
	int timeout;
	int serverport;
	int listenfd;
	unsigned short sid;
	char ip[20];
	char proxyip[20];
	pthread_t threadid;
	pthread_mutex_t sendlock;
	std::queue<stINFO *> sendqueue;
	HashLinkLimitTable *idtable;
}stUDP;

//ip 为IP地址
//serverport 接收应答的端口
//timeout 等待回复的时间
//retrytime  重试的次数,超过次数就丢弃
//queuesize 缓存的大小
stUDP *udp_open(char *ip, int serverport, int timeout, int retrytime, int queuesize, unsigned short sid);

//ip 为IP地址， port为隔离机端口
//函数只能被调用一次
int udp_set_proxy(stUDP *udp, char *ip);

//ip 为IP地址，id为ip在隔离机的id
//id 为ip在隔离机的id
//flag 为ip：port在隔离机里面的通道编号
int udp_set_recvip(stUDP *udp, char *ip, int port, unsigned short id, int flag);

int udp_sendto(stUDP *udp, char *ip, int port, char *data, int len);

int udp_recvfrom(stUDP *udp, char *buf, int len, struct sockaddr_in *src_addr, socklen_t *addrlen);

int udp_close(stUDP *udp);

#endif

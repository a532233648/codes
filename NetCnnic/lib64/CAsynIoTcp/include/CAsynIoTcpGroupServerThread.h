
#ifndef	_C_ASYNIOTCPGROUPSERVERTHREAD_H_
#define _C_ASYNIOTCPGROUPSERVERTHREAD_H_

#include "CAsynIoTcpGroupThread.h"

#define	FDQUEUESIZE	16

class CAsynIoTcpGroupServerThread:public CAsynIoTcpGroupThread{
private:
	int 				m_epollfd;
	LinkAddrFd	*m_listenfds;
	int 				m_listennum;
	void *m_tcpgroup;
	CFIFOQueue	*m_fdqueue;	
	
	time_t		m_lntime;
	
	int m_outlen;
	LinkCallbackFunction m_callback_connect;
	LinkCallbackFunction m_callback_reconnect;
	LinkCallbackFunction m_callback_input;
	LinkCallbackFunction m_callback_output;
	LinkCallbackFunction m_callback_close;
	LinkCallbackFunction m_callback_msg;
	
	static void *ThreadFunction(void *param);
public:
	CAsynIoTcpGroupServerThread(void *tcpgroup,int id,int linkmax,int outlen,
			LinkCallbackFunction cb_connect,
			LinkCallbackFunction cb_reconnect,
			LinkCallbackFunction cb_input,
			LinkCallbackFunction cb_output,
			LinkCallbackFunction cb_close,
			LinkCallbackFunction cb_msg,ul_logstat_t *log);
			
	CAsynIoTcpGroupServerThread(void *tcpgroup,int id,HostAddr *listenaddr,int listennum,int linkmax,int outlen,
			LinkCallbackFunction cb_connect,
			LinkCallbackFunction cb_reconnect,
			LinkCallbackFunction cb_input,
			LinkCallbackFunction cb_output,
			LinkCallbackFunction cb_close,
			LinkCallbackFunction cb_msg,ul_logstat_t *log);
			
	~CAsynIoTcpGroupServerThread();
	
	int OpenListenFd(HostAddr addr,int n);
	
	void ProcessFdQueue();
	void ProcessListenFd();
	
	int AddFd(int fd,LinkAddr addr);

};


#endif

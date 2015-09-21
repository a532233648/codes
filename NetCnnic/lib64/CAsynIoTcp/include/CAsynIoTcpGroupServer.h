
#ifndef	_C_ASYNIOTCPGROUPSERVER_H_
#define _C_ASYNIOTCPGROUPSERVER_H_

#include "CAsynIoTcpGroupServerThread.h"
#include "log.h"

class CAsynIoTcpGroupServer{//根据每种不同种类的功能来设置多个gs类，每个gst类只有一个线程，相同功能的gst放在同一个gs中
private:
	HostAddr	*m_listenaddr;
	int				m_listennum;
	int				m_fdthread;
	int				m_threadnum;
	bool			m_success;
	ul_logstat_t 	*m_log;
	
	CAsynIoTcpGroupServerThread	**m_threads;
	
	int m_outlen;
	LinkCallbackFunction m_callback_connect;
	LinkCallbackFunction m_callback_reconnect;
	LinkCallbackFunction m_callback_input;
	LinkCallbackFunction m_callback_output;
	LinkCallbackFunction m_callback_close;
	LinkCallbackFunction m_callback_msg;
	
public:
	
	CAsynIoTcpGroupServer(HostAddr	*listaddr,int listennum,int threadnum,int linkmax,int outlen,
			LinkCallbackFunction cb_connect,
			LinkCallbackFunction cb_reconnect,
			LinkCallbackFunction cb_input,
			LinkCallbackFunction cb_output,
			LinkCallbackFunction cb_close,
			LinkCallbackFunction cb_msg,ul_logstat_t *log);
			
	~CAsynIoTcpGroupServer();
	
	int GetLinkState(int threadno,LinkAddr addr,LinkIndex index);
	time_t GetLinkTime(int threadno,LinkAddr addr,LinkIndex index);
	int DeleteLink(int threadno,LinkAddr addr,LinkIndex index);
	int CloseLink(int threadno,LinkAddr addr,LinkIndex index);
	
	int AddFd(int fd, LinkAddr addr);
	
	bool IsSuccess(){return m_success;};
	
	int Run();
};

#endif

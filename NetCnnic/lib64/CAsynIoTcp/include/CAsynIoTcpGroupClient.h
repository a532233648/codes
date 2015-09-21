
#ifndef	_C_ASYNIOTCPGROUPCLIENT_H_
#define _C_ASYNIOTCPGROUPCLIENT_H_

#include "CAsynIoTcpGroupThread.h"
#include "log.h"

class CAsynIoTcpGroupClient{
private:
	int				m_fdthread;
	int				m_threadnum;
	bool			m_success;
	
	ul_logstat_t 	*m_log;
	
	CAsynIoTcpGroupThread	**m_threads;
	
public:
	
	CAsynIoTcpGroupClient(int threadnum,int linkmax,ul_logstat_t *log);
	~CAsynIoTcpGroupClient();
	
	int GetLinkState(int threadno,LinkAddr addr,LinkIndex index);
	time_t GetLinkTime(int threadno,LinkAddr addr,LinkIndex index);
	int CloseLink(int threadno,LinkAddr addr,LinkIndex index);
	int DeleteLink(int threadno,LinkAddr addr,LinkIndex index);
	
	/*增加连接，成功返回1，失败返回0，连接异常并关闭返回-1*/
	/*state 连接状态 LINK_STATE_INVALID、LINK_STATE_NOCONNECT、LINK_STATE_CONNECTED*/
	/*type  连接类型 LINK_TYPE_NORMAL、LINK_TYPE_RECONNECT*/
	/*fd 连接描述符*/
	/*addr 连接双方地址*/
	/*threadno 带回连接加入的线程号*/
	/*index 返回连接索引*/
	/*userdata 用户自定义数据*/
	/*outlen 输出缓冲区大小*/
	/*cb_connect 连接成功时回掉函数*/
	/*cb_reconnect 重连成功时回掉函数*/
	/*cb_input 已接收到数据处理函数*/
	/*cb_output 接收待发送数据函数*/
	/*cb_close 连接关闭后回掉函数*/
	/*cb_msg 连接处理信息函数*/ 
	int AddLink(char state,char type,int fd,LinkAddr addr,int &threadno,LinkIndex &index,
			void *userdata,int outlen,
			LinkCallbackFunction cb_connect,
			LinkCallbackFunction cb_reconnect,
			LinkCallbackFunction cb_input,
			LinkCallbackFunction cb_output,
			LinkCallbackFunction cb_close,
			LinkCallbackFunction cb_msg);
		
	bool IsSuccess(){return m_success;};
	
	int Run();
};

#endif


#ifndef	_C_ASYNIOTCPGROUPTHREAD_H_
#define _C_ASYNIOTCPGROUPTHREAD_H_

#include "CFIFOQueue.h"
#include "CAsynIoTcpLink.h"
#include "log.h"

#define	FDQUEUESIZE	16

class CAsynIoTcpGroupThread{
private:
	int m_id;//标记第几个线程
	
	pthread_t	m_pthread;
	
	int	m_epollfd;
	
	CAsynIoTcpLink **m_links;//m_links = (CAsynIoTcpLink **)malloc(sizeof(CAsynIoTcpLink *)*linkmax);
	
	int m_linksize;//存放fd的数组大小,m_links[]，存放fd数的上限
	int m_linknum;//每成功添加一个fd就加1,实际上存放的fd数
	int m_maxlinkid;//统一分配fd的id号,通过m_maxlinkid++
	bool m_success;
///定义一个vector类来保存需要重连的Link类
	time_t	m_ltime;
	struct timeval m_lstime;
	
	pthread_mutex_t 	m_mutex;
	ul_logstat_t 	*m_log;

	void* (*m_ThreadFunction)(void *param);
	static void *ThreadFunction(void *param);
		
public:
	
	/*初始化函数*/
	/*id 线程的唯一id*/
	/*linkmax 最大处理的连接数*/
	CAsynIoTcpGroupThread(int id,int linkmax,ul_logstat_t *log);
	
	~CAsynIoTcpGroupThread();
	
	int SetNonBlocking(int fd) ;
		
//	int PrintMsg(int type,const char *format, ...);
		
	ul_logstat_t* GetLog(){return m_log;};
	
	void	SetId(int id){m_id = id;};
	int		GetId(){return m_id;};
	
	void ProcessLinksEvent();
	
	void SetSuccess(bool suc){m_success = suc;};
	bool IsSuccess(){return m_success;};
	
	void SetThreadFunction(void* (*func)(void *param)){m_ThreadFunction = func;};
	
	int GetLinkNum();
		
	int GetLinkState(LinkAddr addr,LinkIndex index);
	
	time_t GetLinkTime(LinkAddr addr,LinkIndex index);
	
	/*增加连接，成功返回1，失败返回0，连接异常并关闭返回-1*/
	/*state 连接状态 LINK_STATE_INVALID、LINK_STATE_NOCONNECT、LINK_STATE_CONNECTED*/
	/*type  连接类型 LINK_TYPE_NORMAL、LINK_TYPE_RECONNECT*/
	/*fd 连接描述符*/
	/*addr 连接双方地址*/
	/*index 返回连接索引*/
	/*userdata 用户自定义数据*/
	/*outlen 输出缓冲区大小*/
	/*cb_connect 连接成功时回掉函数*/
	/*cb_reconnect 重连成功时回掉函数*/
	/*cb_input 已接收到数据处理函数*/
	/*cb_output 接收待发送数据函数*/
	/*cb_close 连接关闭后回掉函数*/
	/*cb_msg 连接处理信息函数*/ 
	int AddLink(char state,char type,int fd,LinkAddr addr,LinkIndex &index,void *userdata,int outlen,
			LinkCallbackFunction cb_connect,
			LinkCallbackFunction cb_reconnect,
			LinkCallbackFunction cb_input,
			LinkCallbackFunction cb_output,
			LinkCallbackFunction cb_close,
			LinkCallbackFunction cb_msg);
	
	/*关闭连接，成功返回1，失败返回0*/
	int CloseLink(LinkAddr addr,LinkIndex index);
	
	/*删除连接，成功返回1，失败返回0*/
	int DeleteLink(LinkAddr addr,LinkIndex index);
		
	int Run();
};

#endif

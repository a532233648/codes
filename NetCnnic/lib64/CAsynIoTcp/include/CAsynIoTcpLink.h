/*仅允许Read()和Write由另一线程运行，其他函数和属性不允许多线程同时访问*/



#ifndef	_C_ASYNIOTCPLINK_H_
#define _C_ASYNIOTCPLINK_H_

#include "CFIFOQueue.h"
#include "CAsynIoStruct.h"

#define	LINK_STATE_INVALID	0
#define	LINK_STATE_NOCONNECT	1
#define	LINK_STATE_CONNECTED	2
#define	LINK_STATE_CONNECTING	3

#define	LINK_TYPE_NORMAL	0
#define	LINK_TYPE_RECONNECT	1

#define	CALLBACK_CONNECT	1
#define	CALLBACK_RECONNECT	2
#define	CALLBACK_INPUT		3
#define	CALLBACK_OUTPUT		4
#define	CALLBACK_CLOSE		5
#define	CALLBACK_MSG		6

#define	MSG_TYPE_NONE		0
#define	MSG_TYPE_NOTICE			1
#define	MSG_TYPE_ERROR			2

class CAsynIoTcpLink{
private:
	uint32_t	m_id;	//每个fd连接的唯一编号,自己定义的
	uint32_t	m_no;	//连接在CAsynIoTcpGroupThread中的序号
	char	m_state;	//连接状态
	char	m_type;		//连接类型
	int	m_fd;//init to 1，连接的套接字
	LinkAddr	m_addr;	//连接双方地址
	time_t	m_time;	//连接建立时间
	
	void *m_thread;
	void *m_userdata;//有何用？	//用户自定义数据
	char *m_outbuffer;
	int		m_outbufpos;
	int		m_outbuflen;
	int		m_outbufsize;
	
	
	bool m_success;			//是否初始化成功标记

	int	m_first;
	
	LinkCallbackFunction m_callback_connect;
	LinkCallbackFunction m_callback_reconnect;
	LinkCallbackFunction m_callback_input;
	LinkCallbackFunction m_callback_output;
	LinkCallbackFunction m_callback_close;
	LinkCallbackFunction m_callback_msg;
	

public:	
	
	/*初始化函数*/
	/*id 连接的唯一id*/
	/*no 连接在线程连接数组中的位置*/
	/*state 连接状态 LINK_STATE_INVALID、LINK_STATE_NOCONNECT、LINK_STATE_CONNECTED*/
	/*type  连接类型 LINK_TYPE_NORMAL、LINK_TYPE_RECONNECT*/
	/*fd 连接描述符*/
	/*addr 连接双方地址*/
	/*thd 线程类指针*/
	/*userdata 用户自定义数据*/
	/*outlen 输出缓冲区大小*/
	/*cb_connect 连接成功时回掉函数*/
	/*cb_reconnect 重连成功时回掉函数*/
	/*cb_input 已接收到数据处理函数*/
	/*cb_output 接收待发送数据函数*/
	/*cb_close 连接关闭后回掉函数*/
	/*cb_msg 连接处理信息函数*/
	CAsynIoTcpLink(uint32_t id,uint32_t no,char state,char type,int fd,LinkAddr addr,void *thd,void *userdata,int outlen,
			LinkCallbackFunction cb_connect,
			LinkCallbackFunction cb_reconnect,
			LinkCallbackFunction cb_input,
			LinkCallbackFunction cb_output,
			LinkCallbackFunction cb_close,
			LinkCallbackFunction cb_msg);
	
	/*初始化函数*/
	/*id 连接的唯一id*/
	/*no 连接在线程连接数组中的位置*/
	/*state 连接状态 LINK_STATE_INVALID、LINK_STATE_NOCONNECT、LINK_STATE_CONNECTED*/
	/*type  连接类型 LINK_TYPE_NORMAL、LINK_TYPE_RECONNECT*/
	/*fd 连接描述符*/
	/*addr 连接双方地址*/
	/*userdata 用户自定义数据*/
	/*outlen 输出缓冲区大小*/
	/*cb_connect 连接成功时回掉函数*/
	/*cb_reconnect 重连成功时回掉函数*/
	/*cb_input 已接收到数据处理函数*/
	/*cb_output 接收待发送数据函数*/
	/*cb_close 连接关闭后回掉函数*/
	/*cb_msg 连接处理信息函数*/ 
	void Reset(uint32_t id,uint32_t no,char state,char type,int fd,LinkAddr addr,void *userdata,int outlen,
			LinkCallbackFunction cb_connect,
			LinkCallbackFunction cb_reconnect,
			LinkCallbackFunction cb_input,
			LinkCallbackFunction cb_output,
			LinkCallbackFunction cb_close,
			LinkCallbackFunction cb_msg);
			
	~CAsynIoTcpLink();
	
	int Callback(int func,const char *buf,int len);
	
	int CallbackMsg(int type,const char *format, ...);
	
	
	/*建立连接，采用非阻塞方式成功返回1，连接中返回0，异常返回-1*/
	int Connect();
	
	/*设置连接非阻塞，成功返回1，失败返回-1*/
	int SetNonBlocking(int fd);
	
	/*关闭连接，成功返回1，失败返回0*/
	int Close();
	
	/*连接处理读事件，正常返回大于0，异常返回-1，连接关闭返回0*/
	int Read();
	
	/*连接处理写事件，正常返回大于0，异常返回-1，连接关闭返回0*/
	int Write();
	
	/*设置连接状态*/
	void SetState(char state){m_state = state;};
	
	/*返回连接状态*/
	int  GetState(){return m_state;};
	void	 SetState(int state){m_state = state;};
	
	int  GetType(){return m_type;};
	
	int GetFd(){return m_fd;};
	
	int GetId(){return m_id;};
	
	LinkAddr *GetAddr(){return &m_addr;};
	
	/*获取连接建立时间*/
	time_t  GetTime(){return m_time;};
	
	void SetUserdata(void *ud){m_userdata = ud;};
	void  **GetUserdata(){return &m_userdata;};
	
	bool IsSuccess(){return m_success;};
	
	int	ProcessConnecting();
	
};


#endif

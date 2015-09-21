
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
	
	/*�������ӣ��ɹ�����1��ʧ�ܷ���0�������쳣���رշ���-1*/
	/*state ����״̬ LINK_STATE_INVALID��LINK_STATE_NOCONNECT��LINK_STATE_CONNECTED*/
	/*type  �������� LINK_TYPE_NORMAL��LINK_TYPE_RECONNECT*/
	/*fd ����������*/
	/*addr ����˫����ַ*/
	/*threadno �������Ӽ�����̺߳�*/
	/*index ������������*/
	/*userdata �û��Զ�������*/
	/*outlen �����������С*/
	/*cb_connect ���ӳɹ�ʱ�ص�����*/
	/*cb_reconnect �����ɹ�ʱ�ص�����*/
	/*cb_input �ѽ��յ����ݴ�����*/
	/*cb_output ���մ��������ݺ���*/
	/*cb_close ���ӹرպ�ص�����*/
	/*cb_msg ���Ӵ�����Ϣ����*/ 
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

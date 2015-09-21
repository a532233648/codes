
#ifndef _C_BYTEBUFFER_H_
#define	_C_BYTEBUFFER_H_


class CByteBuffer{
private:
	char *m_buff;
	int	m_head;
	int m_tail;
	int m_size;
public:
	CByteBuffer(int size);
	~CByteBuffer();
	
	int GetCount(){return (m_head+m_size-m_tail)%m_size;};
	int GetSize(){return m_size;};
	
	int Write(char *buf,int len);
	int Read(char *buf,int len);
};

#endif

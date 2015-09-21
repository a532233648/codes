#ifndef	_H_FIFOQUEUE_H_	
#define	_H_FIFOQUEUE_H_

class	CFIFOQueue{

public:
	 CFIFOQueue(int qsize,int isize);
	~CFIFOQueue();
	
	int Push(void *item);
	int Pop(void *item);
	void *GetAt(int index);
	bool IsSuccess();
	int GetAvilable(){return (m_tail+m_size-m_head-1)%m_size;};
	int GetCount(){return (m_head+m_size-m_tail)%m_size;};
	int GetSize(){return m_size;};
	void Clear(){m_tail = m_head = 0;};
	
private:
	void **m_queue;
	int	m_head;
	int	m_tail;
	int	m_size;
	int	m_itemsize;
	
};



#endif


#ifndef __HASHTABLE_H__
#define __HASHTABLE_H__

/*****************************************************************************/
typedef struct hash_node
{
    void *v;
    struct hash_node *next;
}hash_node_t;

class CHashTable{
private:
    int m_tablelength;
    hash_node_t **m_table;
    int m_nodecount;
    int m_loadlimit;
    int m_primeindex;
    int m_nexted;
    
    void (*m_free_value)(void *vl);
    unsigned int (*m_hashfunc) (void *k);
    int (*m_compfunc) (void *k1, void *k2);
	
	 int m_prime_table_length;
	float m_max_load_factor;
	int	m_issuccess;
	
	void Init();   
	int Expand();
	
	unsigned int Hash(void *v);
public:

	CHashTable(unsigned int minsize,int loadfactor,
				 void (*free_value)(void *),
                 unsigned int (*hashfunc) (void*),
                 int (*compfunc) (void*,void*));
    
    ~CHashTable();

	int IsSuccess(){return m_issuccess;};              

	int Insert(void *v);

	void *Search(void *v);

	int  Remove(void *v);

	unsigned int Count();

	void Free();

	int Update(void *v);
	
	void *GetNext(int *index,void **node);

	void Trace();

};



#endif /* __HASHTABLE_H__ */



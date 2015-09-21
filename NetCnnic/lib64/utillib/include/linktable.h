
#ifndef _LINKTABLE_H_
#define	_LINKTABLE_H_


typedef struct _linktable_node_t{
	void	*data;
	struct	_linktable_node_t *prev;
	struct	_linktable_node_t *next;
}linktable_node_t;


typedef struct _linktable_t{
	linktable_node_t	head;
	int length;
}linktable_t;


int linktable_init(linktable_t *lt);
int linktable_add(linktable_t *lt,linktable_node_t *node);
linktable_node_t *linktable_get(linktable_t *lt);
int	linktable_del(linktable_t *lt,linktable_node_t *node);
linktable_node_t *linktable_tail(linktable_t *lt);
linktable_node_t *linktable_head(linktable_t *lt);
void linktable_free(linktable_t *lt);

#endif


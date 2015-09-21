#ifndef __HASH_LIMIT_LINK_H_
#define __HASH_LIMIT_LINK_H_

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#ifdef offsetof
#undef offsetof
#endif

#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)

#define container_of(ptr, type, member) ({          \
		const typeof(((type *)0)->member)*__mptr = (ptr);    \
		(type *)((char *)__mptr - offsetof(type, member)); })

struct list_head {
	struct list_head *next, *prev;
};

#define LIST_HEAD_INIT(name) { &(name), &(name)}

#define list_entry(ptr, type, member) \
	    container_of(ptr, type, member)

#define list_for_each_entry(pos, head, member)              \
	for (pos = list_entry((head)->next, typeof(*pos), member);  \
			&pos->member != (head);    \
			pos = list_entry(pos->member.next, typeof(*pos), member))

void list_add_tail(struct list_head *newlink, struct list_head *head);

void list_move_tail(struct list_head *list);

void list_update_tail(struct list_head *list, struct list_head *head);

void list_del(struct list_head *list);
#endif

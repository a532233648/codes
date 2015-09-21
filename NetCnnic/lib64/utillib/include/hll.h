#ifndef __HASH_LINK_LIMIT_H_
#define __HASH_LINK_LIMIT_H_

#include <pthread.h>

#include "hlldef.h"
#include "hllink.h"

#ifdef __cplusplus
extern "C" {
#endif
	//attention: don't forget mutithread read write value
	enum {
		HLL_OK = 0,
		HLL_INSERT_KEY_EXIST = -1,
		HLL_INSERT_ENOUGHT,
		HLL_INSERT_MALLOC_FAILED,
		HLL_INSERT_TIMEOUT_FAILED,
		HLL_REPLACE_ENOUGHT,
		HLL_REMOVE_KEY_NOEXIST,
		HLL_PARAM_ERROR,
	};

	typedef struct _HashLinkLimitItem {
		void *key;
		void *value;
		uint64_t time;
		struct _HashLinkLimitItem *next;
		struct list_head list;
	}HashLinkLimitItem;

	typedef struct _HashLinkLimitTable {
		pthread_rwlock_t lock;
		pthread_mutex_t readerlock;
		int32_t item_time;
		int32_t item_count;
		int32_t item_max;
		int32_t table_size;
		HashLinkLimitItem **table;
		struct list_head *head;
		HllCmp hl_cmp;	
		HllFetch hl_fetch;
		HllHash hl_hash;
		HllDestroy item_destroy;
		HllTimeout item_timeout;
		void *user;
	}HashLinkLimitTable;

	HashLinkLimitTable *hll_create(int limitcount, int limittime, HllCmp hl_cmp, HllHash hl_hash, HllDestroy item_destroy, void *user);

	int hll_set_fetch_callback(HashLinkLimitTable *table, HllFetch fetch);

	int hll_set_hash_callback(HashLinkLimitTable *table, HllHash hash);

	int hll_set_destroy_callback(HashLinkLimitTable *table, HllDestroy destroy, void *user);

	int hll_set_timeout_callback(HashLinkLimitTable *table, HllTimeout timeout);

	int hll_insert_item(HashLinkLimitTable *table, void *key, void *value);

	int hll_replace_item(HashLinkLimitTable *table, void *key, void *value);

	int hll_remove_item(HashLinkLimitTable *table, void *key);

	int hll_fetch_timeout(HashLinkLimitTable *table);

	void *hll_fetch_item(HashLinkLimitTable *table, void *key);

	int hll_destroy(HashLinkLimitTable *table);

	int hll_reset_item(HashLinkLimitTable *table);
#ifdef __cplusplus
}
#endif

#endif

#ifndef __HASHLINKLIMIT_DEF_H_
#define __HASHLINKLIMIT_DEF_H_

#include <stdint.h>
#include <stddef.h>
#include <sys/types.h>

#include "hllink.h"

#ifdef __cplusplus
extern "C" {
#endif
	typedef uint32_t (*HllCmp) (void *arg1, void *arg2);
	typedef	uint32_t (*HllHash) (void *arg1);
	typedef uint32_t (*HllFetch) (void *arg1);

	typedef int32_t (*HllTimeout) (void *key, void *value, void *user);
	typedef int32_t (*HllDestroy) (void *key, void *value, void *user);

#ifdef __cplusplus
}
#endif

#endif

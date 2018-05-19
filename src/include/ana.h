#ifndef COMO_OBJECT_BASE
#define COMO_OBJECT_BASE

/** 
 * This file is the core header for the Como Object API. 
 * Only include this file to get all symbol defns. If you try to include
 * any of them directly, an error will occurr
 */
#define COMO_BASE_INCLUDED 1

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>

#include "utils.h"
#include "system.h"
#include "anaobject.h"
#include "anabool.h"
#include "container.h"
#include "class.h"
#include "array.h"
#include "module.h"
#include "anastring.h"
#include "map.h"
#include "long.h"
#include "double.h"
#include "code.h"
#include "frame.h"
#include "function.h"
#include "arena.h"

#define LIKELY(x)       __builtin_expect(!!(x), 1)
#define UNLIKELY(x)     __builtin_expect(!!(x), 0)


void*(*ana_allocator)(size_t sz);

/* TODO
  also need to figure out to reclaim an object's slot once it gets collected

* basically this can be thought of as a sparse array
*/

#define ANA_RUNTIME_OBJECT_CACHE_SIZE 1024

#define ana_type_check_both(left, right, type) \
  (ana_type_is((left), (type)) && ana_type_is((right), (type)))

/* Runtime cache slots */
typedef struct ana__runtime_object_cache {
  ana_arena *arena;
  ana_array *prefetched__array[ANA_RUNTIME_OBJECT_CACHE_SIZE];
  ana_long  *prefetched__long[ANA_RUNTIME_OBJECT_CACHE_SIZE];
  ana_bool  *prefetched__bool[ANA_RUNTIME_OBJECT_CACHE_SIZE];
  int prefetched__array_slot;
  int prefetched__long_slot;
  int prefetched__bool_slot;
} ana__runtime_object_cache;

typedef struct ana__runtime {
  int object_cache_enabled;
  ana_arena *arena;
  ana__runtime_object_cache object_cache;
} ana_runtime;

extern ana_runtime *ana__base_runtime;

#define ANA_GET_RUNTIME() (ana__base_runtime)

#define ANA_RUNTIME_SET_CACHE_ENABLED() \
  ANA_GET_RUNTIME()->object_cache_enabled = 1

COMO_OBJECT_API void ana__runtime_object_cache_init(void);
COMO_OBJECT_API void ana__runtime_object_cache_finalize(void);

COMO_OBJECT_API char *ana_get_fn_name(ana_frame * frame);

#endif

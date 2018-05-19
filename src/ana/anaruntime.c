#include <stdlib.h>
#include <string.h>
#include <ana.h>


#include "utils.h"

ana_runtime *ana__base_runtime;
static ana_runtime _base_runtime;


COMO_OBJECT_API char *ana_get_fn_name(ana_frame * frame)
{
  const char *base = "";
  const char *sep = "";
  const char *fnname = ana_cstring(frame->name);

  if(frame->self)
  {
    base = ana_cstring(ana_get_class(frame->self)->name);
    sep = ".";
  }

  return ana_build_str("%s%s%s", base, sep, fnname);
}

void ana__runtime_object_cache_init(void)
{
  int i;
  _base_runtime.object_cache_enabled = 0;
  _base_runtime.arena = ana_arena_new_sized(COMO_ARENA_BLOCK_SIZE * 2 * 2);
  ana__base_runtime = &_base_runtime;
  return;

  _base_runtime.object_cache.prefetched__array_slot = 0;
  _base_runtime.object_cache.prefetched__long_slot = 0;
  _base_runtime.object_cache.prefetched__long_slot = 0;

  for(i = 0; i < ANA_RUNTIME_OBJECT_CACHE_SIZE; i++)
  {
    /* init array */
    _base_runtime.object_cache.prefetched__array[i] 
      = malloc(sizeof(ana_array));
    _base_runtime.object_cache.prefetched__array[i]->items 
      = malloc(sizeof(ana_object *) * 8);
    
    /* init long */
    _base_runtime.object_cache.prefetched__long[i] 
      = malloc(sizeof(ana_long));

    /* init bool */
    _base_runtime.object_cache.prefetched__bool[i] 
      = malloc(sizeof(ana_bool));
  }
}

void ana__runtime_object_cache_finalize(void)
{
  ana_arena_free(_base_runtime.arena);
}
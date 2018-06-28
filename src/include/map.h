#ifndef COMO_MAP_OBJECT_H
#define COMO_MAP_OBJECT_H

# if !(defined(COMO_BASE_INCLUDED))
#   error "Please do not include map.h directly"
#endif

typedef struct _ana_map_bucket 
{
  ana_object             *key;
  ana_object             *value;
  struct _ana_map_bucket *next;
} ana_map_bucket;

typedef struct _ana_map 
{
  ana_object       base;
  ana_map_bucket **buckets;
  ana_usize_t      size;
  ana_usize_t      capacity;
} ana_map;


typedef struct _ana_map_iterator {
  ana_object base;
  ana_map *container;
  ana_usize_t     current_bucket_position;
  ana_map_bucket *current_bucket;
  ana_object *current_key;
  ana_object *current_value;
  int        has_next;
} ana_map_iterator;

COMO_OBJECT_API ana_object *ana_map_new(ana_size_t size);
COMO_OBJECT_API ana_object *ana_map_put(ana_object *obj, ana_object *key, 
  ana_object *value);
COMO_OBJECT_API ana_object *ana_map_get(ana_object *obj, ana_object *key);
COMO_OBJECT_API ana_object *ana_map_delete(ana_object *obj, 
  ana_object *key);
COMO_OBJECT_API void ana_map_print(ana_object *map);
COMO_OBJECT_API ana_object *ana_get_local_ex(ana_usize_t hashed, ana_size_t len, 
  char *key, ana_map *map);

COMO_OBJECT_API ana_object *ana_put_local_ex(
  ana_string *key, 
  ana_object *value, 
  ana_map *map
);

#define ana_put_local(map, key, value) \
  ana_put_local_ex((ana_string *)key, value, ((ana_map *)map))


#define ana_get_map(o) (((ana_map *)(o)))
#define ana_map_size(o) (((ana_map *)(o))->size)
#define ana_map_capacity(o) (((ana_map *)(o))->capacity)
#define ana_get_local(map, key) \
  ana_get_local_ex(key->hash, key->len, key->value, ((ana_map *)map))

#define ana_map_foreach(_map, keyname, valuename) { \
  ana_object *keyname; \
  ana_object *valuename; \
  ana_usize_t _curelem = 0; \
  do { \
    ana_usize_t _i; \
    ana_map *map = ana_get_map(_map); \
    for(_i = 0; _i < map->capacity; _i++) { \
      ana_map_bucket *_bucket = map->buckets[_i]; \
      if(_bucket) { \
        while(_bucket) { \
          _curelem++; \
          keyname = _bucket->key; \
          valuename = _bucket->value; \

#define ana_map_foreach_end() \
          _bucket = _bucket->next; \
        } \
      } \
    } \
  } while(0); \
} \

#define ana_map_foreach_kv_apply(map, function) \
  ana_map_foreach(map, key, value) { \
    function(key); \
    function(value); \
  } ana_map_foreach_end() 


#define ANA_KEY_NOT_FOUND ((void*)-1)

extern ana_type ana_map_type;
extern ana_type ana_map_iterator_type;


#endif

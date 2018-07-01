#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <stdio.h>
#include <float.h>

#include <ana.h>

#define key_type_valid(key) \
  !((!ana_type_is(key, ana_string_type) \
    && !ana_type_is(key, ana_long_type))) 

#define maybe_resize(o) do { \
  ana_map *map = (ana_map *)(o); \
  if(map->size >= map->capacity) \
    do_resize(map); \
} while(0)

static inline ana_map_bucket *get_bucket(ana_map *map, ana_object *key, 
  ana_usize_t *idx)
{
  ana_usize_t hashed = 
    ana_type_is(key, ana_string_type) ? ((ana_string *)key)->hash 
    : (ana_usize_t)(((ana_long *)key)->value);

  ana_usize_t index  = hashed % map->capacity;
  ana_map_bucket *bucket = map->buckets[index];
  ana_map_bucket *retval = NULL;

  /* Check to see if this key already exists */
  while(bucket != NULL) 
  {
    ana_map_bucket *next = bucket->next;
    ana_object *thiskey  = bucket->key;

    if(thiskey->type->obj_equals(thiskey, key)) 
    {
      retval = bucket;
      break;
    }

    bucket = next;
  }

  if(idx)
    *idx = index;

  return retval;
}

COMO_OBJECT_API ana_object *ana_get_local_ex(ana_usize_t hashed, ana_size_t len, 
  char *key, ana_map *map) 
{
  ana_map_bucket *bucket = map->buckets[hashed % map->capacity];
  ana_string *str;

  while(bucket != NULL) 
  {
    str = (ana_string *)bucket->key;

    if(len == str->len && !memcmp(key, str->value, len))
      return bucket->value;

    bucket = bucket->next;
  }

  return NULL;
}

COMO_OBJECT_API ana_object *ana_put_local_ex(
  ana_string *key, ana_object *value, ana_map *map)
{
  ana_size_t idx = key->hash % map->capacity;
  ana_map_bucket *bucket = map->buckets[idx];
  ana_string *str;
  ana_size_t len = key->len;

  while(bucket != NULL) 
  {
    str = (ana_string *)bucket->key;

    if(len == str->len && !memcmp(key->value, str->value, len)) 
    {
      bucket->value = value;
      return value;
    }

    bucket = bucket->next;
  }

  bucket = malloc(sizeof(*bucket));

  bucket->key = (ana_object *)key;
  bucket->value = value;
  bucket->next = map->buckets[idx];

  map->buckets[idx] = bucket;
  map->size++;

  return value;
}

COMO_OBJECT_API ana_object *ana_map_new(ana_size_t size)
{
  if(size == 0)
    size = 4;
  
  ana_map *map = malloc(sizeof(*map));
  ana_size_t i;
  map->base.type = &ana_map_type;
  map->base.next = NULL;
  map->base.flags = 0;
  map->base.refcount = 0;
  map->base.is_tracked = 0;

  map->capacity = size;
  map->size = 0;

  map->buckets = malloc(sizeof(ana_map_bucket*) * size);

  for(i = 0; i < size; i++)
    map->buckets[i] = NULL;

  return (ana_object *)map;
}

static inline void do_resize(ana_map *map)
{
  ana_usize_t newcap = map->capacity * 2;
  ana_map_bucket **newbuckets = malloc(sizeof(ana_map_bucket *) * newcap);
  ana_usize_t i;

  for(i = 0; i < newcap; i++)
    newbuckets[i] = NULL;
  
  for(i = 0; i < map->capacity; i++) 
  {
    ana_map_bucket *b = map->buckets[i];
    
    while(b)
    {
      ana_map_bucket *next = b->next;

      ana_usize_t newidx = b->key->type->obj_hash(b->key) % newcap;
      
      ana_map_bucket *bucket = malloc(sizeof(*bucket));
      bucket->key = b->key;
      bucket->value = b->value;
      bucket->next = newbuckets[newidx];

      newbuckets[newidx] = bucket;

      free(b);
      b = next;
    }
  }

  free(map->buckets);
  map->buckets  = newbuckets;
  map->capacity = newcap;
}

COMO_OBJECT_API ana_object *ana_map_put(ana_object *obj, 
  ana_object *key, ana_object *value)
{
  assert(obj);
  assert(key);
  assert(value);
  /* Only ints, and strings are allowed as keys */
  if(!ana_type_is(key, ana_string_type) && 
    !ana_type_is(key, ana_long_type)) 
  {
    fprintf(stdout, "ana_map_put: %s objects not allowed as keys\n",
      ana_type_name(key));
    return NULL;
  }

  maybe_resize(obj);

  ana_usize_t idx;
  ana_map *map = (ana_map *)obj;
  ana_map_bucket *bucket = get_bucket(map, key, &idx);

  if(bucket) 
  {
    bucket->value = value;
    goto done;
  }

  bucket = malloc(sizeof(*bucket));

  bucket->key = key;
  bucket->value = value;
  bucket->next = map->buckets[idx];

  map->buckets[idx] = bucket;
  map->size++;

done:
  return value;
}


COMO_OBJECT_API ana_object *ana_map_get(ana_object *obj, ana_object *key)
{
  ana_map *map = (ana_map *)obj;

  if(!key_type_valid(key)) 
  {
    return NULL;
  }

  ana_map_bucket *bucket = get_bucket(map, key, NULL);

  if(bucket)
    return bucket->value;

  return NULL;
}

COMO_OBJECT_API ana_object *ana_map_delete(ana_object *obj, ana_object *key)
{
  ana_map *map = (ana_map *)obj;

  if(!key_type_valid(key))
    return NULL;

  ana_usize_t hashed = key->type->obj_hash(key);
  ana_size_t index  = hashed % map->capacity;
  ana_map_bucket *bucket = map->buckets[index];
  ana_map_bucket *prev = NULL;
  ana_map_bucket *first = map->buckets[index];;

  while(bucket != NULL) 
  {
    ana_map_bucket *next = bucket->next;
    ana_object *thiskey  = bucket->key;

    if(thiskey->type->obj_equals(thiskey, key)) 
    {
      if(prev) 
      {
        prev->next = bucket->next;
        free(bucket);
      }
      else 
      {
        map->buckets[index] = NULL;
        free(first);
      }

      map->size--;
      return key;
    }

    bucket = next;
    prev = bucket;
  }

  return NULL;
}

static void map_dtor(ana_object *ob)
{
  ana_map *map = (ana_map *)ob;
  ana_usize_t i;

  for(i = 0; i < map->capacity; i++) 
  {
    ana_map_bucket *bucket = map->buckets[i];

    if(bucket) 
    {
      while(bucket != NULL) 
      {
        ana_map_bucket *next = bucket->next;
        /* we don't free keys, or values, */
        free(bucket);
        
        bucket = next;
      }
    }
  }

  free(map->buckets);
  free(map);
}

COMO_OBJECT_API void ana_map_print(ana_object *obj)
{
  printf("{");

  ana_map_foreach(obj, key, mapvalue) 
  {
    ana_tostring_fast(key,  
    {
      printf("'%s': ", value);
    });
    
    ana_tostring_fast(mapvalue,  
    { 
      if(ana_type_is(mapvalue, ana_string_type)) 
      {
        printf("'%s'", value);
      }
      else 
      {
        printf("%s", value);
      }

      if(_curelem != map->size)
        printf(", ");
    });  
  } ana_map_foreach_end();
  
  fputc('}', stdout);
}

static void map_print(ana_object *obj)
{
  ana_map_print(obj);
}

static ana_object *map_str(ana_object *obj)
{
  ana_object *retval;

  ANA_AUTO_RELEASE(ana_build_str("<map at %p>", (void *)obj), {
    retval = ana_stringfromstring(value);
  });

  return retval;
}

static ana_object *map_get(ana_object *obj, ana_object *index)
{  
  return ana_map_get(obj, index);
}

static ana_object *map_set(ana_object *obj, ana_object *index, 
  ana_object *val)
{
  return ana_map_put(obj, index, val);
}


static ana_object *map_equals(ana_object *a, ana_object *b)
{
  return ana_boolfromint(a == b);
}

static ana_object *map_not_equals(ana_object *a, ana_object *b)
{
  return ana_boolfromint(a != b);
}

static ana_comparison_ops compops = {
  .obj_eq  = map_equals,
  .obj_neq = map_not_equals,
  .obj_gt  = NULL,
  .obj_lt  = NULL,
  .obj_gte = NULL,
  .obj_lte = NULL
};

static struct _ana_seq_ops seqops = {
  map_get,
  map_set
};

static ana_object *map_iterator_get(ana_object *obj)
{
  ana_map *map = (ana_map *)obj;
  ana_map_iterator *iter = malloc(sizeof(*iter));

  iter->base.flags = 0;
  iter->base.refcount = 0;
  iter->base.type = &ana_map_iterator_type;
  iter->base.next = NULL;

  iter->container = map;
  iter->current_bucket_position = 0;
  iter->current_bucket = NULL;
  iter->current_key = NULL;
  iter->current_value = NULL;
  iter->has_next = 1;

  return (ana_object *)iter;
}

static ana_object *map_iterator_string(ana_object *obj)
{
  char *val = ana_build_str("<map.iterator at %p>", (void *)obj);

  ana_object *retval = ana_stringfromstring(val);

  free(val);

  return retval;
}

static void map_iterator_dtor(ana_object *obj)
{
  free(obj);
}

static void map_iterator_print(ana_object *obj)
{
  printf("<map.iterator at %p>", (void *)obj);
}

static ana_object *map_array_iterator_next(ana_map_iterator *iterator)
{
  ana_map *map = iterator->container;

  for(; iterator->current_bucket_position < map->capacity; iterator->current_bucket_position++)
  {
    /* first bucket at this index */
    if(!iterator->current_bucket)
    {
      iterator->current_bucket =
        iterator->container->buckets[iterator->current_bucket_position];
    }

    ana_map_bucket *bucket = iterator->current_bucket;

    if(!bucket) 
    {

      if(iterator->current_bucket_position + 1 == map->capacity)
        goto done;

      continue;
    }

    while(bucket)
    {

      iterator->current_key  = bucket->key;
      iterator->current_value = bucket->value;

      iterator->current_bucket = bucket->next;

      if(iterator->current_bucket == NULL)
      {
        iterator->current_bucket_position++;
      }

      /* Yield this */
      return iterator->current_key;
    }
  }

done:
  return NULL;
}

ana_type ana_map_type = {
  .obj_name    = "map",
  .obj_print   = map_print,
  .obj_dtor    = map_dtor,
  .obj_equals  = NULL,
  .obj_bool    = NULL,
  .obj_hash    = NULL,
  .obj_str     = map_str,
  .obj_init    = NULL,
  .obj_deinit  = NULL,
  .obj_binops  = NULL,
  .obj_unops   = NULL,
  .obj_compops = &compops,
  .obj_seqops  = &seqops,
  .obj_get_attr = NULL,
  .obj_props    = NULL,
  .obj_iter     = map_iterator_get,
  .obj_iter_next = NULL
};

ana_type ana_map_iterator_type = {
  .obj_name    = "map.iterator",
  .obj_print   = map_iterator_print,
  .obj_dtor    = map_iterator_dtor,
  .obj_equals  = NULL,
  .obj_bool    = NULL,
  .obj_hash    = NULL,
  .obj_str     = map_iterator_string,
  .obj_init    = NULL,
  .obj_deinit  = NULL,
  .obj_binops  = NULL,
  .obj_unops   = NULL,
  .obj_compops = NULL,
  .obj_seqops  = NULL,
  .obj_get_attr  = NULL,
  .obj_props     = NULL,
  .obj_iter      = NULL,
  .obj_iter_next = (ana_iterator_function)map_array_iterator_next
};













#include <stdlib.h>
#include <stdio.h>

#include <ana.h>

static void ana_get_iterator(ana_iterator *iterator, ana_object *obj)
{
  ana_map *map = (ana_map *)obj;

  iterator->container = map;
  iterator->current_bucket_position = 0;
  iterator->current_bucket = NULL;
  iterator->current_key = NULL;
  iterator->current_value = NULL;
  iterator->has_next = 1;
}

static void ana_iterator_move( 
  ana_iterator *iterator)
{
  ana_map *map = ana_get_map(iterator->container);

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
      return;
    }
  }

done:
  iterator->has_next = 0;
}

int main(void)
{

  ana_object *map = ana_map_new(4);

  ana_object *key   = ana_stringfromstring("name");
  ana_object *value = ana_stringfromstring("ryan mccullagh");

  ana_map_put(map, key, value);

  key   = ana_stringfromstring("age");
  value = ana_longfromlong(27);

  ana_map_put(map, key, value);

  ana_iterator iterator;

  ana_get_iterator(&iterator, map);
  
  ana_iterator_move(&iterator);

  while(iterator.has_next)
  {
    ana_object *str_key   = ana_object_tostring(iterator.current_key);
    ana_object *str_value = ana_object_tostring(iterator.current_value);

    printf("%s => %s\n", ana_cstring(str_key), ana_cstring(str_value));

    ana_object_dtor(str_key);
    ana_object_dtor(str_value);

    ana_iterator_move(&iterator);
  }

}
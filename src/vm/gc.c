#include <ana.h>

static void incref_recursively(ana_object *obj)
{

  obj->refcount++;

  if(ana_type_is(obj, ana_array_type))
  {
    ana_array *array = ana_get_array(obj);

    ana_size_t i;

    for(i = 0; i < array->size; i++)
    {
      if(array->items[i] != obj)
      { 
        incref_recursively(array->items[i]);
      }
      else
      {
        array->items[i]->refcount++;
      }
    }
  }
  else if(ana_type_is(obj, ana_map_type))
  {
    ana_map_foreach(obj, key, value) {
      
      (void)key;

      if(value != obj)
      { 
        incref_recursively(value);
      }
      else
      {
        value->refcount++;
      }

    } ana_map_foreach_end();
  }
  else if(ana_type_is(obj, ana_instance_type))
  {
    ana_map_foreach(ana_get_instance(obj)->properties, key, value) {
      
      (void)key;

      if(value != obj) 
      {
        incref_recursively(value);
      }
      else
      {
        value->refcount--;
      }

    } ana_map_foreach_end();

    if(ana_get_instance(obj)->base_instance)
    {
      incref_recursively(ana_get_instance(obj)->base_instance);
    }
  }
}

static void decref_recursively(ana_object *obj)
{
  if(obj->refcount > 0)
    obj->refcount--;

  if(ana_type_is(obj, ana_array_type))
  {
    ana_array *array = ana_get_array(obj);

    ana_size_t i;

    for(i = 0; i < array->size; i++)
    {
      if(array->items[i] != obj) 
      {
        decref_recursively(array->items[i]);
      }
      else
      {
        array->items[i]->refcount--;
      }
    }
  }
  else if(ana_type_is(obj, ana_map_type))
  {
    ana_map_foreach(obj, key, value) {
      
      (void)key;

      if(value != obj) 
      {
        decref_recursively(value);
      }
      else
      {
        value->refcount--;
      }

    } ana_map_foreach_end();
  }
  else if(ana_type_is(obj, ana_instance_type))
  {
    ana_map_foreach(ana_get_instance(obj)->properties, key, value) {
      
      (void)key;

      if(value != obj) 
      {

        decref_recursively(value);
      }
      else
      {
        value->refcount--;
      }

    } ana_map_foreach_end();

    if(ana_get_instance(obj)->base_instance)
    {
      decref_recursively(ana_get_instance(obj)->base_instance);
    }
  }
}

static void mark_ex(ana_object *obj)
{
  if(obj->flags)
    return;

  obj->flags = 1;

  if(ana_type_is(obj, ana_array_type))
  {
    ana_array *array = ana_get_array(obj);

    ana_size_t i;

    for(i = 0; i < array->size; i++)
    {
      if(!array->items[i]->flags)
        mark_ex(array->items[i]);
    }
  }
  else if(ana_type_is(obj, ana_map_type))
  {
    ana_map_foreach(obj, key, value) {
      
      (void)key;

      mark_ex(value);

    } ana_map_foreach_end();
  }
  else if(ana_type_is(obj, ana_array_type))
  {
    ana_array_foreach(obj, index, value) {

      (void)index;

      mark_ex(value);

    } ana_array_foreach_end();
  }
  else if(ana_type_is(obj, ana_instance_type))
  {
    ana_map_foreach(ana_get_instance(obj)->properties, key, value) {
      
      (void)key;

      mark_ex(value);

    } ana_map_foreach_end();

    if(ana_get_instance(obj)->base_instance)
    {
      mark_ex(ana_get_instance(obj)->base_instance);
    }
  }
}

static void mark_frame(ana_frame *frame)
{
  ana_size_t i;

  ana_get_base(frame)->flags = 1;

  for(i = 0; i < frame->sp; i++)
  {
    ana_object *obj = frame->stack[i];

    if(!obj->flags)
      mark_ex(obj);
  }

  mark_ex(frame->locals);
}

static void mark(ana_vm *vm)
{
  if(vm->flags & COMO_VM_GC_DISABLED)
    return;

  ana_size_t i;

  for(i = 0; i < vm->stackpointer; i++)
  {
    ana_frame *frame = vm->stack[i];

    mark_frame(frame);
  }

  if(vm->base_frame)
    mark_frame(vm->base_frame);
}

static void sweep(ana_vm *vm)
{
  if(vm->flags & COMO_VM_GC_DISABLED)
    return;
  
  ana_object **root = &vm->root;

  while (*root) 
  {    
    if(!(*root)->flags) 
    {
      ana_object* unreached = *root; 
            
      assert(!ana_type_is(unreached, ana_function_type));

      /* it's possible that at the end of the frame
         a value on the stack is also the same value as a local
         in that moment the reference count can become 0,
         however, if a value on the stack contains a reference to this
         value, and also does the decrement, we would get to less than zero
       */
      if(unreached->refcount <= 0) 
      {
        *root = unreached->next;

        ana_object_dtor(unreached);

        vm->nobjs--;
      }
      else
      {
        #ifdef ANA_GC_DE
        ana_object *str = ana_object_tostring(unreached);
        printf("not releasing %p(%s, %s), it's reference count is %ld\n", 
            (void *)unreached, ana_type_name(unreached), ana_cstring(str), unreached->refcount);
        ana_object_dtor(str);
        #endif
        
        root = &(*root)->next;
      }
    } 
    else
    {
      (*root)->flags = 0;
      root = &(*root)->next;
    }
  }
}

static void gc(ana_vm *vm)
{
  if(vm->flags & COMO_VM_GC_DISABLED)
    return;

#ifdef ANA_GC_DEBUG
  fprintf(stderr, "gc: before sweep, nobjs=%ld\n", vm->nobjs);
#endif

  mark(vm);

  sweep(vm);


#ifdef ANA_GC_DEBUG
  fprintf(stderr, "gc: after sweep, nobjs=%ld\n", vm->nobjs);
#endif

  vm->mxobjs = vm->mxobjs * 2;
}

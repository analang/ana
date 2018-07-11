#include <ana.h>

static inline int invoke_class(
  ana_vm *vm,
  ana_object *class_defn,
  int argcount,
  ana_frame *frame)
{
  assert(ana_type_is(class_defn, ana_class_type));
  
  ana_class    *invoked_class = ana_get_class(class_defn);
  ana_object   *instances = ana_array_new(4);

  int calling_frame_pushed = 0;
  int total_ctors = 0;

  while(invoked_class)
  {    
    ana_instance *invoked_instance       = (ana_instance *)ana_instance_new((ana_object *)invoked_class);
    ana_function *invoked_constructor    = (ana_function *)ana_map_get(invoked_class->members, invoked_class->name);
    ana_frame *invoked_constructor_frame = NULL;

    if(invoked_constructor)
    {
      invoked_constructor_frame = (ana_frame *)ana_frame_new(
        ana_get_function_defn(invoked_constructor)->code,
        ana_get_function_defn(invoked_constructor)->jump_targets, 
        ana_get_function_defn(invoked_constructor)->line_mapping, 
        vm->global_frame->locals,
        invoked_constructor->name, 
        frame,
        frame->current_line,
        invoked_constructor->filename
      );

      invoked_constructor_frame->self = (ana_object *)invoked_instance;
      invoked_constructor_frame->retval = (ana_object *)invoked_instance;

      if(!calling_frame_pushed)
      {
        COMO_VM_PUSH_FRAME(frame);
        calling_frame_pushed = 1;
      }

      COMO_VM_PUSH_FRAME(invoked_constructor_frame);
      total_ctors++;
    }

    GC_TRACK(vm, invoked_instance);

    ana_array_push(instances, (ana_object *)invoked_instance);

    if(invoked_class->c_base)
    {
      assert(vm->global_frame->locals);

      ana_object *temp = ana_map_get(vm->global_frame->locals, invoked_class->c_base);
    
      if(temp && !ana_type_is(temp, ana_class_type))
      {
        Ana_SetError(AnaTypeError, "%s is not a class", ana_cstring(invoked_class->c_base));

        return 1;
      } 
      else if(!temp)
      {
        Ana_SetError(AnaTypeError, "%s", ana_cstring(invoked_class->c_base));

        return 1;
      } 
      
      invoked_class = ana_get_class(temp);
    }
    else
    {
      invoked_class = NULL;
    }
  }

  ana_size_t sp = vm->stackpointer - total_ctors;
  ana_size_t i;

  for(i = sp; i < vm->stackpointer; i++)
  {
    ana_frame *f = vm->stack[i];

    if(i + 1 < vm->stackpointer)
    {
      ana_map_put(f->locals, vm->base_symbol, vm->stack[i + 1]->self);
    }
  }

  for(i = 0; i < ana_array_size(instances); i++)
  {
    ana_instance *inst = ana_get_instance(ana_array_get(instances, i));

    if(i + 1 < ana_array_size(instances))
    {      
      /* TODO, determine if refcoutn needs to be increased */
      inst->base_instance = ana_array_get(instances, i + 1);
    }
  }

  ana_object_dtor(instances);

  return 0;
}


static inline ana_object *mul(ana_vm *vm, ana_object *a, ana_object *b)
{
  if(a->type->obj_binops != NULL && a->type->obj_binops->obj_mul != NULL) 
  {
    ana_object *res = a->type->obj_binops->obj_mul(a, b);
    if(res) 
    {
      GC_TRACK(vm, res);
      return res;
    }
  }
  return NULL;
}

static inline ana_object *sub(ana_vm *vm, ana_object *a, ana_object *b)
{
  if(a->type->obj_binops != NULL && a->type->obj_binops->obj_sub != NULL) 
  {
    ana_object *res = a->type->obj_binops->obj_sub(a, b);
    
    if(res) 
    {
      GC_TRACK(vm, res);
      
      return res;
    }
  }
  return NULL;
}

static inline ana_object *getindex(ana_vm *vm, ana_object *container, 
  ana_object *idx)
{
  if(container->type->obj_seqops != NULL && container->type->obj_seqops->get != NULL)
  {
    ana_object *res = container->type->obj_seqops->get(container, idx);

    if(!res)
    {
      return ana_bool_false;
    }

    if(!ana_type_is(container, ana_array_type) 
      && !ana_type_is(container, ana_map_type))
        GC_TRACK(vm, res);

    return res;
  }
  else
  {
    Ana_SetError(AnaTypeError, "operator `in` not supported for '%s' type",
      ana_type_name(container));
  }

  return NULL; 
}

static inline ana_object *setindex(ana_vm *vm, ana_object *container, 
  ana_object *idx, ana_object *val)
{

  if(container->type->obj_seqops != NULL && container->type->obj_seqops->set != NULL)
  {
    ana_object *prev = NULL;

    if(container->type->obj_seqops != NULL && container->type->obj_seqops->get != NULL)
    {
      prev = container->type->obj_seqops->get(container, idx);
    }

    ana_object *res = container->type->obj_seqops->set(container, idx, val);

    if(!res)
      return ANA_KEY_NOT_FOUND;

    /* array values and map values, are already part of the GC root */
    if(!ana_type_is(container, ana_array_type) 
      && !ana_type_is(container, ana_map_type))
        GC_TRACK(vm, res);

    if(prev && prev->refcount > 0) 
    {
      decref_recursively(prev);
    }

    incref_recursively(res);
    //->refcount++;

    return res;
  }

  return NULL; 
}

static inline ana_object *do_div(ana_vm *vm, ana_object *a, ana_object *b)
{
  ana_object *retval = NULL;

  if(a->type->obj_binops != NULL && a->type->obj_binops->obj_div != NULL) 
  {
    retval = a->type->obj_binops->obj_div(a, b);
    if(retval)
      GC_TRACK(vm, retval);
  }

  return retval;
}

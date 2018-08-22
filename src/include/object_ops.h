#define _GNU_SOURCE 1 /* For realpath */

#include <ana.h>
#include <stdio.h>
#include <limits.h>

#include "vmmacros.h"

static inline int setup_args(ana_vm *vm, ana_frame *frame, 
  ana_frame *execframe, ana_function *fn, int totalargs)
{
  int retval = 0;
  ana_function_defn *call = ana_get_function_defn(fn);

  if(!(fn->flags & COMO_FUNCTION_HAS_VARARGS) 
      && ana_get_array(call->parameters)->size != totalargs)
  {
    Ana_SetError(AnaArgumentError, "%s expects %lu argument(s), %d given",
      ana_cstring(fn->name), 
      ana_get_array(call->parameters)->size,
      totalargs
    );

    return 1;
  }

  if(ana_get_array(call->parameters)->size == 1 
      && (fn->flags & COMO_FUNCTION_HAS_VARARGS))
  {
    ana_object *paramname = ana_array_get(
      call->parameters, 0);

    ana_object *vargs = ana_array_new(4);

    GC_TRACK(vm, vargs);

    vargs->refcount++;

    while(totalargs--)
    {
      ana_object *theargvalue = pop2(frame);
      
      theargvalue->refcount++;

      ana_array_push(vargs, theargvalue);
    }  

    ana_map_put(execframe->locals, paramname, 
      ana_array_reverse(vargs)
    );
  }
  else if(ana_get_array(call->parameters)->size > 1 
      && (fn->flags & COMO_FUNCTION_HAS_VARARGS))
  {
    ana_object *arguments = ana_array_new(totalargs);

    while(totalargs--)
    {
      ana_array_push(arguments, pop2(frame));
    }

    ana_array_reverse(arguments);

    ana_size_t position = 0;
    ana_size_t i;

    for(i = 0; i < ana_array_size(call->parameters) - 1; i++)
    {
      ana_map_put(execframe->locals, 
        ana_array_get(call->parameters, i), ana_array_get(arguments, i));

      position++;
    }

    ana_object *vargs = ana_array_new(2);

    for(i = position; i < ana_array_size(arguments); i++)
    {
      ana_array_push(vargs, ana_array_get(arguments, position));
    }

    ana_map_put(execframe->locals,
      ana_array_get(call->parameters, ana_array_size(call->parameters) - 1),
      vargs
    );

    GC_TRACK(vm, vargs);

    ana_object_dtor(arguments);
  }
  else
  {
    while(totalargs--)
    {
      ana_object *theargvalue = pop2(frame);

      theargvalue->refcount++;

      ana_object *paramname = ana_array_get(
        call->parameters, (ana_size_t)totalargs);

      ana_map_put(execframe->locals, paramname, theargvalue);
    }
  }

  return retval;
}

/**
 TODO Unit test this exception 
 */
static char *get_relative_dir(ana_frame *frame)
{
  ana_object *curr_file_copy = ana_stringfromstring(ana_cstring(frame->filename));
  ana_object *parts = ana_array_new(4);
  char *path = ana_cstring(curr_file_copy);
  char part[50];
  size_t i = 0;

  while(*path)
  {
    if(*path == '/')
    {
      part[i] = '\0';
      ana_array_push(parts, ana_stringfromstring(part));
      memset(part, '\0', sizeof(part));
      i = 0;
    }
    else
    {
      if(i == sizeof(part) - 1)
      {
        Ana_SetError("ImportError", "Directory path is too large");

        ana_object_dtor(parts);
        ana_object_dtor(curr_file_copy);

        return NULL;
      }
      else
      {
        part[i++] = *path;
      }
    }

    path++;
  }

  size_t buffsize = 1;
  i = 0;
  char *buffer;

  ana_array_foreach(parts, index, value)
  {
    (void)index;
    buffsize += ana_get_string(value)->len + 1;
  } ana_array_foreach_end();

  buffer = malloc(buffsize);

  ana_array_foreach(parts, index, value)
  {
    (void)index;

    memcpy(buffer + i, ana_cstring(value), ana_get_string(value)->len);
    i += ana_get_string(value)->len;
    memcpy(buffer + i, "/", 1);
    i += 1;
  } ana_array_foreach_end();

  buffer[i] = '\0';


  ana_array_foreach(parts, index, value)
  {
    (void)index;

    ana_object_dtor(value);

  } ana_array_foreach_end();


  ana_object_dtor(parts);
  ana_object_dtor(curr_file_copy);

  return buffer;
}

static char *get_real_path(ana_frame *frame, ana_object *pathobj)
{
  char *relative_path = get_relative_dir(frame);

  if(!relative_path)
  {
    return NULL;
  }

  ana_object *pathobjcopy = ana_stringfromstring(ana_cstring(pathobj));
  char *pathwithext = NULL;
  char *path = ana_cstring(pathobjcopy);

  while(*path)
  {
    if(*path == '.')
    {
      *path = '/';
    }
  
    path++; 
  }

  path = ana_cstring(pathobjcopy);
  pathwithext = ana_build_str("%s%s.ana", relative_path, path);

  free(relative_path);
  ana_object_dtor(pathobjcopy);

  return pathwithext;
}

static int compile_file(char *path, FILE *fp, ana_parser_state *state)
{
  int retval;

  retval = ana_astfromfile(
    fp, path, 0, NULL, state);

  if(retval != 0)
  {
    Ana_SetError("ImportError", "%s", state->error);
    return 1;
  }

  return retval;
}

static inline int import_file(ana_vm *vm, ana_frame *frame, ana_object *alias, 
  ana_object *pathobj)
{ 
  char *path = get_real_path(frame, pathobj);
  int retval = 0;

  if(path == NULL)
  {
    import_error:

    Ana_SetError("ImportError", "`%s` was not located", ana_cstring(pathobj));
    
    return 1;
  }

  FILE *fp = ana_open_file_for_parsing(path);

  if(!fp)
  {
    goto import_error;
  }

  ana_parser_state parser_state;
  parser_state.debug = 0;
  parser_state.arena = ana_arena_new();

  int status = compile_file(path, fp, &parser_state);

  if(status != 0)
  {
    retval = 1;
    goto exit;
  }

  ana_compile_state compile_state;
  compile_state.ast = parser_state.ast;
  compile_state.filename = (char *)path;

  ana_module *code = ana_compilemodule(vm, &compile_state, 
    ana_cstring(pathobj));

  code->filename = ana_stringfromstring(path);

  ana_frame *execframe = ana_frame_new(
    ana_get_function_defn(code->func)->code,
    ana_get_function_defn(code->func)->jump_targets, 
    ana_get_function_defn(code->func)->line_mapping, 
    vm->global_frame->locals,
    code->name,
    frame,
    frame->current_line,
    pathobj
  );

  execframe->module = code;
  execframe->flags |= COMO_FRAME_MODULE;

  GC_TRACK(vm, code);

  ana_map_put(frame->locals, alias, (ana_object *)code);

  COMO_VM_PUSH_FRAME(frame);
  COMO_VM_PUSH_FRAME(execframe);

  code->members = execframe->locals;

  exit:

  ana_arena_free(parser_state.arena);

  if(fp != NULL)
  {
    fclose(fp);
  }

  free(path);

  return retval;
}

static inline int invoke_function(
  ana_vm *vm,
  ana_object *self,
  ana_object *function,
  int argcount,
  ana_frame *frame)
{
  assert(
    ana_type_is(function, ana_function_type) ||
    ana_type_is(function, ana_bounded_function_type)
  );

  call_function_type:

  if(ana_type_is(function, ana_function_type))
  {
    ana_function *fn = ana_get_function(function);

    if((fn->flags & COMO_FUNCTION_LANG) == COMO_FUNCTION_LANG)
    {
      struct _ana_function_def *call = fn->func;

      ana_frame *execframe = ana_frame_new(
        call->code,
        call->jump_targets, 
        call->line_mapping, 
        vm->global_frame->locals,
        fn->name, 
        frame,
        frame->current_line,
        frame->filename
      );

      if(frame->module)
        execframe->module = frame->module;

      if(self && ana_type_is(self, ana_instance_type))
      {
        printf("invoke_function: setting self to instance\n");
        execframe->self = self;

        ana_map_put(execframe->locals, vm->self_symbol, self);
      }

      if(setup_args(vm, frame, execframe, fn, argcount) != 0)
      {
        ana_object_dtor(execframe);
        
        return 1;
      }

      COMO_VM_PUSH_FRAME(frame);
      COMO_VM_PUSH_FRAME(execframe);
    }
    else
    {
      ana_object *nativeargs = ana_array_new(4);

      while(argcount--)
      {
        ana_object *thearg = pop();

        ana_array_push(nativeargs, thearg);
      }       

      ana_object *res = fn->handler(nativeargs); 

      if(res) 
      {
        GC_TRACK(vm, res);

        pushto(frame, res);
      }

      ana_object_dtor(nativeargs);

      return 1;
    }
  }
  else if(ana_type_is(function, ana_bounded_function_type))
  {           
    ana_bounded_function *bounded = ana_get_bounded_function(function);

    function = (ana_object *)bounded->func;

    self = bounded->self;

    goto call_function_type;
  }
  else
  {
    /* Unreachable */
    abort();
  }

  return 0;
}

static inline int invoke_class(
  ana_vm *vm,
  ana_object *class_defn,
  int argcount,
  ana_frame *frame,
  ana_object *self
  )
{
  assert(ana_type_is(class_defn, ana_class_type));
  
  ana_class    *invoked_class = ana_get_class(class_defn);
  ana_instance *invoked_instance = 
      (ana_instance *)ana_instance_new((ana_object *)invoked_class);

  ana_instance *most_child_instance = invoked_instance;

  GC_TRACK(vm, invoked_instance);

  ana_object   *instances = ana_array_new(4);
  ana_size_t i;

  int calling_frame_pushed = 0;
  int total_ctors = 0, invoked_constructors = 0;

  while(invoked_class)
  {    
    ana_function *invoked_constructor = 
        (ana_function *)ana_map_get(invoked_class->members, invoked_class->name);
    ana_frame *invoked_constructor_frame = NULL;

    if(self && ana_type_is(self, ana_module_type))
    {
      invoked_instance->module = ana_get_module(self);
    }

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
        frame->filename
      );

      ana_map_put(invoked_constructor_frame->locals, vm->self_symbol, 
          ana_get_base(invoked_instance));

      if(total_ctors == 0 && setup_args(vm, frame, invoked_constructor_frame,
        invoked_constructor, argcount) != 0)
      {
        ana_object_dtor(invoked_constructor);
        
        return 1;
      }

      invoked_constructor_frame->self = (ana_object *)invoked_instance;
      invoked_constructor_frame->retval = (ana_object *)invoked_instance;

      if(total_ctors != 0 && ana_array_size(
        ana_get_function_defn(invoked_constructor)->parameters) != 0)
      {
        /* Don't call base constructors unless they have 0 arguments */
        ana_object_dtor(invoked_constructor_frame);
        
        goto skip;
      }
      
      if(!calling_frame_pushed)
      {
        COMO_VM_PUSH_FRAME(frame);
        
        calling_frame_pushed = 1;
      }
      
      COMO_VM_PUSH_FRAME(invoked_constructor_frame);
      
      invoked_constructors++;
    }

    skip:
    
    total_ctors++;

    ana_array_push(instances, (ana_object *)invoked_instance);

    if(invoked_class->c_base)
    {
      ana_object *temp = ana_map_get(vm->global_frame->locals, invoked_class->c_base);
    
      if(temp && !ana_type_is(temp, ana_class_type))
      {
        Ana_SetError(AnaTypeError, 
          "%s is not a class", ana_cstring(invoked_class->c_base));

        return 1;
      } 
      else if(!temp)
      {

        return 1;
      } 
      
      invoked_class = ana_get_class(temp);
      invoked_instance = (ana_instance *)ana_instance_new((ana_object *)invoked_class);

      GC_TRACK(vm, invoked_instance);
    }
    else
    {
      invoked_class = NULL;
    }
  }

  if(invoked_constructors > 0)
  {
    vm->stack[vm->stackpointer - 1]->retval = (ana_object *)most_child_instance;
  }

  ana_size_t sp = vm->stackpointer - invoked_constructors;
  
  for(i = 0; i < ana_array_size(instances); i++)
  {
    ana_instance *inst = ana_get_instance(ana_array_get(instances, i));

    if(i + 1 < ana_array_size(instances))
    {      
      inst->base_instance = ana_array_get(instances, i + 1);
    }
  }

  assert(sp >= 0);

  for(i = sp; i < vm->stackpointer; i++)
  {
    ana_frame *f = vm->stack[i];

    if((i + 1) < vm->stackpointer)
    {
      ana_map_put(f->locals, vm->base_symbol, vm->stack[i + 1]->self);
    }
    else if(((ana_instance *)f->self)->base_instance)
    {
      ana_map_put(f->locals, vm->base_symbol, ((ana_instance *)f->self)->base_instance);
    }
  }

  assert(ana_array_size(instances) > 0);

  if(invoked_constructors == 0)
  {
    pushto(frame, ana_array_get(instances, 0));
  }

  ana_object_dtor(instances);

  return invoked_constructors;
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
      {
        GC_TRACK(vm, res);
      }


    res->refcount++;

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

#include <ana.h>

static inline ana_frame *create_function_frame(ana_frame *caller, ana_object *globals, 
    ana_function *function, 
    ana_function_defn *funcdef,
    int totalargs)
{  
  ana_frame *execframe = NULL;
  ana_array *parameters = ana_get_array(funcdef->parameters);

  if(parameters != totalargs)
  {
    Ana_SetError(AnaArgumentError, "%s expects %ld arguments, %d given",
      ana_cstring(function->name), parameters->size, totalargs
    );

    return NULL;
  }
  
  execframe = ana_frame_new(funcdef->code, funcdef->jump_targets, 
    funcdef->line_mapping, 
    globals,
    function->name, 
    caller,
    current_line,
    function->filename
  );

  while(totalargs--)
  {
    ana_object *theargvalue = pop2(caller);

    theargvalue->refcount++;

    ana_object *paramname = ana_array_get(parameters, (ana_size_t)totalargs);

    ana_map_put(execframe->locals, paramname, theargvalue);
  }

  return execframe;
}

static ana_frame *ana_setup_frame(ana_frame *caller, ana_object *globals,
  ana_function *function, int totalargs)
{
  if((function->flags & COMO_FUNCTION_LANG) == COMO_FUNCTION_LANG)
  {
    return create_function_frame(caller, globals, function, function->func, 
        totalargs);
  }
  else if((function->flags & COMO_FUNCTION_NATIVE) == COMO_FUNCTION_NATIVE)
  {
    return NULL;
  }
}

static inline ana_object *ana_call_native_method(ana_frame *caller, ana_function *fn, int totalargs)
{
  ana_object *nativeargs = ana_array_new(4);
  ana_object *res = NULL;

  while(totalargs--)
  {
    ana_object *thearg = pop2(caller);

    ana_array_push(nativeargs, thearg);
  }       

  res = fn->handler(nativeargs); 

  if(res) 
  {
    GC_TRACK(vm, res);
  }

  ana_object_dtor(nativeargs);  

  return res;
}


vm_target(CALL) {
  TRACE(CALL, oparg, 0, 1);

  ana_object *res = NULL;
  ana_object *callable = pop();
  ana_object *globals = BASE_FRAME->locals;

  if(ana_type_is(callable, ana_function_type)) 
  {
    ana_function *function = ana_get_function(callable);
      
    COMO_VM_PUSH_FRAME(frame);
    COMO_VM_PUSH_FRAME(execframe);
  }

  else if(ana_type_is(callable, ana_class_type))
  {
    ana_class *classdef = ana_get_class(callable);
    ana_object *inst = ana_instance_new(callable);
    ana_frame *base_constructor_frame = NULL;
    ana_object *ana_base_instance = NULL;
    ana_function *bc_func;
    ana_function_defn *bc_funcdef;
    ana_object *base_constructor = NULL;

    GC_TRACK(vm, inst);

    res = inst;
    
    ana_object *constructor;

    constructor = ana_map_get(ana_get_instance(inst)->self->members,
      ana_get_instance(inst)->self->name);

    /* TODO, now we must call the base contructor, only if there is a
       construct defined here. 
     */

    if(classdef->c_base)
    {
      ana_object *base_class = ana_map_get(frame->locals, 
        classdef->c_base);

      if(!base_class)
      {
        assert(frame->globals);
        base_class = ana_map_get(frame->globals, classdef->c_base);
      }

      if(!base_class)
      {
        set_except("NameError", "%s", ana_cstring(classdef->c_base));

        goto call_exit;
      }
      else
      {
        ana_base_instance = ana_instance_new(base_class);

        ana_get_instance(inst)->base_instance = ana_base_instance;
        
        base_constructor = ana_map_get(
          ana_get_class(base_class)->members, classdef->c_base);

        if(base_constructor)
        {
          bc_func = ana_get_function(base_constructor);
          bc_funcdef = bc_func->func;
        }
      }
    }

    if(constructor)
    {
      int totalargs = oparg;
      ana_function *fn = ana_get_function(constructor);

      struct _ana_function_def *call = fn->func;

      ana_frame *execframe = ana_frame_new(
        call->code,
        call->jump_targets, 
        call->line_mapping, 
        BASE_FRAME->locals,
        fn->name, 
        frame,
        current_line,
        fn->filename
      );

      if(call->parameters != NULL)
      {
        if(ana_get_array(call->parameters)->size != totalargs)
        {
          set_except(
            "ArgumentError", "%s expects %lu arguments, %d given",
            ana_get_fn_name(execframe), 
            ana_get_array(call->parameters)->size,
            totalargs);
          
          goto call_exit;
        }
      }
      else if(totalargs != 0)
      {
        set_except(
          "ArgumentError", "%s expects 0 arguments, %d given",
          ana_get_fn_name(execframe), totalargs);

        goto call_exit;
      }

      /* TODO, if there is a base instance, put this in 
         as "base" variable 
      */
      execframe->self = inst;

      if(ana_base_instance)
      {
        ana_map_put(execframe->locals, 
          vm->base_symbol, ana_base_instance);
      }
      
      ana_map_put(execframe->locals, 
        vm->self_symbol, inst);

      while(totalargs--)
      {
        ana_object *theargvalue = pop();

        theargvalue->refcount++;

        ana_object *paramname = ana_array_get(
          call->parameters, (ana_size_t)totalargs);

        ana_map_put(execframe->locals, paramname, theargvalue);
      }
      

      execframe->retval = inst;

      /* the base constructor */

      COMO_VM_PUSH_FRAME(frame);              
      COMO_VM_PUSH_FRAME(execframe);
      
      /* Base construct only get's called if it has 0 arguments */
      /* else, the calling class must call it explicitly */
      if(base_constructor 
        && ana_array_size(bc_funcdef->parameters) == 0)
      {
        base_constructor_frame = ana_frame_new(
          bc_funcdef->code,
          bc_funcdef->jump_targets, 
          bc_funcdef->line_mapping, 
          BASE_FRAME->locals,
          bc_func->name, 
          frame,
          current_line,
          bc_func->filename
        );

        base_constructor_frame->self = ana_base_instance;

        ana_map_put(base_constructor_frame->locals, 
          vm->self_symbol, ana_base_instance);

        base_constructor_frame->retval = ana_base_instance;

        COMO_VM_PUSH_FRAME(base_constructor_frame);
      }

      goto enter;  
    }
    else
    {
      if(base_constructor 
        && ana_array_size(bc_funcdef->parameters) == 0)
      {
        base_constructor_frame = ana_frame_new(
          bc_funcdef->code,
          bc_funcdef->jump_targets, 
          bc_funcdef->line_mapping, 
          BASE_FRAME->locals,
          bc_func->name, 
          frame,
          current_line,
          bc_func->filename
        );

        base_constructor_frame->self = ana_base_instance;

        ana_map_put(base_constructor_frame->locals, 
          vm->self_symbol, ana_base_instance);

        base_constructor_frame->retval = inst;

        COMO_VM_PUSH_FRAME(frame);
        /* base constructor is called first */
        COMO_VM_PUSH_FRAME(base_constructor_frame);

        goto enter;
      }
    }
  }
  else if(ana_type_is(callable, ana_instance_type))
  {
    // this will happen when a method calls base()
    int totalargs = oparg;
    ana_instance *instance = ana_get_instance(callable);
    ana_class *class_defn = instance->self; 
    ana_object *instance_name = class_defn->name;
    ana_object *constructor = NULL;
    /* ensure that base constructors are only called from 
       child constructors
    */
    if(frame->self != NULL)
    {
      assert(ana_type_is(frame->self, ana_instance_type));

      /* todo all base constructors need to be called */
      constructor = ana_map_get(class_defn->members, instance_name);

      if(constructor == NULL)
      {
        goto no_constructor_found;
      }      

      ana_instance *current_instance = ana_get_instance(frame->self);

      /* prevent instances from being called outside
         constructors (e.g. in class methods)
       */
      if(!(current_instance->self->name->type->obj_equals(current_instance->self->name,
        frame->name)))
      {
        Ana_SetError(InvalidOperation, 
          "Illegal invocation of %s.%s outside of %s.%s", 
           ana_cstring(instance_name),
           ana_cstring(instance_name),
           ana_cstring(ana_get_instance(frame->self)->self->name),
           ana_cstring(ana_get_instance(frame->self)->self->name)
        );

        goto call_exit;
      }

      /* Now we are sure this instance is being called inside a class constructor, but we now must check that this call is legal
         e.g. no passing instances to other classes */

      /* perhaps, an instance won't be called in a constructor if it doesn't have a base */

      if(current_instance->base_instance == NULL)
      {
        Ana_SetError(InvalidOperation, 
          "Illegal invocation of instance `%s`", ana_cstring(instance_name)
        );

        goto call_exit;               
      }

      /* Now check that this target instance call is an immediate parent of this current instance */

      ana_instance *parent_instance = (ana_instance *)current_instance->base_instance;

      if(!ana_object_equals(parent_instance->self->name, instance->self->name))
      {
         Ana_SetError(InvalidOperation, 
          "Illegal invocation of instance `%s`, as it's not an immediate parent of `%s`", 
          ana_cstring(instance_name),
          ana_cstring(current_instance->self->name)
        );

        goto call_exit;
      }


      if(constructor == NULL)
      {
        no_constructor_found:
        Ana_SetError(AnaTypeError, 
          "constructor for class %s is not defined", 
          ana_cstring(class_defn->name));   

        goto call_exit;            
      }
      else
      {
        ana_function *c_func = ana_get_function(constructor);
        ana_function_defn *c_func_def = c_func->func;

        ana_frame *execframe = ana_frame_new(
          c_func_def->code,
          c_func_def->jump_targets, 
          c_func_def->line_mapping, 
          BASE_FRAME->locals,
          c_func->name, 
          frame,
          current_line,
          c_func->filename
        );

        ana_map_put(execframe->locals, 
          vm->self_symbol, (ana_object *)instance);

        if(c_func_def->parameters != NULL)
        {
          if(ana_get_array(c_func_def->parameters)->size != totalargs)
          {
            set_except(
              "ArgumentError", "%s expects %lu arguments, %d given",
              ana_get_fn_name(execframe), 
              ana_get_array(c_func_def->parameters)->size,
              totalargs);
            
            goto call_exit;
          }
        }
        else if(totalargs != 0)
        {
          set_except(
            "ArgumentError", "%s expects 0 arguments, %d given",
            ana_get_fn_name(execframe), totalargs);

          goto call_exit;
        }

        while(totalargs--)
        {
          ana_object *theargvalue = pop();

          theargvalue->refcount++;

          ana_object *paramname = ana_array_get(
            c_func_def->parameters, (ana_size_t)totalargs);

          ana_map_put(execframe->locals, paramname, theargvalue);
        }

        /* TODO, if there is a base instance, put this in 
           as "base" variable 
        */
        execframe->self = (ana_object *)instance;

        COMO_VM_PUSH_FRAME(frame);
        COMO_VM_PUSH_FRAME(execframe);

        goto enter;
      }
    }
    else
    {
      Ana_SetError(InvalidOperation, 
        "Can't call instance type outside of a class constructor");
    }
  }
  else
  { 
    set_except("RuntimeError", "value of type '%s' is not callable",
      ana_type_name(callable));
  }

  if(res)
    push(res);

  call_exit:
  vm_continue();
}

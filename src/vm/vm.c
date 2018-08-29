#include <stdarg.h>
#define _GNU_SOURCE 1

#include <stdlib.h>
#include <assert.h>
#include <signal.h>
#include <string.h>
#include <time.h>
#include <stdint.h>

#include <stdio.h>
#include <limits.h>

#include "ana.h"
#include "vm.h"
#include "opcodes.h"
#include "ana_ast.h"
#include "ana_debug.h"
#include "utils.h"
#include "compiler.h"
#include "anaio.h"
#include "builtin.h"

#include "vmmacros.h"

static void trace_frame(ana_vm *vm, ana_frame *frame);
static void ana_print_backtrace(ana_frame *frame);
static void gc(ana_vm *vm);
static void decref_recursively(ana_object *obj);
static void incref_recursively(ana_object *obj);

static ana_frame *BASE_FRAME;
static ana_vm *AnaVM = NULL;

ana_vm *ana_vm_location(void)
{
  assert(AnaVM != NULL);

  return AnaVM;
}

static char *make_except(const char *fmt, ...)
{
  char *buffer = NULL;
  int size = 1024;
  va_list args;
  va_start (args, fmt);
  buffer = malloc(size + 1);
  vsnprintf(buffer, size, fmt, args);
  va_end (args);
  return buffer;
}

char *ex = NULL;
char *ex_type = NULL;

#define COMO_VM_PUSH_FRAME(frame) do { \
  if(vm->stackpointer >= COMO_VM_STACK_MAX) { \
    set_except("RuntimeError", "max call stack size reached, max frame stack size is %d", \
      COMO_VM_STACK_MAX); \
  } \
  vm->stack[vm->stackpointer++] = frame; \
  ana_get_base(frame)->next = (ana_object *)vm->frameroot; \
  vm->frameroot = ana_get_base(frame); \
} while(0)

#define COMO_VM_POP_FRAME() \
  vm->stack[--vm->stackpointer]

#define COMO_VM_HAS_FRAMES() (vm->stackpointer == 0)

#define ANA_KEY_NOT_FOUND ((void*)-1)

ana_vm *ana_vm_new()
{  
  ana_vm *vm = malloc(sizeof(ana_vm));
  ana_size_t i;

  for(i = 0; i < COMO_VM_STACK_MAX; i++)
    vm->stack[i] = NULL;

  vm->stacksize = 0;
  vm->stackpointer = 0;
  vm->exception = NULL;
  vm->flags = 0;
  vm->nobjs = 0;
  vm->do_gc = gc;
  vm->mxobjs = 24;
  vm->root = NULL;
  vm->symbols   = ana_array_new(8);
  vm->constants = ana_array_new(16);
  vm->self_symbol = ana_stringfromstring("self");
  vm->base_symbol = ana_stringfromstring("base");

  vm->base_frame = NULL;
  vm->global_frame = NULL;
  vm->frameroot = NULL;

  ana_bool_type_init();

  /* TODO, make a InitTabe with these setup/shutdown functions */
  ana_array_type_init(vm);
  
  ana_string_type_init(vm);

  ana_long_type_init(vm);

  return vm;
}

void ana_vm_finalize(ana_vm *vm)
{
  ana_array_foreach_apply(vm->symbols, ana_object_dtor);
  ana_object_dtor(vm->symbols);

  ana_object_dtor(vm->self_symbol);  
  ana_object_dtor(vm->base_symbol);

  ana_array_foreach_apply(vm->constants, ana_object_dtor);
  ana_object_dtor(vm->constants);

  ana_bool_type_finalize();
  ana_array_type_finalize(vm);
  ana_string_type_finalize(vm);
  ana_long_type_finalize(vm);

  free(vm);
}

ana_object *ana_vm_new_symbol(ana_vm *vm, char *symbol)
{
  ana_object *thesymbol = ana_stringfromstring(symbol);

  ana_array_push(vm->symbols, thesymbol);

  return thesymbol;
}

#include "object_ops.h"

static __attribute__((unused)) void inspect_stack(ana_frame *frame)
{
  fprintf(stdout, "Ana Stack Inspection (%s)\n", ana_cstring(frame->name));
  printf("    SP: %ld\n", frame->sp);

  ana_size_t i;

  for(i = 0; i < frame->sp; i++)
  {
    printf("    [%ld] => %p(%s)\n", i, 
      (void *)frame->stack[i], ana_cstring(ana_object_tostring(frame->stack[i])));
  }
}

static inline void trace_frame(ana_vm *vm, ana_frame *frame)
{
  fprintf(stdout, "Ana Virtual Machine Disassembler\n");
  fprintf(stdout, "  Frame: %s\n", ana_cstring(frame->name));
  fprintf(stdout, "  Constant Table: \n");
  fprintf(stdout, "   %-15s%-13s%-11s\n", "Index", "Value", "Type"); 

  ana_size_t i;

  for(i = 0; i < ana_array_size(vm->constants); i++)
  {
    ana_object *value = ana_array_get(vm->constants, i);
    assert(value); 
    ana_string *_sval = (ana_string *)ana_object_tostring(value);
    char *sval = _sval->value;

    if(_sval->len > 13)
    {
      sval[12] = '\0';
    }

    fprintf(stdout, "   %-15d%-13.25s%-11s\n", (int)i, sval, 
      value->type->obj_name);

    ana_object_dtor(_sval);
  }

  fprintf(stdout, "  Jump Targets: \n");
  
  ana_array_foreach(frame->jump_targets, i, value) {
    ana_string *_sval = (ana_string *)ana_object_tostring(value);
    char *sval = _sval->value;
    fprintf(stdout, "   %-15d%-13.25s%-11s\n", (int)i, sval, 
      value->type->obj_name);
    ana_object_dtor(_sval);
  } ana_array_foreach_end();

  fprintf(stdout, "  VM Symbols:\n");

  ana_array_foreach(vm->symbols, i, value) {
    ana_tostring_fast(value, {
      fprintf(stdout, "   %-15d%-13.25s\n", (int)i, value); 
    });
  } ana_array_foreach_end();

  fputc('\n', stdout);
}

static ana_object *ana_frame_eval(ana_vm *vm)
{
  ana_frame *frame;
  ana_long **jmptargets;
  ana_object **constants;
  ana_object *locals;

  ana_object *arg = NULL, *retval = NULL, *left, *right, *result;

  while(!COMO_VM_HAS_FRAMES())
  {    
    int current_line = 0;

    enter:
    assert(vm->stackpointer > 0);
    
    frame = COMO_VM_POP_FRAME();
    vm->base_frame = frame;

    unsigned int *code = frame->code;
    jmptargets = (ana_long **)(((ana_array *)frame->jump_targets)->items);
    constants = (ana_object **)(((ana_array *)vm->constants)->items);
    locals = frame->locals;

    if(vm->flags & COMO_VM_TRACING_ANY)
      trace_frame(vm, frame);

    ana_uint32_t opline;
    ana_uint32_t opcode;
    int next_op_code;
    short oparg;
    unsigned opflag;

    for(;;) {
      top:
      fetch();

      vm_case(opcode) {
        default: {
          Ana_SetError("VMError", "Opcode %#04x is not implemented", opcode);
          vm_continue();
        }
        vm_target(ITER) {
          /* This is the container */
          ana_object *iterable = pop();

          /* the container is already part of the GC root*/

          if(iterable->type->obj_iter == NULL)
          {
            set_except("TypeError", "%s object is not iterable",
              ana_type_name(iterable));
          }
          else
          {
            ana_object *iterator = iterable->type->obj_iter(iterable);
            
            /* The iterator is tracked */
            GC_TRACK(vm, iterator);

            /* We must hold a reference to it because it will be 
               reachable for the entire duration of this loop */
            iterator->refcount++;

            push(iterator);

          }

          vm_continue();   
        }
        vm_target(ITER_MV) {          
          ana_object *iterator = pop();
          
          assert(iterator);
          assert(iterator->type->obj_iter_next != NULL);

          ana_object *next = iterator->type->obj_iter_next(iterator);

          if(!next)
          {
            frame->pc = (*(jmptargets + oparg))->value;
            
            /* release lock on the iterator */
            iterator->refcount--;

            goto top;          
          }
          else
          {
            push(iterator);
            push(next);
          }

          vm_continue();
        }
        vm_target(ILAND) {
          ana_object *right = pop();
          ana_object *left = pop();

          if(!left->type->obj_bool(left))
          {
            /* This was falsy, so put this on the stack */
            push(left);
          }
          else if(!right->type->obj_bool(right))
          {
            push(right);
          }
          else
          {
            push(right);
          }

          vm_continue();
        }
        vm_target(ILOR) {
          // // operator, returns the first value that was true,
          // else the right value is returned
          //
          ana_object *right = pop();
          ana_object *left = pop();

          /* left is true */
          if(left->type->obj_bool(left))
          {
            push(left);

            vm_continue();
          }

          if(right->type->obj_bool(right))
          {
            push(right);

            vm_continue();
          }   

          push(right);

          vm_continue();
        }
        vm_target(ILSHFT) {
          ana_object *right = pop();
          ana_object *left = pop();

          if(left->type->obj_binops != NULL 
            && left->type->obj_binops->obj_ls != NULL)
          {
            ana_object *res = left->type->obj_binops->obj_ls(left, right);

            if(res)
            {
              push(res);
            }
          }
          else
          {

            set_except("TypeError", "unsupported operands for << operator");
          }

          vm_continue();
        }
        vm_target(IRSHFT) {
          ana_object *right = pop();
          ana_object *left = pop();

          if(left->type->obj_binops != NULL 
            && left->type->obj_binops->obj_rs != NULL)
          {
            ana_object *res = left->type->obj_binops->obj_rs(left, right);

            if(res)
            {
              push(res);
            }
          }
          else
          {

            set_except("TypeError", "unsupported operands for >> operator");
          }

          vm_continue();
        }
        vm_target(IUNARYNOT) {
          arg = pop();
          
          if(arg->type->obj_bool(arg) == 0)
          {
            push(ana_bool_true);
          }
          else
          {
            push(ana_bool_false);
          }

          vm_continue();
        }
        vm_target(IIN) {
          ana_object *container = pop();
          ana_object *index = pop();
          ana_object *res = getindex(vm, container, index);

          if(res)
          {

            push(res);
          }

          vm_continue();
        }
        vm_target(IUNARYMINUS) {
          arg = pop();

          if(arg->type->obj_unops != NULL 
            && arg->type->obj_unops->obj_minus != NULL)
          {
            result = arg->type->obj_unops->obj_minus(arg);

            if(result) 
            {
              GC_TRACK(vm, result);

              push(result);
            }
            else
              set_except("RuntimeError", "unsupported operand type for - operator");
          }
          else
          {
            set_except("RuntimeError", "unsupported operand type for - operator");
          }

          vm_continue();
        }
        vm_target(IUNARYPLUS) {
          arg = pop();

          if(arg->type->obj_unops != NULL 
            && arg->type->obj_unops->obj_plus != NULL)
          {
            result = arg->type->obj_unops->obj_plus(arg);

            if(result) {
              GC_TRACK(vm, result);
              push(result);
            }
            else
              set_except("RuntimeError", "unsupported operand type for + operator");
          }
          else
          {
            set_except("RuntimeError", "unsupported operand type for + operator");
          }

          vm_continue();
        }
        vm_target(ITHROW) {
          arg = pop();

          ana_tostring_fast(arg, {
            set_except("RuntimeError", value);
          });

          vm_continue();
        }
        vm_target(JMP) {
          frame->pc = (*(jmptargets + oparg))->value;
          goto top;
        }
        vm_target(JMPF) {
          result = pop();
          push(result);

          if(result->type->obj_bool(result) == 0)
          {     
            frame->pc = (*(jmptargets + oparg))->value;
            goto top;
          }

          vm_continue();
        }
        vm_target(JMPT) {
          result = pop();
          push(result);

          if(result->type->obj_bool(result) != 0)
          {     
            frame->pc = (*(jmptargets + oparg))->value;
            goto top;
          }

          vm_continue();
        }
        vm_target(JMPZ) {
          result = pop();
          
          assert(result);

          if(LIKELY(result->type == &ana_bool_type))
          {
            if(!(((ana_bool *)result)->value))
            {
              frame->pc = (*(jmptargets + oparg))->value;
              goto top;
            }
            else
            {
              goto done;
            }
          } 

          if(result->type->obj_bool(result) == 0)
          {     
            frame->pc = (*(jmptargets + oparg))->value;
            goto top;
          }

          done:
          vm_continue();
        }
        vm_target(BEGIN_LOOP) {
          /* TODO need to check if need resize */
          ana_basic_block *loop = malloc(sizeof(*loop));
          loop->stack_obj_count = 0;
          loop->next = frame->loop->root;
          frame->loop->root = loop;

          assert(frame->loop->stack_position < frame->loop->stack_capacity);

          frame->loop->stack[frame->loop->stack_position++] = loop;
          frame->loop->stack_size++;

          vm_continue();
        }
        vm_target(EXIT_LOOP_CONTINUE) {       
          /* all of pushs to this tack inside the block, need to be poped 
             if they weren't already
            */
          if(frame->loop->stack[frame->loop->stack_position - 1]->stack_obj_count < 0)
            frame->loop->stack[frame->loop->stack_position - 1]->stack_obj_count = 0;

          ana_size_t sz = frame->loop->stack[frame->loop->stack_position - 1]
              ->stack_obj_count;

          ana_size_t i;

          for(i = 0; i < sz; i++)
          {
            --frame->loop->stack[frame->loop->stack_position - 1]->stack_obj_count;

            (void)xpop();
          }

          vm_continue();
        }
        vm_target(END_LOOP) {
          /* now we must pop this loop block */
          --frame->loop->stack_position;
          --frame->loop->stack_size;
          /* frame->sp must be the same as it was when starting the loop */
          //printf("end of loop SP=%ld\n", frame->sp);
          vm_continue();
        }
        vm_target(TRY) {
          int arg = get_arg();
          /* Arg is the index into the jmptargets table */
          ana_object *jmptargetobj = ana_array_get(frame->jump_targets,
            (ana_size_t)arg);

          ana_basic_block *tryblock = 
            ana_basic_block_new((int)ana_get_long(jmptargetobj)->value);

          tryblock->next = frame->exception->root;
          frame->exception->root = tryblock;

          frame->exception->stack[frame->exception->stack_position++] 
            = tryblock;

          frame->exception->stack_size++;

          vm_continue();
        }
        vm_target(SETUP_CATCH) {
          int cindex = get_arg();
                    
          if(ex == NULL) 
          {
            assert(ana_excep != NULL);
          }

          arg = get_const(cindex);
          
          assert(arg != NULL);
          
          ana_object *exvalue = ana_stringfromstring(ex == NULL ? ana_excep : ex);
          
          GC_TRACK(vm, exvalue);
          
          ana_map_put(frame->locals, arg, exvalue);
          
          if(ex) 
          {
            free(ex);
          }
          
          if(ex)
          {
            ex = NULL;
            ex_type = NULL;
          }
          else
          {
            free(ana_excep);
            ana_excep = NULL;
            ana_except_type = NULL;
          }
          
          vm_continue();
        }
        vm_target(LOAD_SUBSCRIPT) {
          ana_object *index = pop();
          ana_object *container = pop();
          ana_object *res = getindex(vm, container, index);

          if(res == ANA_KEY_NOT_FOUND)
          {
            ana_tostring_fast(index, {
              set_except("KeyError", "%s", value);
            });
          }
          else if(res) 
          {
            push(res);
          }
          else
          {
            set_except("RuntimeError", "value does not support indexed access");
          }

          vm_continue(); 
        }
        vm_target(STORE_SUBSCRIPT) {
          ana_object *value = pop();
          ana_object *index = pop();
          ana_object *container = pop();
          ana_object *res = setindex(vm, container, index, value);
          
          if(res == ANA_KEY_NOT_FOUND)
          {
            set_except("KeyError", "index is out of range for assignment");
          }
          else if(!res)
          {
            set_except("RuntimeError", 
              "indexed assignment is not supported for type '%s'", 
              ana_type_name(container));
          }
          else
          {
            if(!opflag) 
            {
              push(res);
            }
          }

          vm_continue();
        }
        vm_target(INITARRAY) {
          result = ana_array_new(oparg);

          while(oparg--)
          {
            left = pop();
            
            ana_array_push(result, left);
            
            incref_recursively(left);
          }

          GC_TRACK(vm, result);

          push(result);
          
          vm_continue();
        }
        vm_target(INITOBJ) {
          int arg = get_arg();
          ana_object *obj = ana_map_new(arg > 0 ? arg : 2);

          while(arg--)
          {
            ana_object *val = pop();
            ana_object *key = pop();

            assert(ana_type_is(key, ana_string_type));

            ana_map_put(obj, key, val);

            incref_recursively(val);
          }

          GC_TRACK(vm, obj);

          push(obj);

          vm_continue();
        }
        vm_target(SETPROP) {
          arg = ana_array_get(vm->symbols, oparg);
          assert(arg);
          
          ana_object *value = pop();          
          ana_object *instance = pop();

          if(ana_type_is(instance, ana_map_type))
          {
            ana_object *prev;
            prev = ana_map_get(instance, arg);

            if(prev)
            {
              if(prev->refcount > 0)
                decref_recursively(prev);
            }

            ana_map_put(instance, arg, value);

            incref_recursively(value);
            
            if(!opflag)  
            {              
              push(value);
            }

            vm_continue();
          }

          /* prevent arbitrary properties from begin defined on non class types */
          if(!ana_type_is(instance, ana_instance_type))
          {
            Ana_SetError(InvalidOperation, "Can't set property on object of type %s",
              ana_type_name(instance));
          } 
          else  
          {


            //--- Begin chained resolution --------------------------
            ana_instance *ins = ana_get_instance(instance);
            ana_object *res = NULL;

            while(ins)
            {
              res = ana_map_get(ins->self->members, arg);

              if(res)
              {
                Ana_SetError(AnaTypeError, "Property `%s` is read-only", 
                  ana_cstring(arg));
                
                vm_continue();
              }
                
              if(!res)
              {
                res = ana_map_get(ins->properties, arg);
              }

              if(res)
              {
                decref_recursively(res);

                res = ana_map_put(ins->properties, arg, value);

                incref_recursively(value);

                break;
              }

              ins = ana_get_instance(ins->base_instance);
            }


            if(!res)
            {
              res = ana_map_put(
                ana_get_instance(instance)->properties, 
                arg, 
                value
              );
            }

            //----------------------------------------------------
            if(!opflag)
              push(res);
          } 
          vm_continue();
        }
        vm_target(GETPROP) {
          int argvalue = get_arg();
          ana_object *instance = pop();
          arg = ana_get_array(vm->symbols)->items[argvalue];

          if(ana_type_is(instance, ana_map_type)) 
          {
            ana_object *res = ana_map_get(instance, arg);

            if(res)
            {
              push(res);
            }
            else
            {
              set_except("KeyError", "%s", ana_cstring(arg));
            }
          }
          else if(instance->type->obj_props != NULL)
          {
            ana_object *res = ana_map_get(instance->type->obj_props, arg);

            if(res) 
            {
              /* NO GC, since these are only builtin methods, sometimes */
              push(res);
            }
            else
            {
              set_except("RuntimeError", "can't get property on object of type %s"
              ,ana_type_name(instance)); 
            }
          }
          else if(ana_type_is(instance, ana_instance_type))
          {
            /* Method resolution for GETPROP on instances
             * 
             * class definition, functions 
             * properties
             * locals
             */

            //--- Begin chained resolution --------------------------
            ana_instance *ins = ana_get_instance(instance);
            ana_object *res = NULL;

            while(ins)
            {

              assert(ins->self);
              assert(ins->self->members);

              res = ana_map_get(ins->self->members, arg);

              if(!res)
              {
                res = ana_map_get(ins->properties, arg);
              }

              if(res)
                break;

              ins = ana_get_instance(ins->base_instance);
            }
            //----------------------------------------------------

            if(!res)
            {
              Ana_SetError(AnaRuntimeError, "%s is not defined on %s instance",
                ana_get_string(arg)->value, 
                ana_cstring(ana_get_instance(instance)->self->name));
            }
            else
            {
              if(ana_type_is(res, ana_function_type) && 
                (next_op_code != CALL_METHOD))
              {
                res = ana_bounded_function_new(
                  instance, ana_get_function(res)
                );

                GC_TRACK(vm, res);
              }
              push(res);
            }
          }
          else 
          {
            Ana_SetError(AnaRuntimeError, "can't get property on object of type %s"
              ,ana_type_name(instance));
          }
          
          vm_continue();
        }
        vm_target(IRETURN) {
          /* this is for early return value binding, for class constructors instances */
          if(frame->retval)
          {
            arg = frame->retval;
          }
          else
          {
            arg = pop();
          }

          if(!COMO_VM_HAS_FRAMES())
          {
            ana_frame *caller = vm->stack[vm->stackpointer - 1];
            
            frame->retval = arg;

            pushto(caller, arg);
          }
                  
          goto exit; 
        }
        vm_target(LOAD_CONST) {
          arg = ana_array_get(vm->constants, oparg);
          push(arg);
          vm_continue();
        }
        vm_target(STORE_NAME) {

          ana_object *thename = ana_array_get(vm->symbols, oparg);
          result = pop();
          ana_object *oldvalue = NULL;
          ana_instance *ins = NULL;

          if(frame->self)
          {
            assert(ana_type_is(frame->self, ana_instance_type));

            ins = ana_get_instance(frame->self);

            while(ins)
            {
              oldvalue = ana_map_get(ins->self->members, arg);

              if(oldvalue)
              {
                Ana_SetError(AnaTypeError, "Property `%s` is read-only", 
                  ana_cstring(arg));
                
                vm_continue();
              }
                
              if(!oldvalue)
              {
                oldvalue = ana_map_get(ins->properties, arg);
              }

              if(oldvalue)
              {
                break;
              }

              ins = ana_get_instance(ins->base_instance);
            }

          }
          else
          {
            oldvalue = ana_map_get(locals, thename);
          }


          if(oldvalue)
          {

            decref_recursively(ana_get_base(oldvalue));          
          }       


          if(!frame->self)
          {
            ana_map_put(locals, thename, result);
          }
          else
          {
            ana_map_put(ana_get_instance(frame->self)->properties, thename, result);
          }

          result->refcount++;

          if(!opflag) 
          {
            push(result);
          }

          vm_continue();
        }
        vm_target(LOAD_NAME) {
          ana_object *thename  = ana_array_get(vm->symbols, oparg);
          ana_object *result   = ana_map_get(frame->locals, thename);

          if(!result && frame->self != NULL)
          {
            assert(ana_type_is(frame->self, ana_instance_type));

            ana_instance *instance = (ana_instance *)frame->self;

            result = ana_map_get(instance->properties, thename);
          }

          if(!result)
          {
            if(frame->globals) 
            {
              result = ana_map_get(frame->globals, thename);
            }
          }

          if(result) 
          {
            push(result);
          }
          else 
          {
            Ana_SetError(AnaNameError, "undefined variable '%s'", 
              ana_cstring(thename));
          }
          
          vm_continue();
        }
        vm_target(IDIV) {
          right = pop();
          left  = pop();

          result = do_div(vm, left, right);

          if(result) {
            push(result);
          }
          else
            set_except("RuntimeError", "unsupported operands for / operator");
          vm_continue();       
        }
        vm_target(IEQUAL) {
          right = pop();
          left  = pop();
          result = NULL;

          if(left->type->obj_compops != NULL 
                && left->type->obj_compops->obj_eq != NULL)
          {
            /*
              result does not need to be GC'd because it is an instance of 
              bool 
             */
            result = left->type->obj_compops->obj_eq(left, right);
            if(result)
            {
              push(result);
            }
          }
          else
          {
            if(left == right)
            {
              push(ana_bool_true);
            }
            else
            {
              push(ana_bool_false);
            }
          }

          vm_continue();
        }
        vm_target(INEQUAL) {
          right = pop();
          left  = pop();
          result = NULL;

          if(left->type->obj_compops != NULL 
                && left->type->obj_compops->obj_neq != NULL)
          {
            /* this will return the singelton bool instance */
            result = left->type->obj_compops->obj_neq(left, right);
          }

          if(result)
          {
            push(result);
          }
          else
          {
            if(left != right)
            {
              push(ana_bool_true);
            }
            else
            {
              push(ana_bool_false);
            }
          }

          vm_continue();
        }
        vm_target(IREM) {
          right = pop();
          left  = pop();
          result = NULL;

          if(left->type->obj_binops != NULL 
                && left->type->obj_binops->obj_rem != NULL)
          {
            result = left->type->obj_binops->obj_rem(left, right);
          }

          if(result)
          {
            GC_TRACK(vm, result);

            push(result);
          }
          else
          {
            set_except("NotImplementedError", "%% operation is not implemented"
              "for type %s", ana_type_name(left));
          }

          vm_continue();
        }  
        vm_target(IADD) {

          /* Here we have a reference to two objects that may
             have been GC allocated */
          right = pop();
          left  = pop();

          result = NULL;

          if(ana_type_check_both(left, right, ana_long_type))
          {
            long val = ana_get_long(left)->value + ana_get_long(right)->value;
            
            result = ana_longfromlong(val);
          } 
          else 
          {
            if(left->type->obj_binops != NULL 
              && left->type->obj_binops->obj_add != NULL) 
                result = left->type->obj_binops->obj_add(left, right);
          }

          if(result) 
          {
            /* what happens here, is that we've pushed an 
               object onto the stack, then call GC, 
               this will mark the current object, but we need it later on
            */
            GC_TRACK(vm, result);
            /* GC_TRACK may run the GC, then add this object to the root
               it's important that the push happens after, since push
                increases the stack pointer 
             */
            push(result);
          }
          else
            set_except("RuntimeError", "unsupported operands for + operator");
          
          vm_continue();
        }
        vm_target(ILT) {
          right = pop();
          left  = pop();

          result = NULL;

          if(ana_type_check_both(left, right, ana_long_type))
          {
            if(ana_get_long(left)->value < ana_get_long(right)->value)
              result = ana_bool_true;
            else
              result = ana_bool_false;    

            push(result);
          }
          else if(left->type->obj_compops != NULL 
              && left->type->obj_compops->obj_lt != NULL) 
          {
            /* this is just a boolean which only exists in a singleton */
            /* do not add to GC */
            result = left->type->obj_compops->obj_lt(left, right);
            
            if(result) 
            {
              push(result);
            }

            vm_continue();
          }
          else
          {
            set_except("TypeError", "unsupported operands for < operator");
          }

          vm_continue(); 
        }
        vm_target(IGT) {
          right = pop();
          left  = pop();

          result = NULL;

          if(ana_type_check_both(left, right, ana_long_type))
          {
            if(ana_get_long(left)->value > ana_get_long(right)->value)
              result = ana_bool_true;
            else
              result = ana_bool_false; 

            push(result);
          }
          else if(left->type->obj_compops != NULL 
              && left->type->obj_compops->obj_lt != NULL) 
          {
            /* this is just a boolean which only exists in a singleton */
            /* do not add to GC */
            result = left->type->obj_compops->obj_lt(left, right);

            if(result)
            {
              push(result);
            }

            vm_continue();
          }
          else
          {
            set_except("TypeError", "unsupported operands for < operator");
          }

          vm_continue(); 
        }
        vm_target(ILTE) {
          right = pop();
          left  = pop();

          result = NULL;

          if(ana_type_check_both(left, right, ana_long_type))
          {
            if(ana_get_long(left)->value <= ana_get_long(right)->value)
              result = ana_bool_true;
            else
              result = ana_bool_false;    

            push(result);
          }
          else if(left->type->obj_compops != NULL 
              && left->type->obj_compops->obj_lte != NULL) 
          {
            /* this is just a boolean which only exists in a singleton */
            /* do not add to GC */
            result = left->type->obj_compops->obj_lte(left, right);

            if(result) 
            {
              /* bool objects aren't GC'd, there are only 2 instances throught 
                 the entire run 
               */
              push(result);
            }
          }
          else 
          {
            set_except("RuntimeError", "unsupported operands for <= operator");
          }

          vm_continue(); 
        }
        vm_target(IGTE) {
          right = pop();
          left  = pop();

          result = NULL;

          if(ana_type_check_both(left, right, ana_long_type))
          {
            if(ana_get_long(left)->value >= ana_get_long(right)->value)
              result = ana_bool_true;
            else
              result = ana_bool_false;

            push(result);
          }
          else if(left->type->obj_compops != NULL 
              && left->type->obj_compops->obj_lte != NULL) 
          {
            /* this is just a boolean which only exists in a singleton */
            /* do not add to GC */
            result = left->type->obj_compops->obj_gte(left, right);

            if(result)
            {
              push(result);
            }
          }
          else 
          {
            set_except("RuntimeError", "unsupported operands for >= operator");
          }

          vm_continue();
        }
        vm_target(ITIMES) {
          right = pop();
          left  = pop();

          result = mul(vm, left, right);

          if(result) 
          {
            push(result);
          }
          else
            set_except("RuntimeError", "unsupported operands for * operator");

          vm_continue();
        }
        vm_target(IMINUS) {
          right = pop();
          left  = pop();

          result = sub(vm, left, right);

          if(result) 
          {
            push(result);
          }
          else
            set_except("RuntimeError", "unsupported operands for - operator");

          vm_continue();
        }
        vm_target(DEFINE_CLASS) {
          ana_object *baseclass = NULL;

          if(opflag)
          {
            baseclass = pop();
          }

          ana_object *name = pop();
          ana_object *fncount = pop();
          ana_class *theclass = (ana_class *)ana_class_new(baseclass, name);
          long i = ana_get_long(fncount)->value;

          while(i--)
          {
            ana_object *function_name = pop();
            ana_object *function_code = pop();

            ana_map_put(theclass->members, function_name, function_code);
          }

          ana_object *prev;

          if((prev = ana_map_get(frame->locals, name)) != NULL)
          {
            decref_recursively(prev);
          }

          ana_map_put(frame->locals, name, (ana_object *)theclass);

          ana_get_base(theclass)->refcount++;

          GC_TRACK(vm, theclass);

          vm_continue();
        }
        vm_target(DEFINE_FUNCTION) {
          ana_object *name = pop();
          ana_object *code = pop();
          ana_map_put(frame->locals, name, code);
          vm_continue();
        }
        vm_target(CALL_METHOD) {
          ana_object *res = NULL;
          ana_object *callable = pop();
          ana_object *self = pop();
          
          int totalargs = oparg;

          if(ana_type_is(callable, ana_function_type))
          {
            ana_function *func = ana_get_function(callable);

            if((ana_get_function(callable)->flags & COMO_FUNCTION_LANG) 
                == COMO_FUNCTION_LANG)
            {
              int totalargs = oparg;
              ana_function *fn = ana_get_function(callable);
        
              struct _ana_function_def *call = fn->func;

              ana_frame *execframe = ana_frame_new(
                call->code,
                call->code_size,
                call->jump_targets, 
                call->line_mapping, 
                BASE_FRAME->locals,
                fn->name, 
                frame,
                current_line,
                frame->filename
              );


              if(setup_args(vm, frame, execframe, fn, totalargs) != 0)
              {
                ana_object_dtor(execframe);
                vm_continue();
              }

              /* Make sure this isn't a module funciton */
              if(ana_type_is(self, ana_instance_type))
              {
                /* figure out if this is the base or parent */
                /* base instnace being null would mean this is the parent class
                   , so this must be the child 
                */

                ana_instance *i = ana_get_instance(self);
                ana_object *real_self = self;

                while(i)
                {                
                  if(ana_map_get(i->self->members, fn->name) != NULL)
                  {
                    real_self = (ana_object *)i;
                    break;
                  }

                  i = ana_get_instance(i->base_instance);
                }

                if(ana_get_instance(real_self)->base_instance != NULL)
                  ana_map_put(execframe->locals, 
                    vm->base_symbol, ana_get_instance(real_self)->base_instance);

                ana_map_put(execframe->locals, 
                  vm->self_symbol, real_self);

                execframe->self = real_self;
              }

              COMO_VM_PUSH_FRAME(frame);
              COMO_VM_PUSH_FRAME(execframe);

              goto enter;            
            }
            else if( (ana_get_function(callable)->flags 
                & COMO_FUNCTION_NATIVE_METHOD) == COMO_FUNCTION_NATIVE_METHOD)
            {
              if(ana_array_size(ana_get_function(callable)->method.m_parameters) 
                  != (ana_size_t)totalargs)
              {
                 Ana_SetError(AnaArgumentError, "%s expects %ld argument(s), %d given",
                  ana_cstring(ana_get_function(callable)->name), 
                  ana_array_size(ana_get_function(callable)->method.m_parameters),
                  totalargs);      

                vm_continue();
              }

              ana_object *nativeargs = ana_array_new(4);

              while(totalargs--)
              {
                ana_object *thearg = pop();

                ana_array_push(nativeargs, thearg);
              }

              res = ana_get_function(callable)->method.m_handler(self, 
                oparg == 1 ? ana_get_array(nativeargs)->items[0] : nativeargs);
              
              /* TODO flag to check if it's already tracked */
              /* this value may already be tracked */
              if(oparg > 0)
              {
                /* TODO Check to make sure the returned value is not an argument */
                /* because the arguments on the stack may either be constants
                   or already part of the GC root 
                */
                int found_reflected = 0;
                int i;
                for(i = 0; i < oparg; i++)
                {
                  /* To do this is single dimensional, will have bugs later */
                  if(res == ana_get_array(nativeargs)->items[i])
                  {
                    found_reflected = 1;
                    break;
                  }
                }

                if(!found_reflected) 
                {
                  if(!res->is_tracked) 
                  {
                    GC_TRACK_DIMENSIONAL(vm, res);
                  }
                  //GC_TRACK(vm, res);
                }
              }
              else
              {
                /* If this is already tracked this can be a bug */
                /* for example:
                 *
                 * a search method, which takes a key and returns a value in an array
                   that was already tracked, here we would track it twice.
                   TODO make a flag where we say if it is already tracked
                 */

                /* is_this_tracked? */
                
                if(!res->is_tracked)
                {
                  GC_TRACK_DIMENSIONAL(vm, res);
                }

                //GC_TRACK(vm, res);
              }

              if(res)
              {
                push(res);
              }

              ana_object_dtor(nativeargs);
            }
            else if( (ana_get_function(callable)->flags & COMO_FUNCTION_NATIVE)
              == COMO_FUNCTION_NATIVE)
            {
              /* this is a just a native function wrapper into a variable */
            
              ana_object *nativeargs = ana_array_new(4);

              while(totalargs--)
              {
                ana_object *thearg = pop();

                ana_array_push(nativeargs, thearg);
              }       

              ana_object *res = func->handler(nativeargs); 

              if(res) 
              {
                GC_TRACK(vm, res);
              }

              ana_object_dtor(nativeargs);
            }
          }
          else if(ana_type_is(callable, ana_class_type))
          {
            /* This happens on a class property assign to a class */
            if(invoke_class(vm, callable, totalargs, frame, self) == 0)
            {
              vm_continue();
            }
            else
            {
              goto enter;
            }
          }
          else
          {
            Ana_SetError(InvalidOperation, "Cannot call object of type `%s`",
              ana_type_name(callable));

            vm_continue();
          }

          vm_continue();
        }
        vm_target(CALL) {
          ana_object *res = NULL;
          ana_object *callable = pop();
          ana_object *self = NULL;

          if(ana_type_is(callable, ana_function_type) || 
                ana_type_is(callable, ana_bounded_function_type)) 
          {
            if(invoke_function(vm, self, callable, oparg, frame) != 0)
              vm_continue();
            else
              goto enter;
          }
          else if(ana_type_is(callable, ana_class_type))
          {
            /* This happens on a class property assign to a class */
            if(invoke_class(vm, callable, (int)oparg, frame, NULL) == 0)
            {
              vm_continue();
            }
            else
            {
              goto enter;
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
              if(!ana_object_equals(current_instance->self->name, frame->name))
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
                  c_func_def->code_size,
                  c_func_def->jump_targets, 
                  c_func_def->line_mapping, 
                  BASE_FRAME->locals,
                  c_func->name, 
                  frame,
                  current_line,
                  frame->filename
                );

                ana_map_put(execframe->locals, 
                  vm->self_symbol, (ana_object *)instance);

                if(c_func_def->parameters != NULL)
                {
                  if(ana_get_array(c_func_def->parameters)->size != totalargs)
                  {
                    Ana_SetError(
                      AnaArgumentError, "%s expects %lu argument(s), %d given",
                      ana_get_fn_name(execframe), 
                      ana_get_array(c_func_def->parameters)->size,
                      totalargs);
                    
                    goto call_exit;
                  }
                }
                else if(totalargs != 0)
                {
                  Ana_SetError(
                    AnaArgumentError, "%s expects 0 argument(s), %d given",
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
            Ana_SetError(InvalidOperation, "Cannot call object of type `%s`",
              ana_type_name(callable));

            goto call_exit;
          }

          if(res)
            push(res);

          call_exit:
          vm_continue();
        }
      }

      if(ex || ana_excep) 
      {
        char *the_message = ex ==  NULL ? ana_excep : ex;
        char *the_type = ex_type == NULL ? ana_except_type : ex_type;

        ana_frame *thisframe = frame;
        ana_frame *originalframe = frame;

        while(thisframe != NULL) 
        {

          ana_basic_block *block;
          
          while(!(thisframe->exception->stack_position == 0))
          {
            block = thisframe->exception->stack[--thisframe->exception->stack_position];

            thisframe->pc = (ana_size_t)block->jmpto;
            
            /* do finalization here */
            
            /* frame that threw the exception */
            /* code won't continue from there */
            if(originalframe != thisframe)
            {
              while(!(originalframe->sp == 0)) 
              {
                ana_object *temp = pop2(originalframe);

                assert(temp);

                decref_recursively(temp);
              }

              ana_map_foreach(originalframe->locals, key, value) 
              {
                (void)key;

                decref_recursively(value);

              } ana_map_foreach_end();

              /* finalize this frame, as we won't be returning*/
              ana_object_finalize(originalframe);
              ana_object_dtor(originalframe);
              vm->base_frame = NULL;
            }

            /* frame with the exception handler */
            /* If the exception handler wasn't located in the current executing frame */
            COMO_VM_PUSH_FRAME(thisframe);

            goto enter; 
          } 

          if(!COMO_VM_HAS_FRAMES()) 
          {
            thisframe = COMO_VM_POP_FRAME();
          }
          else
          {
            thisframe = NULL;
          }
        }
        int current_line;
        ana_object *pc = ana_longfromlong((long)frame->pc);
        ana_object *theline = ana_map_get(frame->line_mapping, pc);
        
        if(theline) {
          current_line = (int)ana_get_long(theline)->value;
          frame->current_line = current_line; 
        }
        else {
          current_line = 0;
        }

        ana_object_dtor(pc);

        ana_object *filename;     
        filename = frame->filename;
            
        fprintf(stdout, "%s: %s\n    in %s:%d\n", 
          the_type, 
          the_message, 
          ana_cstring(filename), 
          current_line
        );

        free(ex);

        ana_print_backtrace(frame);
      
        exit(1);
      }
    }

    exit:
    while(!empty()) 
    {
      ana_object *temp = pop();

      assert(temp);

      decref_recursively(temp);
    }

    ana_map_foreach(frame->locals, key, value) 
    {
      (void)key;

      decref_recursively(value);
    } ana_map_foreach_end();
    
    ana_object_finalize(frame);
    ana_object_dtor(frame);
    vm->base_frame = NULL;
  }

  return retval;
}

static void ana_print_backtrace(ana_frame *frame)
{
  ana_frame *fm = frame;

  while(fm != NULL)
  {
    if(fm == BASE_FRAME)
      goto next;

    char *fnname = ana_get_fn_name(fm);

    fprintf(stdout, "\tat %s (%s:%ld)\n", 
      fnname,
      ana_cstring(fm->filename),
      fm->activation_line_number
    );

    free(fnname);

    next:
      fm = fm->caller;
  }
}

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
        #ifdef ANA_GC_DEBUG1
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

static int trace_function(ana_vm *vm, char *function_name)
{
  AnaVM = vm;

  ana_size_t i;
  ana_array *constants = ana_get_array(vm->constants);
  ana_object *function_name_obj = ana_stringfromstring(function_name);
  ana_function *function = NULL;

  for(i = 0; i < constants->size; i++)
  {
    ana_object *value = constants->items[i];

    if(ana_type_is(value, ana_function_type))
    {
      ana_function *func = ana_get_function(value);

      if(func->name->type->obj_equals(func->name, function_name_obj))
      {
        function = func;

        break;
      }
    }
  }

  if(function == NULL)
  {
    printf("function '%s' is not defined\n", function_name);
    exit(1);
  }

  ana_function_defn *func = ANA_GET_FUNCTION_DEF(function);

  ana_frame *firstframe = ana_frame_new(
    func->code,
    func->code_size,
    func->jump_targets, 
    func->line_mapping, 
    NULL,
    function->name, 
    NULL,
    0,
    function->filename
  );

  BASE_FRAME = firstframe;

  COMO_VM_PUSH_FRAME(firstframe);

  ana_frame_eval(vm);

  gc(vm);

  return 0;
}

int ana_eval(ana_vm *vm, ana_function *functionobj, char *function)
{
  AnaVM = vm;

  ana_function_defn *func = ANA_GET_FUNCTION_DEF(functionobj);

  if(function && (vm->flags & COMO_VM_TRACING)) 
  {
    return trace_function(vm, function);
  }

  ana_frame *firstframe = ana_frame_new(
    func->code,
    func->code_size,
    func->jump_targets, 
    func->line_mapping, 
    NULL,
    functionobj->name, 
    NULL,
    0,
    functionobj->filename
  );

  vm->global_frame = firstframe;

  BASE_FRAME = firstframe;

  COMO_VM_PUSH_FRAME(firstframe);

  ana_frame_eval(vm);

  gc(vm);

  ana_object_dtor(functionobj);

  return 0;
}

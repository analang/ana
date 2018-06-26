#include <stdarg.h>
#include <assert.h>
#include <string.h>
#include <time.h>
#include <stdint.h>

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

static ana_frame *BASE_FRAME;

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
  vm->base_frame = NULL;

  vm->frameroot = NULL;

  ana_bool_type_init();

  /* TODO, make a InitTabe with these setup/shutdown functions */
  ana_array_type_init(vm);
  
  ana_string_type_init(vm);

  return vm;
}

void ana_vm_finalize(ana_vm *vm)
{
  ana_array_foreach_apply(vm->symbols, ana_object_dtor);
  ana_object_dtor(vm->symbols);

  ana_array_foreach_apply(vm->constants, ana_object_dtor);
  ana_object_dtor(vm->constants);

  ana_bool_type_finalize();

  ana_array_type_finalize(vm);
  
  ana_string_type_finalize(vm);

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
  fprintf(stdout, "Ana Stack Inspection (%s)\n",
    ana_cstring(frame->name));
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
  ana_uint32_t **code;
  ana_long **jmptargets;
  ana_object **constants;
  ana_object *locals;

  ana_object *arg = NULL, *retval = NULL, *left, *right, *result;

  while(!COMO_VM_HAS_FRAMES())
  {    
    int current_line = 0;

    enter:

    frame = COMO_VM_POP_FRAME();
    vm->base_frame = frame;

    code = (ana_uint32_t **)(((ana_container *)frame->code)->data);
    jmptargets = (ana_long **)(((ana_array *)frame->jump_targets)->items);
    constants = (ana_object **)(((ana_array *)vm->constants)->items);
    locals = frame->locals;


    if(vm->flags & COMO_VM_TRACING_ANY)
      trace_frame(vm, frame);

    ana_uint32_t opline;
    ana_uint32_t opcode;
    short oparg;
    unsigned opflag;

    for(;;) {
      top:
      /* this is for exceptions, we need to be able to store the line that
         was active prior to exception handling. The PC will have changed in
         the case of a function call because it's actually a new code object
       */
      fetch();

      vm_case(opcode) {
        default: {
          set_except("VMError", "Opcode %#04x is not implemented\n", opcode);
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
        vm_target(ILOR)
        {
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
        vm_target(IUNARYNOT)
        {
          TRACE(IUNARYNOT, oparg, 0, 1);
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
          TRACE(IIN, oparg, 0, 1);
          ana_object *container = pop();
          ana_object *index = pop();
          ana_object *res = getindex(vm, container, index);

          if(res == ANA_KEY_NOT_FOUND)
            push(ana_bool_false);
          else
            push(ana_bool_true);

          vm_continue();
        }
        vm_target(IUNARYMINUS) {
          TRACE(IUNARYMINUS, 0, 0, 1);
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
          TRACE(IUNARYPLUS, 0, 0, 1);
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
          TRACE(ITHROW, 0, 0, 1);  
          arg = pop();

          ana_tostring_fast(arg, {
            set_except("RuntimeError", value);
          });

          vm_continue();
        }
        vm_target(JMP) {
          TRACE(JMP, oparg, 0, 1);
          frame->pc = (*(jmptargets + oparg))->value;
          goto top;
        }
        vm_target(JMPZ) {
          TRACE(JMPZ, oparg, 0, 1);
          result = pop();

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
        vm_target(BEGIN_LOOP)
        {
          /* TODO need to check if need resize */
          ana_basic_block *loop = malloc(sizeof(*loop));
          loop->stack_obj_count = 0;
          loop->next = frame->loop->root;
          frame->loop->root = loop;

          frame->loop->stack[frame->loop->stack_position++] = loop;
          frame->loop->stack_size++;

          vm_continue();
        }
        vm_target(EXIT_LOOP_CONTINUE)
        {       
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
        vm_target(END_LOOP)
        {
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
          TRACE(TRY, arg, 0, 1);
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
        vm_target(SETUP_CATCH) 
        {
          int cindex = get_arg();
          
          TRACE(SETUP_CATCH, cindex, 0, 1);
          
          if(ex == NULL) {
            assert(ana_excep != NULL);
          }

          arg = get_const(cindex);
          
          assert(arg != NULL);
          
          ana_object *exvalue = ana_stringfromstring(ex == NULL ? ana_excep : ex);
          
          GC_TRACK(vm, exvalue);
          ana_map_put(frame->locals, arg, exvalue);
          
          if(ex) {
            free(ex);
          }
          
          if(ex)
          {
            ex = NULL;
            ex_type = NULL;
          }
          else
          {
            ana_excep = NULL;
            ana_except_type = NULL;
          }
          
          vm_continue();
        }
        vm_target(LOAD_SUBSCRIPT) 
        {
          TRACE(LOAD_SUBSCRIPT, 0, 0, 0);
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
        vm_target(STORE_SUBSCRIPT) 
        {
          TRACE(STORE_SUBSCRIPT, 0, 0, 0);
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
          TRACE(INITARRAY, oparg, 0, 1);
          
          result = ana_array_new(oparg);

          while(oparg--)
          {
            left = pop();
            
            ana_array_push(result, left);

            left->refcount++;
          }

          GC_TRACK(vm, result);

          push(result);
          
          vm_continue();
        }
        vm_target(INITOBJ) {
          int arg = get_arg();
          TRACE(INITOBJ, arg, 0, 1);
          ana_object *obj = ana_map_new(arg > 0 ? arg : 2);

          while(arg--)
          {
            ana_object *val = pop();
            ana_object *key = pop();

            assert(ana_type_is(key, ana_string_type));

            ana_map_put(obj, key, val);

            val->refcount++;
          }

          GC_TRACK(vm, obj);

          push(obj);

          vm_continue();
        }
        vm_target(SETPROP) {
          arg = ana_array_get(vm->symbols, oparg);
          TRACE(SETPROP, oparg, 0, 1);
          assert(arg);
          
          ana_object *value = pop();          
          ana_object *instance = pop();

          /* Todo, for all other types, do we want to let properties be 
             defined? But make them readonly? 
            */

          /* map is special in that it will allow aribtrary properties to be defiend */
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
            
            if(!opflag) 
            {              
              push(value);
            }

            goto SETPROP_leave;
          }

          /* prevent arbitrary properties from begin defined on non class types */
          if(!ana_type_is(instance, ana_class_type))
          {
            set_except("RuntimeError", "Can't set property on object of type %s",
              ana_type_name(instance));
          } 
          else 
          {
            ana_class *theclass = (ana_class*)instance;
            ana_object *res = ana_map_put(theclass->members, arg, value);

            if(!res)
              assert(res);

            if(!opflag)
              push(res);
          } 
SETPROP_leave:
          vm_continue();
        }
        vm_target(GETPROP) {
          int argvalue = get_arg();
          TRACE(GETPROP, argvalue, 0, 1);
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

            goto leave_GETPROP;
          }
          /* runtime properties, dynamically computed */
          else if(instance->type->obj_get_attr != NULL)
          {
            ana_object *res = instance->type->obj_get_attr(instance, arg);

            if(res)
            {
              GC_TRACK(vm, res);

              push(res);

              goto leave_GETPROP;
            }
            else
            {
                goto again;
            }
          }
          
again:
          if(instance->type->obj_props != NULL)
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
          else if(ana_type_is(instance, ana_class_type))
          {
            ana_object *res = ana_map_get(
              ((ana_class *)instance)->members, arg);

            if(!res)
            {
              set_except("RuntimeError", "%s is not defined on %s instance",
                ((ana_string *)arg)->value, 
                ana_cstring(ana_get_class(instance)->name));
            }
            else
            {
              /* TODO, GC_TRACK */
              push(res);
            }
          }
          else 
          {
            set_except("RuntimeError", "can't get property on object of type %s"
              ,ana_type_name(instance));
          }
leave_GETPROP:
          vm_continue();
        }
        vm_target(IRETURN) {
          TRACE(IRETURN, get_arg(), 0, 0);
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

            if(arg->refcount > 0)
              arg->refcount--;
            
            frame->retval = arg;

            pushto(caller, arg);
          }
                  
          goto exit; 
        }
        vm_target(LOAD_CONST) {
          TRACE(LOAD_CONST, oparg, 0, 1);
          arg = ana_array_get(vm->constants, oparg);
          push(arg);
          vm_continue();
        }
        vm_target(STORE_NAME) 
        {
          TRACE(STORE_NAME, oparg, 0, 1);
          ana_object *thename = ana_array_get(vm->symbols, oparg);
          result = pop();

          {
            ana_object *oldvalue;

            oldvalue = ana_map_get(locals, thename);

            if(oldvalue)
            {
              if(ana_get_base(oldvalue)->refcount > 0)
              {
                
                #ifdef ANA_GC_DEBUG
                printf("decreasing refcount for %p, old %ld\n",
                  (void *)oldvalue, ana_get_base(oldvalue)->refcount);
                #endif
                decref_recursively(ana_get_base(oldvalue));
              }
            }       
          }

          ana_map_put(locals, thename, result);
          
          result->refcount++;

          if(!opflag) 
          {
            push(result);
          }

          vm_continue();
        }
        vm_target(LOAD_NAME) 
        {
          TRACE(LOAD_NAME, oparg, 0, 1);

          ana_object *thename  = ana_array_get(vm->symbols, oparg);
          ana_object *result   = ana_map_get(frame->locals, thename);

          if(!result && frame->self != NULL)
          {
            ana_class *instance = (ana_class *)frame->self;

            result = ana_map_get(instance->members, thename);
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
            set_except("RuntimeError", "undefined variable '%s'", 
              ana_cstring(thename));
          }
          
          vm_continue();
        }
        vm_target(IDIV) {
          TRACE(IDIV, get_arg(), 0, 0);
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
          TRACE(IEQUAL, get_arg(), 0, 1);
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
        vm_target(INEQUAL) 
        {
          TRACE(INEQUAL, oparg, 0, 1);
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
          TRACE(IREM, get_arg(), 0, 1);
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
            set_except("NotImplementedError", "%% opeartion is not implemented"
              "for type %s", ana_type_name(left));
          }

          vm_continue();
        }  
        vm_target(IADD) {
          TRACE(IADD, get_arg(), 0, 1);

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
        vm_target(ILT) 
        {
          TRACE(ILT, oparg, 0, 1);
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
        vm_target(IGT) 
        {
          TRACE(IGT, oparg, 0, 1);
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
        vm_target(ILTE) 
        {
          TRACE(ILTE, get_arg(), 0, 1);
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
        vm_target(IGTE)
        {
          TRACE(IGTE, get_arg(), 0, 1);
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
        vm_target(ITIMES) 
        {
          TRACE(ITIMES, get_arg(), 0, 1);
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
          TRACE(IMINUS, get_arg(), 0, 1);
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
          TRACE(DEFINE_CLASS, oparg, 0, 1);
          ana_object *name = pop();

          printf("defining class %s\n", ana_cstring(name));

          ana_object *fncount = pop();
          ana_class *theclass = (ana_class *)ana_class_new(NULL, name);
          long i = ana_get_long(fncount)->value;

          while(--i)
          {
            ana_object *function_name = pop();
            ana_object *function_code = pop();

            ana_map_put(theclass->members, function_name, function_code);
          }

          ana_map_put(frame->locals, name, (ana_object *)theclass);

          ana_get_base(theclass)->refcount++;

          GC_TRACK(vm, theclass);

          vm_continue();
        }
        vm_target(DEFINE_FUNCTION) {
          int arg = get_arg();
          TRACE(DEFINE_FUNCTION, arg, 0, 1);        
          ana_object *name = pop();
          ana_object *code = pop();
          ana_map_put(frame->locals, name, code);
          vm_continue();
        }
        vm_target(CALL_METHOD) {
          /* TODO, this needs to be refactorted to account for language defined
             methods too
           */
          TRACE(CALL_METHOD, oparg, 0, 1);
          ana_object *res = NULL;
          ana_object *callable = pop();
          ana_object *self = pop();
          
          int totalargs = oparg;

          assert(ana_type_is(callable, ana_function_type) 
            && ana_get_function(callable)->flags && COMO_FUNCTION_METHOD);

          if(ana_array_size(ana_get_function(callable)->method.m_parameters) 
              != (ana_size_t)totalargs)
          {
             set_except("ArgumentError", "%s expects %ld argument(s), but %d were passed",
              ana_cstring(ana_get_function(callable)->name), 
              ana_array_size(ana_get_function(callable)->method.m_parameters),
              totalargs);      

            goto CALL_METHOD_leave;
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
            if(res != ana_get_array(nativeargs)->items[0]) 
            {
              GC_TRACK(vm, res);
            }
          }
          else
          {
            GC_TRACK(vm, res);
          }

          push(res);

          ana_object_dtor(nativeargs);

CALL_METHOD_leave:
          vm_continue();
        }

        vm_target(CALL) {
          TRACE(CALL, oparg, 0, 1);

          ana_object *res = NULL;
          ana_object *callable = pop();
          ana_object *globals = BASE_FRAME->locals;

          if(ana_type_is(callable, ana_function_type)) 
          {
            int totalargs = oparg;
            ana_function *fn = ana_get_function(callable);

            if(fn->flags & COMO_FUNCTION_LANG) 
            {         
              struct _ana_function_def *call = fn->func;

              ana_frame *execframe = ana_frame_new(
                call->code,
                call->jump_targets, 
                call->line_mapping, 
                globals,
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

              while(totalargs--)
              {
                ana_object *theargvalue = pop();

                theargvalue->refcount++;

                ana_object *paramname = ana_array_get(
                  call->parameters, (ana_size_t)totalargs);

                ana_map_put(execframe->locals, paramname, theargvalue);
              }
              
              COMO_VM_PUSH_FRAME(frame);
              COMO_VM_PUSH_FRAME(execframe);

              goto enter;
            }
            else
            {
              ana_object *nativeargs = ana_array_new(4);

              while(totalargs--)
              {
                ana_object *thearg = pop();

                ana_array_push(nativeargs, thearg);
              }       

              ana_object *res = fn->handler(nativeargs); 

              if(res) 
              {
                GC_TRACK(vm, res);
              }

              ana_object_dtor(nativeargs);
            }
          }
          else if(ana_type_is(callable, ana_class_type))
          {
            #include "class.h"
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

                if(temp->refcount > 0) 
                {
                  decref_recursively(temp);
                }
              }

              ana_map_foreach(originalframe->locals, key, value) 
              {
                (void)key;

                if(value->refcount > 0) 
                {
                  if(value != frame->retval) 
                  {
                    /* the return value is always going to need to be kept around */
                    /* the caller can worry about it*/

                    decref_recursively(value);
                  }
                }
              } ana_map_foreach_end();


              /* finalize this frame, as we won't be returning*/
              ana_object_finalize(originalframe);
              ana_object_dtor(originalframe);

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

        fprintf(stdout, "%s: %s in %s:%d\n", 
          the_type, 
          the_message, 
          ana_cstring(frame->filename), 
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

        if(temp->refcount > 0) 
        {
          decref_recursively(temp);
        }
      }

      ana_map_foreach(frame->locals, key, value) 
      {
        (void)key;

        if(value->refcount > 0) 
        {
          if(value != frame->retval) 
          {
            /* the return value is always going to need to be kept around */
            /* the caller can worry about it*/

            decref_recursively(value);
          }
        }
      } ana_map_foreach_end();
      
#ifdef ANA_DUMP_LOCALS_AFTER_FRAME_EXIT
      ana_object_print(frame->locals);
      fputc('\n', stdout);
#endif

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

    next:
      fm = fm->caller;
  }
}

static void decref_recursively(ana_object *obj)
{

  if(obj->refcount == 0)
    return;

  obj->refcount--;

  if(ana_type_is(obj, ana_array_type))
  {
    ana_array *array = ana_get_array(obj);

    ana_size_t i;

    for(i = 0; i < array->size; i++)
    {
      decref_recursively(array->items[i]);
    }
  }
  else if(ana_type_is(obj, ana_map_type))
  {
    ana_map_foreach(obj, key, value) {
      
      (void)key;

      decref_recursively(value);

    } ana_map_foreach_end();
  }
  else if(ana_type_is(obj, ana_array_type))
  {
    ana_array_foreach(obj, index, value) {

      (void)index;

      decref_recursively(value);

    } ana_array_foreach_end();
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

      if(unreached->refcount == 0) 
      {
        *root = unreached->next;

        ana_object_dtor(unreached);

        vm->nobjs--;
      }
      else
      {
        #ifdef ANA_GC_DEBUG
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
  ana_function_defn *func = ANA_GET_FUNCTION_DEF(functionobj);

  if(function && (vm->flags & COMO_VM_TRACING)) 
  {
    return trace_function(vm, function);
  }

  ana_frame *firstframe = ana_frame_new(
    func->code,
    func->jump_targets, 
    func->line_mapping, 
    NULL,
    functionobj->name, 
    NULL,
    0,
    functionobj->filename
  );

  BASE_FRAME = firstframe;

  COMO_VM_PUSH_FRAME(firstframe);

  ana_frame_eval(vm);

  gc(vm);

  ana_object_dtor(functionobj);

  return 0;
}

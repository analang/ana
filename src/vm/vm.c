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

#include "vmmacros.h"

static void trace_frame(ComoVM *vm, ana_frame *frame);
static void ana_print_backtrace(ana_frame *frame);
static void gc(ComoVM *vm);

static ComoVM *vm;
static ana_frame *BASE_FRAME;
static ana_frame *CURRENT_FRAME = NULL;

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
  (vm->stack[vm->stackpointer++] = frame); \
} while(0)

#define COMO_VM_POP_FRAME() \
  vm->stack[--vm->stackpointer]

#define COMO_VM_HAS_FRAMES() (vm->stackpointer == 0)

#define ANA_KEY_NOT_FOUND ((void*)-1)

ComoVM *ana_vm_new()
{  
  ComoVM *vm = malloc(sizeof(ComoVM));
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
  vm->symbols = ana_array_new(8);

  return vm;
}

void ana_vm_finalize(ComoVM *vm)
{
  ana_array_foreach_apply(vm->symbols, ana_object_dtor);

  ana_object_dtor(vm->symbols);

  free(vm);
}

#include "object_ops.h"

static inline void trace_frame(ComoVM *vm, ana_frame *frame)
{
  fprintf(stdout, "Ana Virtual Machine Disassembler\n");
  fprintf(stdout, "  Frame: %s\n", ana_cstring(frame->name));
  fprintf(stdout, "  Constant Table: \n");
  fprintf(stdout, "   %-15s%-13s%-11s\n", "Index", "Value", "Type"); 

  ana_size_t i;

  for(i = 0; i < ana_array_size(frame->constants); i++)
  {
    ana_object *value = ana_array_get(frame->constants, i);
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
  
  ana_array_foreach(frame->jmptargets, i, value) {
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

static ana_object *transform_module_name(char *name)
{
  ana_object *str = ana_stringfromstring(name);
  name = ana_cstring(str);
  char *begin = name;

  while(*name)
  {
    char *c = &(*name++);

    if(*c == '.')
      *c = '/';
  }

  ana_cstring(str) = begin;

  return str;
}

static ana_object *do_import(ana_frame *frame, ana_object *modulename,
  ana_object *alias)
{
  #ifdef ANA_MODULE_DEBUG
  printf("do_import: request to load %s\n", ana_cstring(modulename));
  #endif

  ana_parser_state state;
  FILE *fp = NULL;
  char *modulepath;
  ana_object *code = NULL;
  int retval;
  ana_object *modulename_fs = transform_module_name(ana_cstring(modulename));
  modulepath = realpath(ana_cstring(modulename_fs), NULL);

  /* Only needed for path resolution */
  ana_object_dtor(modulename_fs);

  if(!modulepath)
  {
    set_except("ImportError","module %s was not found", ana_cstring(modulename));
    return NULL;
  }

  fp = ana_open_file_for_parsing(modulepath);

  if(!fp)
  {
    set_except("ImportError", "failed to load module %s", ana_cstring(
      modulename));

    goto done;
  }

  state.arena = ana_arena_new();
  
  retval = ana_astfromfile(
    fp, modulepath, 0, NULL, &state);

  fclose(fp);
  
  if(retval != 0)
  {
    set_except("ImportError", "failed to parse module %s", 
      ana_cstring(modulename));
    ana_arena_free(state.arena);
    goto done;
  }

  code = ana_compilemodule(ana_cstring(modulename), 
    modulepath, vm, state.ast);

  GC_TRACK(vm, code);

  ana_arena_free(state.arena);

  if(!alias)
    ana_map_put(frame->locals, modulename, code);
  else
    ana_map_put(frame->locals, alias, code);

done:
  free(modulepath);

  return code;
}


static ana_object *do_literal_import(ana_frame *frame, ana_object *modulename)
{
  ana_parser_state state;
  FILE *fp = NULL;
  char *modulepath;
  ana_object *code = NULL;
  int retval;

  ana_object *modulename_fs = transform_module_name(ana_cstring(modulename));

  modulepath = realpath(ana_cstring(modulename_fs), NULL);
  
  ana_object_dtor(modulename_fs);

  if(!modulepath)
  {
    set_except("ImportError","module %s was not found", ana_cstring(modulename));
    return NULL;
  }

  fp = ana_open_file_for_parsing(modulepath);

  if(!fp)
  {
    set_except("ImportError", "failed to load module %s", ana_cstring(
      modulename));
    goto done;
  }

  state.arena = ana_arena_new();
  
  retval = ana_astfromfile(
    fp, modulepath, 0, NULL, &state);

  fclose(fp);
  
  if(retval != 0)
  {
    set_except("ImportError", "failed to parse module %s", 
      ana_cstring(modulename));
    ana_arena_free(state.arena);
    goto done;
  }

  code = ana_compilemodule(ana_cstring(modulename), modulepath, vm, state.ast);

  GC_TRACK(vm, code);

  ana_arena_free(state.arena);

  ana_size_t module_symbol_index = make_symbol(vm, ana_cstring(modulename));
  ana_object *module_symbol = 
    ana_get_array(vm->symbols)->items[module_symbol_index];

  ana_map_put(frame->locals, module_symbol, code);

done:
  free(modulepath); 

  return code;
}

static ana_object *ana_frame_eval(ComoVM *vm)
{
  ana_frame *frame;
  ana_uint32_t **code;
  ana_long **jmptargets;
  ana_object **constants;
  ana_object *locals;

  ana_object *arg = NULL, *retval = NULL, *left, *right, *result;

  while(!COMO_VM_HAS_FRAMES())
  {
    uint64_t ticks = 0;
    
    enter:

    frame = COMO_VM_POP_FRAME();
    CURRENT_FRAME = frame;
    code = (ana_uint32_t **)(((ana_container *)frame->code)->data);
    jmptargets = (ana_long **)(((ana_array *)frame->jmptargets)->items);
    constants = (ana_object **)(((ana_array *)frame->constants)->items);
    locals = frame->locals;


    if(vm->flags & COMO_VM_TRACING_ANY)
      trace_frame(vm, frame);

    if(!frame->ready)
      ana_object_init(frame);

    ana_uint32_t opline;
    ana_uint32_t opcode;
    short oparg;

    for(;;) {
      top:

      if(vm->flags & COMO_VM_TRACING_ANY)
        fprintf(stderr,"vm[%lu] pc at # %ld\n", ticks++, frame->pc);

      fetch();

      vm_case(opcode) {
        default:
          set_except("VMError", "Opcode %d is not implemented\n", opcode);
          vm_continue();
        vm_target(IIN) {
          TRACE(IIN, oparg, 0, 1);
          ana_object *container = pop();
          ana_object *index = pop();
          ana_object *res = getindex(container, index);

          if(res == ANA_KEY_NOT_FOUND)
            push(ana_bool_false);
          else
            push(ana_bool_true);

          vm_continue();
        }

        vm_target(IIMPORTAS) {
          result = pop();
          // result stores the filename
          TRACE(IIMPORTAS, oparg, 0, 1);
          // the alias
          ana_object *alias = ana_array_get(vm->symbols, oparg);
          ana_frame *importedframe = (ana_frame *)do_import(frame, result, 
            alias);
          
          if(importedframe != NULL) {
            ana_frame *call = importedframe;

            ana_frame *execframe = 
              ana_frame_base_new(
                call->name,
                call->filename,
                (ana_object *)frame,
                call->code,
                call->constants,
                call->jmptargets,
                call->linemapping,
                call->bpsize
              );

            GC_TRACK(vm, (ana_object *)execframe);

            if(!execframe->ready) {
              ((ana_object *)execframe)->type->obj_init((ana_object *)execframe);
            }

            execframe->parent = (ana_object *)frame;
            execframe->params  = NULL;
            execframe->globals = BASE_FRAME->locals;
            execframe->self = call->self;
            execframe->calling_module = frame->filename;

            ana_object *pc = ana_longfromlong((long)frame->pc);
            ana_object *theline = ana_map_get(frame->linemapping, pc);
            int lineno = 0;
            if(theline)
              lineno = (int)ana_get_long(theline)->value;

            ana_object_dtor(pc);

            execframe->lineno = lineno;
            // Push frame to execute
            COMO_VM_PUSH_FRAME(importedframe);
            // save calling frame
            COMO_VM_PUSH_FRAME(frame);
            goto enter;
          }
          // exception may have happened, do_literal_import set's the ex 
          // variable         
          vm_continue();
        }
        vm_target(IIMPORT) {
          TRACE(IIMPORT, oparg, 0, 1);
          ana_object *thename = ana_array_get(vm->symbols, oparg);
          ana_frame *importedframe = (ana_frame *)do_literal_import(frame, thename);
          
          if(importedframe != NULL) 
          {
            ana_frame *call = importedframe;

            ana_frame *execframe = 
              ana_frame_base_new(
                call->name,
                call->filename,
                (ana_object *)frame,
                call->code,
                call->constants,
                call->jmptargets,
                call->linemapping,
                call->bpsize
              );

            GC_TRACK(vm, (ana_object *)execframe);

            if(!execframe->ready) {
              ((ana_object *)execframe)->type->obj_init((ana_object *)execframe);
            }

            execframe->parent = (ana_object *)frame;
            execframe->params  = NULL;
            execframe->globals = BASE_FRAME->locals;
            execframe->self = call->self;
            execframe->calling_module = frame->filename;

            ana_object *pc = ana_longfromlong((long)frame->pc);
            ana_object *theline = ana_map_get(frame->linemapping, pc);
            int lineno = 0;
            if(theline)
              lineno = (int)ana_get_long(theline)->value;

            ana_object_dtor(pc);

            execframe->lineno = lineno;
            // Push frame to execute
            COMO_VM_PUSH_FRAME(importedframe);
            // save calling frame
            COMO_VM_PUSH_FRAME(frame);
            goto enter;
          }
          // exception may have happened, do_literal_import set's the ex 
          // variable
          vm_continue();
        }
        vm_target(IUNARYMINUS) {
          TRACE(IUNARYMINUS, 0, 0, 1);
          arg = pop();

          if(arg->type->obj_unops != NULL 
            && arg->type->obj_unops->obj_minus != NULL)
          {
            result = arg->type->obj_unops->obj_minus(arg);

            if(result) {
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

          if(!result->type->obj_bool(result))
          {            
            frame->pc = (*(jmptargets + oparg))->value;
            goto top;
          }

          done:
          vm_continue();
        }
        vm_target(TRY) {
          int arg = get_arg();
          /* Arg is the index into the jmptargets table */
          TRACE(TRY, arg, 0, 1);
          ana_object *jmptargetobj = ana_array_get(frame->jmptargets,
            (ana_size_t)arg);

          ana_basic_block *tryblock = 
            ana_basic_block_new((int)ana_get_long(jmptargetobj)->value);

          tryblock->next = frame->blockroot;
          frame->blockroot = tryblock;

          frame->blockstack[frame->bp++] = tryblock;
          frame->bpsize++;

          vm_continue();
        }
        vm_target(SETUP_CATCH) {
          int cindex = get_arg();
          TRACE(SETUP_CATCH, cindex, 0, 1);
          assert(ex != NULL);
          arg = get_const(cindex);
          assert(arg != NULL);
          ana_object *exvalue = ana_stringfromstring(ex);
          GC_TRACK(vm, exvalue);
          ana_map_put(frame->locals, arg, exvalue);
          free(ex);
          ex = NULL;
          ex_type = NULL;
          vm_continue();
        }
        vm_target(LOAD_SUBSCRIPT) {
          TRACE(LOAD_SUBSCRIPT, 0, 0, 0);
          ana_object *index = pop();
          ana_object *container = pop();
          ana_object *res = getindex(container, index);

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
          TRACE(STORE_SUBSCRIPT, 0, 0, 0);
          ana_object *value = pop();
          ana_object *index = pop();
          ana_object *container = pop();
          ana_object *res = setindex(container, index, value);   
          
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
            push(res);
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
          ana_class *theclass = (ana_class*)instance;

          assert(value);
          assert(instance);

          if(!ana_type_is(instance, ana_class_type))
          {
            set_except("RuntimeError", "Can't set property on object of type %s",
              ana_type_name(instance));
          } 
          else 
          {
            ana_object *res = ana_map_put(theclass->members, arg, value);

            if(!res)
              assert(res);

            push(res);
          } 

          vm_continue();
        }
        vm_target(GETPROP) {
          int argvalue = get_arg();
          TRACE(GETPROP, argvalue, 0, 1);
          ana_object *instance = pop();
          arg = ana_get_array(vm->symbols)->items[argvalue];

          /* this is a module get */
          if(ana_type_is(instance, ana_frame_type))
          {
            ana_object *res = ana_map_get(
              ana_get_frame(instance)->locals, arg);

            if(!res)
            {
              set_except("RuntimeError", "module '%s' has no property '%s'",
                ana_cstring(ana_get_frame(instance)->name), 
                ana_cstring(arg)
              ); 
            }
            else
            {
              push(res);
            }             
          }
          else if(!ana_type_is(instance, ana_class_type))
          {
            set_except("RuntimeError", "can't get property on object of type %s"
              ,ana_type_name(instance));
          }
          else 
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
              push(res);
            }
          }
          vm_continue();
        }
        vm_target(IRETURN) {
          TRACE(IRETURN, get_arg(), 0, 0);
          /* this is for early return value binding, for class constructors instances */
          if(frame->retval)
            arg = frame->retval;
          else
            arg = pop();

          assert(arg);

          if(!COMO_VM_HAS_FRAMES())
          {
            ana_frame *caller = vm->stack[vm->stackpointer - 1];
            pushto(caller, arg);
          }
          goto exit; 
        }
        vm_target(LOAD_CONST) {
          TRACE(LOAD_CONST, oparg, 0, 1);
          arg = get_const(oparg);
          push(arg);
          vm_continue();
        }
        vm_target(STORE_NAME) {
          TRACE(STORE_NAME, oparg, 0, 1);
          ana_object *thename = ana_array_get(vm->symbols, oparg);
          result = pop();
          ana_map_put(locals, thename, result);
          push(result);
          vm_continue();
        }
        vm_target(LOAD_NAME) {
          TRACE(LOAD_NAME, oparg, 0, 1);
          ana_object *thename  = ana_array_get(vm->symbols, oparg);

          ana_object *result = ana_map_get(frame->locals, thename);

          if(!result && frame->self != NULL)
          {
            ana_class *instance = (ana_class *)frame->self;

            result = ana_map_get(instance->members, thename);
          }

          if(!result)
          {
            if(frame->globals) {
              result = ana_map_get(frame->globals, thename);
            }
          }

          if(result) {
            push(result);
          }
          else {
            set_except("RuntimeError", "undefined variable '%s'", 
              ana_cstring(thename));
          }
          
          vm_continue();
        }
        vm_target(IDIV) {
          TRACE(IDIV, get_arg(), 0, 0);
          right = pop();
          left  = pop();

          result = do_div(left, right);

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
          }

          if(result)
          {
            push(result);
          }
          else
          {
            set_except("NotImplementedError", "== operation is not implemented"
              " for type %s", ana_type_name(left));
          }

          vm_continue();
        }
        vm_target(INEQUAL) {
          TRACE(INEQUAL, get_arg(), 0, 1);
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
            set_except("NotImplementedError", "!= opeartion is not implemented"
              "for type %s", ana_type_name(left));
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

        vm_target(ILTE) {
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
          }
          else if(left->type->obj_compops != NULL 
              && left->type->obj_compops->obj_lte != NULL) 
          {
            /* this is just a boolean which only exists in a singleton */
            /* do not add to GC */
            result = left->type->obj_compops->obj_lte(left, right);
          }

          if(result) {
            /* bool objects aren't GC'd, there are only 2 instances throught 
               the entire run 
             */
            push(result);
          }
          else
            set_except("RuntimeError", "unsupported operands for <= operator");

          vm_continue(); 
        }
        vm_target(ITIMES) {
          TRACE(ITIMES, get_arg(), 0, 1);
          right = pop();
          left  = pop();

          result = mul(left, right);

          if(result) {
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

          result = sub(left, right);

          if(result) {
            push(result);
          }
          else
            set_except("RuntimeError", "unsupported operands for - operator");

          vm_continue();
        }
        vm_target(CALL) {
          int arg = get_arg();
          TRACE(CALL, arg, 0, 1);
          ana_object *res = NULL;
          ana_object *callable = pop();

          /* increase the ref count here, XXX not sure, jumping somehow
             gets fucked up, if not. not sure how this works
          */
          //callable->flags++;

          if(ana_type_is(callable, ana_function_type)) 
          {
            int totalargs = arg;
            ana_function *fn = ana_get_function(callable);

            if(fn->flags & COMO_FUNCTION_LANG) 
            {         
              ana_frame *call = fn->impl.frame;

              ana_frame *execframe = 
                ana_frame_base_new(
                  call->name,
                  call->filename,
                  (ana_object *)frame,
                  call->code,
                  call->constants,
                  call->jmptargets,
                  call->linemapping,
                  call->bpsize
                );
              
              GC_TRACK(vm, (ana_object *)execframe);

              if(!execframe->ready) {
                ((ana_object *)execframe)->type->obj_init((ana_object *)execframe);
              }

              execframe->parent = (ana_object *)frame;
              execframe->params  = call->params;
              execframe->globals = BASE_FRAME->locals;
              execframe->self = call->self;
              execframe->calling_module = frame->filename;

              if(execframe->params != NULL)
              {
                if(ana_get_array(execframe->params)->size != totalargs)
                {
                  set_except(
                    "TypeError", "%s expects %lu arguments, %d given",
                    ana_get_fn_name(execframe), 
                    ana_get_array(execframe->params)->size,
                    totalargs);
                  
                  goto call_exit;
                }
              }
              else if(totalargs != 0)
              {
                set_except(
                  "TypeError", "%s expects 0 arguments, %d given",
                  ana_get_fn_name(execframe), 
                  0,
                  totalargs);

                goto call_exit;
              }

              while(totalargs--)
              {
                ana_object *theargvalue = pop();

                ana_object *paramname = ana_array_get(
                  execframe->params, (ana_size_t)totalargs);

                ana_map_put(execframe->locals, paramname, theargvalue);
              }
              
              COMO_VM_PUSH_FRAME(frame);      
              COMO_VM_PUSH_FRAME(execframe);
              
              ana_object *pc = ana_longfromlong((long)frame->pc);
              ana_object *theline = ana_map_get(frame->linemapping, pc);
              int lineno = 0;
              if(theline)
                lineno = (int)ana_get_long(theline)->value;

              ana_object_dtor(pc);

              execframe->lineno = lineno;

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

              ana_object *res = fn->impl.handler(nativeargs); 

              if(res) 
              {
                GC_TRACK(vm, res);
              }

              ana_object_dtor(nativeargs);
            }
          }
          /* Call the constructor, set up the instance */
          else if(ana_type_is(callable, ana_class_type))
          {            
            int totalargs = arg;

            ana_object_ctor(callable);

            ana_class *theclass = (ana_class *)callable;
            
            ana_object *self = ana_stringfromstring("self");
            GC_TRACK(vm, self);
            ana_map_put(theclass->members, self, callable);
            
            ana_object *__name__ = ana_stringfromstring("__name__");
            GC_TRACK(vm, __name__);
            ana_map_put(theclass->members, __name__, theclass->name);

            ana_object *constructor = ana_map_get(theclass->members, 
              theclass->name);

            if(constructor)
            {
              ana_frame *realconstructor = 
                ((ana_function *)constructor)->impl.frame;
              ana_frame *execframe;

                {
                  execframe = 
                    ana_frame_base_new(
                      realconstructor->name,
                      realconstructor->filename,
                      (ana_object *)frame,
                      realconstructor->code,
                      realconstructor->constants,
                      realconstructor->jmptargets,
                      realconstructor->linemapping,
                      realconstructor->bpsize
                    ); 

                  execframe->self = realconstructor->self;
                  execframe->params = realconstructor->params;
                  
                  if(!execframe->ready)
                    ((ana_object *)execframe)
                       ->type->obj_init((ana_object *)execframe);
                  
                  if(execframe->params != NULL)
                  {
                    if(ana_get_array(execframe->params)->size != totalargs)
                    {
                      set_except(
                        "TypeError", "%s expects %lu arguments, %d given",
                        ana_get_fn_name(execframe), 
                        ana_get_array(execframe->params)->size,
                        totalargs);

                      goto call_exit;
                    }
                  } 

                  while(totalargs--)
                  {
                    ana_object *theargvalue = pop();

                    ana_object *paramname = ana_array_get(
                      execframe->params, (ana_size_t)totalargs);

                    ana_map_put(execframe->locals, paramname, theargvalue);
                  }

                  GC_TRACK(vm, (ana_object *)execframe);
                }

              assert(execframe->self != NULL);
              assert(execframe->self == callable);

              execframe->parent  = (ana_object *)frame;
              /* only if frame == main */
              execframe->globals = frame->locals;

              /* Save calling frame */
              COMO_VM_PUSH_FRAME(frame);
              /* Push frame to execute */
              COMO_VM_PUSH_FRAME(execframe);

              /* Get the line this frame was activated on */
              ana_object *pc = ana_longfromlong((long)frame->pc);
              ana_object *theline = ana_map_get(frame->linemapping, pc);
              int lineno = 0;
              if(theline)
                lineno = (int)ana_get_long(theline)->value;

              ana_object_dtor(pc);

              execframe->lineno = lineno;

              execframe->retval = callable;

              goto enter;
            }

            res = callable;
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

      if(ex) 
      {
        ana_frame *thisframe = frame;
        ana_frame *originalframe = frame;

        while(thisframe != NULL) 
        {

          ana_basic_block *block;
          while(!(thisframe->bp == 0))
          {
            block = thisframe->blockstack[--thisframe->bp];

            thisframe->pc = (ana_size_t)block->jmpto;
            
            /* do finalization here */
            
            /* frame that threw the exception */
            /* code won't continue from there */
            //COMO_VM_PUSH_FRAME(originalframe);

            if(originalframe != thisframe)
            {
              /* finalize this frame, as we won't be returning*/
              ana_object_finalize(originalframe);
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

        int currentpc = frame->pc;
        int line = 0;
        ana_object *pc = ana_longfromlong((long)currentpc);
        /* TODO 
            abstract the fetch, to automatically update the current line number
            */
        ana_object *theline = ana_map_get(frame->linemapping, 
          pc);

        if(theline)
        {
          line = ana_get_long(theline)->value;
        }

        fprintf(stdout, "%s: %s in %s:%d\n", 
          ex_type, 
          ex, 
          ana_cstring(frame->filename), 
          line
        );

        free(ex);

        ana_print_backtrace(frame);
      
        exit(1);
      }
    }
    exit:
      while(!empty()) {
        (void)pop();
      }

      ana_object_finalize(frame);
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
      fm->lineno
    );

    next:
      fm = (ana_frame *)fm->parent;
  }
}

static void mark(ComoVM *vm)
{
  if(vm->flags & COMO_VM_GC_DISABLED)
    return;

  ana_size_t i, j;

  /* if stackPointer is 0, then this is the main frame */

  if(vm->stackpointer == 0)
  {
    #ifdef ANA_GC_DEBUG
    printf("mark: stackPointer was 0, this means we've popped the last frame to execute\n");
    #endif

    if(CURRENT_FRAME) {
      for(j = 0; j < CURRENT_FRAME->sp; j++)
      {
        ana_object *obj = CURRENT_FRAME->stack[j];

#ifdef ANA_GC_DEBUG 
        printf("marking object %p as reachable\n", (void *)obj)
#endif
        obj->flags = 1;
      }   
    }
  }
  else 
  {
    for(i = 0; i < vm->stackpointer; i++)
    {
      ana_frame *frame = vm->stack[i];

#ifdef ANA_GC_DEBUG
      printf("mark: doing mark for frame %s\n", ana_cstring(frame->name));
#endif

      ana_get_base(frame)->flags = 1;

      for(j = 0; j < frame->sp; j++)
      {
        ana_object *obj = frame->stack[j];

        // TODO check if this is a function....
        // and mark it's stack

#ifdef ANA_GC_DEBUG
        printf("marking object %p as reachable\n", (void *)obj);
#endif
        obj->flags = 1;
      }
    }
  }
}

static void sweep(ComoVM *vm)
{
  if(vm->flags & COMO_VM_GC_DISABLED)
    return;

  ana_object** root = &vm->root;
  int i = 0;
  int n = vm->nobjs;

  while (*root && i < n) 
  {
    i++;
    
    if(!(*root)->flags) 
    {
      ana_object* unreached = *root; 
            
      if(!ana_type_is(unreached, ana_bool_type)) 
      {

        if(ana_type_is(unreached, ana_function_type)) 
        {
          if(ana_get_function_flags(unreached) & COMO_FUNCTION_LANG)
          {
            if(ana_get_function_frame(unreached) == CURRENT_FRAME)
            {
              #ifdef ANA_GC_DEBUG
              fprintf(stdout, "gc.sweep: attempt to free current executing frame (1)\n");
              #endif

              continue;
            }
            else
            {
              if(CURRENT_FRAME != NULL)
              {
                /* don't free function definitions, they are constant */
                continue;
              }
            }
          }
        }
        if(ana_type_is(unreached, ana_frame_type))
        {
          if(ana_get_frame(unreached) == CURRENT_FRAME)
          {
            #ifdef ANA_GC_DEBUG
            fprintf(stdout, "gc.sweep: attempt to free current executing frame (2)\n");
            #endif

            continue; 
          }
          if(CURRENT_FRAME != NULL) 
          {
           /* don't free function definitions, they are constant */
            continue;
          }
        }
        if(ana_type_is(unreached, ana_class_type))
        {
          //if(ana_get_class_frame(unreached) == CURRENT_FRAME)
          //continue;
        }
        
        int x;
        int found = 0;

        if(CURRENT_FRAME) 
        {
          for(x = 0; x < CURRENT_FRAME->sp; x++)
          {
            if(CURRENT_FRAME->stack[x] == unreached)
            {
              #ifdef ANA_GC_DEBUG
              printf("sweep: attempt to destroy object that is reachable on current frames the stack\n");
              #endif
              found = 1;
              break;
            }
          }
        }

        if(!found)
        {

#ifdef ANA_GC_DEBUG
          printf("gc.sweep: free object %p, type=%s\n", (void *)unreached,
            ana_type_name(unreached));
#endif

          *root = unreached->next;
          ana_object_dtor(unreached);
          vm->nobjs--;
        }
      }

    } 
    else
    {
      ana_object *obj = *root;
      int found = 0;
      int x;

      if(CURRENT_FRAME)
      {
        for(x = 0; x < CURRENT_FRAME->sp; x++)
        {
          if(CURRENT_FRAME->stack[x] == obj)
          {
            found = 1;
            break;
          }
        }
      }

      if(found == 0)
        (*root)->flags = 0;

      root = &(*root)->next;
    }
  }
}

static void gc(ComoVM *vm)
{
  printf("gc.noobjs=%ld\n", vm->nobjs);

  if(vm->flags & COMO_VM_GC_DISABLED)
    return;
  
  mark(vm);
  sweep(vm);
}

int ana_eval(ComoVM *_vm, ana_object *code, char *function)
{
  vm = _vm;

  ana_frame *frame = (ana_frame *)code;

  BASE_FRAME = frame;

  if(function)
  {
    ana_object *fname = ana_stringfromstring(function);
    ana_object *fn = ana_map_get(frame->locals, fname);
    
    if(!fn)
    {
      fprintf(stderr, "ana: function %s does not exist in module\n", function);
      exit(1);
    }

    frame = ana_get_function(fn)->impl.frame;

    ana_object_dtor(fname);
  }

  COMO_VM_PUSH_FRAME(frame);

  (void)ana_frame_eval(vm);

  CURRENT_FRAME = NULL;

  gc(vm);
  gc(vm);

  ana_object_dtor(code);

  ana_bool_type_finalize();

  return 0;
}
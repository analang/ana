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

#define GC_TRACK(frame, obj) do { \
  ((ana_object *)obj)->next = frame->root; \
  ((ana_object *)obj)->flags = 0; \
  frame->root = obj; \
  frame->nobjs++; \
  if(!ana_get_base(obj)->scope) { \
    char *scope = ana_get_fn_name(frame); \
    ((ana_object *)obj)->scope \
      = ana_stringfromstring(scope); \
    free(scope); \
  } \
  if (frame->nobjs >= frame->mxobjs) { \
    do_gc(NULL, frame); \
  } \
} while(0)


static void trace_frame(ComoVM *vm, ana_frame *frame);
static void ana_print_backtrace(ana_frame *frame);

static ComoVM *vm;
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

#define EXCEPT (ex != NULL)

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

/* This is a quick hack to implement dynamic container value lookups,
   Currently in the container APIs, NULL indicates a value was not found
   however, there needs to be a way for the VM to catch this since
   NULL can also indicate an implementation for the container type wasn't
   available
*/
#define ANA_KEY_NOT_FOUND ((void*)-1)

ComoVM *ana_vm_new()
{  
  ana__runtime_object_cache_init();

  ComoVM *vm = malloc(sizeof(ComoVM));
  ana_size_t i;

  for(i = 0; i < COMO_VM_STACK_MAX; i++)
    vm->stack[i] = NULL;

  vm->stacksize = 0;
  vm->stackpointer = 0;
  vm->exception = NULL;
  vm->flags = 0;
  vm->root = NULL;
  vm->symbols = ana_array_new(8);

  return vm;
}

void ana_vm_finalize(ComoVM *vm)
{
  ana_array_foreach_apply(vm->symbols, ana_object_dtor);

  ana_object_dtor(vm->symbols);

  free(vm);

  ana__runtime_object_cache_finalize();
}

static void real_gc(ComoVM *vm, ana_frame *frame);
static void do_mark_stack(ana_frame *frame);
static void do_sweep(ana_frame *frame);

static inline void do_mark_object(ana_object *obj)
{
  if(obj->flags)
    return;

  obj->flags = 1;

  if(ana_type_is(obj, ana_function_type))
  {
    ana_function *fn = ana_get_function(obj);

    if(fn->flags & COMO_FUNCTION_LANG)
    {
      if(fn->impl.frame->root) 
        do_sweep(fn->impl.frame);
    }
  } 
  else if(ana_type_is(obj, ana_frame_type))
  {
    do_sweep(ana_get_frame(obj));
  }
}

static void do_mark_stack(ana_frame *frame)
{
  ana_size_t i;

  for(i = 0; i < frame->sp; i++)  
  {
    ana_object *root = frame->stack[i];
    if(root->scope && ana_get_base(frame)->scope) 
    {
      if(strcmp(ana_cstring(root->scope),
        ana_cstring(ana_get_base(frame)->scope)) != 0) 
      {
        continue;
      }
    }
    do_mark_object(frame->stack[i]);
  }
}

static inline void ensure_non_circular_root(ana_object *root)
{
  ana_object *visited_list = ana_map_new(16);

  while(root)
  {
    ana_object *key = ana_longfromlong((intptr_t)root);

    if(ana_map_get(visited_list, key))
    {
      fprintf(stdout, "%p has been visited already\n", (void *)root);
      ana_tostring_fast(root, {
        fprintf(stdout, "  %s\n", value);
      });        
      abort();
    }
    ana_map_put(visited_list, 
      ana_longfromlong((intptr_t)root),
      ana_longfromlong((intptr_t)root)
    );

    root = root->next;

    ana_object_dtor(key);
  }

  ana_map_foreach(visited_list, key, value) 
  {
    ana_object_dtor(key);
    ana_object_dtor(value);
  } ana_map_foreach_end();

  ana_object_dtor(visited_list);
}

static int count_objs(ana_frame *frame)
{
  int i = 0;
  ana_object *root = frame->root;
  while(root) {
    i++;
    root = root->next;
  }
  return i;
}

static void ana_gc_do_sweep(ana_frame *frame)
{
  ana_object *root = frame->root;
  ana_object *prev = NULL;
  ana_object *next = NULL;
  int i = 0;
  int objcount = count_objs(frame);

  while(root && i <= objcount)
  {
    i++;
    next = root->next;

    if(root->scope && ana_get_base(frame)->scope) 
    {
      if(strcmp(ana_cstring(root->scope),
        ana_cstring(ana_get_base(frame)->scope)) != 0) 
      {
        goto finish;
      }
    }

    if(!root->flags) 
    {
      ana_object* unreached = root;
      
      if(prev)
        prev->next = root;
      else
        frame->root = root->next;

      if(ana_type_is(unreached, ana_function_type)) 
      {
        ana_function *fn = ana_get_function(unreached);

        if(fn->flags & COMO_FUNCTION_LANG)
        {
          if(!fn->impl.frame->finalized)
            ana_object_finalize(fn->impl.frame);

          do_sweep(fn->impl.frame);
        }
      }
      if(ana_type_is(unreached, ana_frame_type))
      {
        ana_frame *fm = ana_get_frame(unreached);

        if(!fm->finalized)
          ana_object_finalize(fm);

        ana_gc_do_sweep(fm);
      }
      
      if(!ana_type_is(unreached, ana_bool_type)) 
      {
        ana_object_dtor(unreached);
      }

      frame->nobjs--;
    } 
    else 
    {
      root->flags = 0;
    }

    finish:
      prev = root;
      root = next;
  }
}

static void do_sweep(ana_frame *frame)
{

  ana_gc_do_sweep(frame);

  return;

  ensure_non_circular_root(frame->root);

  clock_t start = clock();

  ana_object** root = &frame->root;

  #ifdef ANA_GC_DEBUG
    printf("begin sweep for frame %s\n", ana_get_base(frame)->scope ? 
      ana_cstring(ana_get_base(frame)->scope) :  "__main__");
  #endif

  while (*root) 
  {

      if((*root)->scope == NULL) {
        #ifdef ANA_GC_DEBUG
        printf("found a NULL scope in frame %s\n", 
          ana_get_base(frame)->scope ? ana_cstring(ana_get_base(frame)->scope) 
            : "__main__");
        #endif
      }

      {
        ana_object *theobj = (*root);
        
        if(theobj->scope && ana_get_base(frame)->scope) {
          if(strcmp(ana_cstring(theobj->scope), ana_cstring(ana_get_base(frame)->scope)) != 0) {
            /* don't free it, since we're not GC'ing the objects own definition scope */
            #ifdef ANA_GC_DEBUG
            printf("\tgc: skipping object of type %s defined in frame %s, but we're running GC for frame %s\n", ana_type_name(*root),
              theobj->scope == NULL ? "__main__" : ana_cstring(theobj->scope),
              ana_get_base(frame)->scope == NULL ?
                 "__main__" : ana_cstring(ana_get_base(frame)->scope));
            #endif
            goto skip;
          }
        }
      }

    clock_t elapsed_time = clock() - start;
    int sec = elapsed_time / CLOCKS_PER_SEC;

    if(sec >= COMO_VM_GC_TIMEOUT)
    {
      fprintf(stderr, "do_sweep: timing out after %d seconds\n",
        COMO_VM_GC_TIMEOUT);
      goto done;
    }

    if(!(*root)->flags) 
    {
      ana_object* unreached = *root;
      *root = unreached->next;

      if(ana_type_is(unreached, ana_function_type)) 
      {
        ana_function *fn = ana_get_function(unreached);

        if(fn->flags & COMO_FUNCTION_LANG)
        {
          if(!fn->impl.frame->finalized)
            ana_object_finalize(fn->impl.frame);

          do_sweep(fn->impl.frame);
        }
      }
      if(ana_type_is(unreached, ana_frame_type))
      {
        ana_frame *fm = ana_get_frame(unreached);

        if(!fm->finalized)
          ana_object_finalize(fm);

        do_sweep(fm);
      }
      
      if(!ana_type_is(unreached, ana_bool_type)) 
      {
        ana_object_dtor(unreached);
      }

      frame->nobjs--;
    } 
    else 
    {

skip:
      (*root)->flags = 0;
      root = &(*root)->next;
      continue;
    }
  }


done:
;
  #ifdef ANA_GC_DEBUG
  {
    clock_t end = clock() - start;
    int msec = end * 1000 / CLOCKS_PER_SEC;
    printf("finished in %d seconds %d milliseconds\n", msec/1000, msec%1000);
  }
    printf("end sweep for frame %s\n", ana_get_base(frame)->scope ? 
      ana_cstring(ana_get_base(frame)->scope) : "__main__");

  #endif
}

static inline void real_gc(ComoVM *vm, ana_frame *frame)
{
  COMO_UNUSED(vm);

  ana_size_t nobjs = frame->nobjs;

  do_mark_stack(frame);
  do_sweep(frame);

  frame->mxobjs = frame->nobjs * 2;

  (void)nobjs;
}

void do_gc(ComoVM *vm, ana_frame *frame)
{
  real_gc(vm, frame);
}

static inline ana_object *mul(ana_frame *f, ana_object *a, ana_object *b)
{
  if(a->type->obj_binops != NULL && a->type->obj_binops->obj_mul != NULL) 
  {
    ana_object *res = a->type->obj_binops->obj_mul(a, b);
    if(res) 
    {
      GC_TRACK(f, res);
      return res;
    }
  }
  return NULL;
}

static inline ana_object *sub(ana_frame *f, ana_object *a, ana_object *b)
{
  if(a->type->obj_binops != NULL && a->type->obj_binops->obj_sub != NULL) 
  {
    ana_object *res = a->type->obj_binops->obj_sub(a, b);
    if(res) {
      GC_TRACK(f, res);
      return res;
    }
  }
  return NULL;
}

static inline ana_object *getindex(ana_frame *f, ana_object *container, 
  ana_object *idx)
{
  (void )f;

  if(container->type->obj_seqops != NULL && container->type->obj_seqops->get != NULL)
  {
    ana_object *res = container->type->obj_seqops->get(container, idx);

    if(!res)
      return ANA_KEY_NOT_FOUND;

    /* array values and map values, are already part of the GC root */
    if(!ana_type_is(container, ana_array_type) 
      && !ana_type_is(container, ana_map_type))
        GC_TRACK(f, res);

    return res;
  }

  return NULL; 
}

static inline ana_object *setindex(ana_frame *f, ana_object *container, 
  ana_object *idx, ana_object *val)
{
  (void )f;

  if(container->type->obj_seqops != NULL && container->type->obj_seqops->set != NULL)
  {
    ana_object *res = container->type->obj_seqops->set(container, idx, val);

    if(!res)
      return ANA_KEY_NOT_FOUND;

    /* array values and map values, are already part of the GC root */
    if(!ana_type_is(container, ana_array_type) 
      && !ana_type_is(container, ana_map_type))
        GC_TRACK(f, res);

    return res;
  }

  return NULL; 
}

static inline ana_object *do_div(ana_frame *f, ana_object *a, ana_object *b)
{
  ana_object *retval = NULL;

  if(a->type->obj_binops != NULL && a->type->obj_binops->obj_div != NULL) 
  {
    retval = a->type->obj_binops->obj_div(a, b);
    if(retval)
      GC_TRACK(f, retval);
  }

  return retval;
}

static void __attribute__((unused)) ana_abort(ana_frame *frame)
{
  assert(frame);
  assert(frame->stack);

  ana_size_t i;

  fprintf(stdout, "Ana Stack Inspection\n");
  fprintf(stdout, "    frame %s\n\n", ana_get_fn_name(frame));

  for(i = frame->sp - 1; i >= 0; --i)
  {
    ana_tostring_fast(frame->stack[i], {
      printf("[%ld] => %s (%s)\n", i , value, ana_type_name(frame->stack[i]));
    });
  }
}

static inline void dump_locals(ana_frame *frame)
{

  ana_string *val;
  char *sval;

  fprintf(stdout, "Symbol Table for frame %s\n", ana_cstring(frame->name));
  fprintf(stdout, "   %-15s%c%-13s\n", "Id", ' ', "Value"); 

  ana_map_foreach(frame->locals, key, value) {
    val = (ana_string *)ana_object_tostring(value);
    sval = ana_cstring(val);

    if(val->len > 13)
      sval[12] = '\0';

    fprintf(stdout, "   %-15s%c%-13s\n", ana_cstring(key),' ', sval);
    ana_object_dtor(val);
  } ana_map_foreach_end();

  fprintf(stdout, "End Symbol Table\n");
}

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

//#define ANA_MODULE_DEBUG

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

  modulepath = realpath(ana_cstring(modulename), NULL);

  #ifdef ANA_MODULE_DEBUG
  printf("do_import: resolved module %s to file path %s\n",
    ana_cstring(modulename), modulepath);
  #endif

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

  modulepath = realpath(ana_cstring(modulename), NULL);
  
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

  GC_TRACK(frame, code);

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

  #ifdef ANA_VM_COMPUTED_GOTO
    static void **DISPATCH_TABLE[ANA_LAST_OPCODE] = {
      [IUNARYMINUS] = &&do_IUNARYMINUS,
      [ITHROW] = &&do_ITHROW,
      [JMP] = &&do_JMP,
      [JMPZ] = &&do_JMPZ,
      [TRY] = &&do_TRY,
      [SETUP_CATCH] = &&do_SETUP_CATCH,
      [LOAD_SUBSCRIPT] = &&do_LOAD_SUBSCRIPT,
      [STORE_SUBSCRIPT] = &&do_STORE_SUBSCRIPT,
      [INITARRAY] = &&do_INITARRAY,
      [INITOBJ] = &&do_INITOBJ,
      [SETPROP] = &&do_SETPROP,
      [GETPROP] = &&do_GETPROP,
      [IRETURN] = &&do_IRETURN,
      [LOAD_CONST] = &&do_LOAD_CONST,
      [STORE_NAME] = &&do_STORE_NAME,
      [LOAD_NAME] = &&do_LOAD_NAME,
      [IDIV] = &&do_IDIV,
      [IADD] = &&do_IADD,
      [ILTE] = &&do_ILTE,
      [ITIMES] = &&do_ITIMES,
      [IMINUS] = &&do_IMINUS,
      [CALL] = &&do_CALL,
      [IUNARYPLUS] =  NULL,
      [IUNARYNOT] = NULL,
      [IEQUAL] = NULL,
      [INEQUAL] = NULL
    };
  #endif


  ana_frame *frame;
  ana_uint32_t **code;
  ana_long **jmptargets;
  ana_object **constants;
  ana_object *locals;


  ana_object *arg = NULL, *retval = NULL, *left, *right, *result;

  while(!COMO_VM_HAS_FRAMES())
  {
    enter:

    frame = COMO_VM_POP_FRAME();
    code = (ana_uint32_t **)(((ana_container *)frame->code)->data);
    jmptargets = (ana_long **)(((ana_array *)frame->jmptargets)->items);
    constants = (ana_object **)(((ana_array *)frame->constants)->items);
    locals = frame->locals;


    #if 0

    fprintf(stdout, "[vm.EXEC] executing frame %s\n", ana_cstring(frame->name));

    #endif

    if(vm->flags & (COMO_VM_TRACING|COMO_VM_LIVE_TRACING))
      trace_frame(vm, frame);

    if(!frame->ready)
      ana_object_init(frame);

    //fprintf(stdout, "[vm.EXEC]: start execute of frame %s\n", 
    // ana_cstring(frame->name));

    ana_uint32_t opline;
    ana_uint32_t opcode;
    //ana_size_t *__pc = &frame->pc;
    short oparg;

    for(;;) {
      top:

        //fprintf(stdout, "[vm.EXEC]: current pc is %ld\n", frame->pc);
        //fprintf(stdout, "[vm.EXEC]: code size is %ld\n", ana_container_size(frame->code));
      
      #ifndef ANA_VM_COMPUTED_GOTO
        fetch();
      #else
        vm_continue();
      #endif

      //fprintf(stdout, "[vm]: current opcode is %u at pc = %ld\n",
      //     opcode, frame->pc);

      vm_case(opcode) {
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

            GC_TRACK(frame, (ana_object *)execframe);

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

            GC_TRACK(frame, (ana_object *)execframe);

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
              GC_TRACK(frame, result);
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
          
          //ana_object *targetpcobj = ana_get_array(frame->jmptargets)->items[ 
          //  (ana_size_t)oparg];

          //frame->pc = (ana_size_t)(ana_get_long(targetpcobj)->value);
          
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
           //ana_object *targetpcobj 
           //   = ana_get_array(frame->jmptargets)->items[(ana_size_t)oparg];
            
           // frame->pc = (ana_size_t)(ana_get_long(targetpcobj)->value);
            
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
          GC_TRACK(frame, exvalue);
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
          ana_object *res = getindex(frame, container, index);

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
          ana_object *res = setindex(frame, container, index, value);   
          
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

          GC_TRACK(frame, result);

          while(oparg--)
          {
            left = pop();
            ana_array_push(result, left);
          }

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

          GC_TRACK(frame, obj);

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
          assert(arg);
          push(arg);
          vm_continue();
        }
        vm_target(STORE_NAME) {
          TRACE(STORE_NAME, oparg, 0, 1);
          ana_object *thename = ana_array_get(vm->symbols, oparg);
          result = pop();
          //ana_put_local(locals, name, result);
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

          result = do_div(frame, left, right);

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
            GC_TRACK(frame, result);
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
          right = pop();
          left  = pop();

          result = NULL;

          if(ana_type_check_both(left, right, ana_long_type))
          {
            result = ana_longfromlong(
              ana_get_long(left)->value + ana_get_long(right)->value
            );
          } 
          else 
          {
            if(left->type->obj_binops != NULL 
              && left->type->obj_binops->obj_add != NULL) 
                result = left->type->obj_binops->obj_add(left, right);
          }

          if(result) 
          {
            GC_TRACK(frame, result);
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

          result = mul(frame, left, right);

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

          result = sub(frame, left, right);

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
          callable->flags++;

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
              
              GC_TRACK(frame, (ana_object *)execframe);

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

              if(res) {
                GC_TRACK(frame, res);
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
            GC_TRACK(frame, self);
            ana_map_put(theclass->members, self, callable);
            
            ana_object *__name__ = ana_stringfromstring("__name__");
            GC_TRACK(frame, __name__);
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

                  GC_TRACK(frame, (ana_object *)execframe);
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

      if(ex) {
        /* Find the exception handler */

       // fprintf(stdout, "[vm]: exception occurred in frame %s\n",
       //   ana_cstring(frame->name));
      //  ana_size_t cpc = frame->pc;
       // fprintf(stdout, "[vm]: caught exception at address %d with message %s\n", 
       //   (int)cpc, ex);

        ana_frame *thisframe = frame;
        ana_frame *originalframe = frame;

        while(thisframe != NULL) 
        {
          #if 0
          fprintf(stdout, "[vm]: checking fram %s for fault handler\n",
            ana_cstring(thisframe->name));
          #endif

          if(thisframe->bp == 0)
          {
            #if 0
            fprintf(stdout, "[vm]: checked all blocks in this frame\n");
            if(thisframe->parent != NULL) {
              fprintf(stdout, "[vm]: the caller of this frame is %s\n",
                ana_cstring(ana_get_frame(thisframe->parent)->name));
            }
            #endif
          }

          ana_basic_block *block;
          while(!(thisframe->bp == 0))
          {
            #if 0
            fprintf(stdout, "[vm]: popping a block from frame %s\n", 
              ana_cstring(thisframe->name));
            #endif
            block = thisframe->blockstack[--thisframe->bp];

            #if 0
            fprintf(stdout, "[vm]: setting pc to %d\n", block->jmpto);
            #endif

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

            //fprintf(stdout, "[popped frame]: %s\n",
            //  ana_cstring(thisframe->name));
            //fprintf(stdout, "  bpsize is %ld, pos is %ld\n", thisframe->bpsize,
            //  thisframe->bp);
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
        // fprintf(stdout, "    thrown frame %s in file \"%s\" on line %d\n", 
        // ana_cstring(originalframe->name), ana_cstring(frame->filename), line);


        free(ex);

        ana_print_backtrace(frame);
        
        #if 0
        fprintf(stdout, "--- Symbol Table for this frame ---\n");
        assert(frame->locals);
        if(frame->locals) {
          ana_map_foreach(frame->locals, key, value) {
            ana_object_print(key);
            fputc(':', stdout);
            ana_object_print(value);
            fputc('\n', stdout);
          } ana_map_foreach_end();
          fprintf(stdout, "--- End Symbol Table ---\n");
        }
        #endif

        ex = NULL;
        /* todo long jump here */
        exit(1);
      }
    }
    exit:
      while(!empty()) {
        (void)pop();
      }


      printf("vm: calling gc for frame %s\n",
        ana_cstring(frame->name));

      do_gc(vm, frame);

      ana_object_finalize(frame);

      fflush(stdout);
  }

  return retval;
}

static void ana_print_backtrace(ana_frame *frame)
{
  ana_frame *fm = frame;

  //if(fm)
   // fm = (ana_frame *)frame->parent;

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


  /* since this global frame is special, C needs to control this rather than the VM
     because the first frame isn't added to Como's Gc
  */
  do_gc(vm, frame);

  ana_object *root = frame->root;

  if(root == NULL) {
    printf("root is NULL\n");
  }

  ana_object_dtor(code);

  ana_bool_type_finalize();

  return 0;
}
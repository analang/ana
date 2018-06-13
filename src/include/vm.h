#ifndef COMO_VM_H
#define COMO_VM_H

#include <ana.h>
#include "ana_debug.h"

#define ANA_GC_DEBUG 1


#define COMO_VM_GC_TIMEOUT     10
#define COMO_VM_TRACING        (1 << 0)
#define COMO_VM_LIVE_TRACING   (1 << 1)
#define COMO_VM_GC_DISABLED    (1 << 2)
#define COMO_VM_TRACING_ANY    (COMO_VM_TRACING | COMO_VM_LIVE_TRACING)

#define COMO_VM_STACK_MAX 255

typedef struct ComoVM ComoVM;

struct ComoVM
{
  ana_object *symbols;
  ana_object *exception;
  ana_frame *stack[COMO_VM_STACK_MAX]; 
  ana_size_t stacksize;
  ana_size_t stackpointer;
  ana_uint32_t flags;

  ana_size_t nobjs;
  ana_size_t mxobjs;
  ana_object *root; 
  void(*do_gc)(struct ComoVM *vm);
};

ComoVM *ana_vm_new();
void ana_vm_finalize(ComoVM *vm);
int ana_eval(ComoVM *vm, ana_object *code, char *function);

#define make_symbol(vm, symbol) \
  (ana_array_push(vm->symbols, ana_stringfromstring((symbol))), \
    (ana_array_size(vm->symbols) - 1))

#define GC_TRACK(vm, obj) do { \
  if(ana_type_is((obj), ana_frame_type)) { \
    fprintf(stdout, "%s:%s:%d can't GC track frame types\n", \
      __FILE__, __FUNCTION__, __LINE__); \
  } \
  if (vm->nobjs == vm->mxobjs) \
  { \
    vm->do_gc(vm); \
  } \
  ana_get_base(obj)->next = vm->root; \
  ana_get_base(obj)->flags = 0; \
  vm->root = ana_get_base(obj); \
  vm->nobjs++; \
} while(0)

#endif
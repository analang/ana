#ifndef COMO_VM_H
#define COMO_VM_H

#include <ana.h>
#include "ana_debug.h"

#define COMO_VM_GC_TIMEOUT     10
#define COMO_VM_TRACING        (1 << 0)
#define COMO_VM_LIVE_TRACING   (1 << 1)
#define COMO_VM_GC_DISABLED    (1 << 2)
#define COMO_VM_TRACING_ANY    (COMO_VM_TRACING | COMO_VM_LIVE_TRACING)

#define COMO_VM_STACK_MAX 255

typedef struct ana_vm ana_vm;

struct ana_vm
{
  ana_object *symbols;
  ana_object *constants;
  ana_object *exception;
  ana_frame  *stack[COMO_VM_STACK_MAX];
  ana_size_t stacksize;
  ana_size_t stackpointer;
  ana_uint32_t flags;

  ana_size_t nobjs;
  ana_size_t mxobjs;
  ana_object *root; 

  ana_frame *base_frame;
  ana_object *frameroot;
  void(*do_gc)(ana_vm *);
};

ana_vm *ana_vm_new();
void ana_vm_finalize(ana_vm *vm);
int ana_eval(ana_vm *vm, ana_function *function, char *function_name);
ana_object *ana_vm_new_symbol(ana_vm *vm, char *symbol);

#define make_symbol(vm, symbol) \
  (ana_array_push(vm->symbols, ana_stringfromstring((symbol))), \
    (ana_array_size(vm->symbols) - 1))

#define NEW_SYMBOL make_symbol
  
#define GC_TRACK(vm, obj) do { \
  if (vm->nobjs == vm->mxobjs) \
  { \
    vm->do_gc(vm); \
  } \
  ana_get_base(obj)->next = vm->root; \
  vm->root = ana_get_base(obj); \
  vm->nobjs++; \
} while(0)


#endif
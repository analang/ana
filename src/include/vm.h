#ifndef COMO_VM_H
#define COMO_VM_H

#include <ana.h>

#include "ana_debug.h"

#define COMO_VM_GC_TIMEOUT     10
#define COMO_VM_TRACING        0x00000001
#define COMO_VM_LIVE_TRACING   0x00000002

typedef struct ana_gc_root {
  ana_frame *value;
  struct ana_gc_root *next;
} ana_gc_root;

#define COMO_VM_STACK_MAX 255

typedef struct ComoVM {
  ana_object *symbols;
  ana_object *exception;
  ana_frame *stack[COMO_VM_STACK_MAX]; /* frame stack, represents functions */
  ana_size_t stacksize;
  ana_size_t stackpointer;
  ana_uint32_t flags;
  ana_object *root; 
} ComoVM;

ComoVM *ana_vm_new();

void ana_vm_finalize(ComoVM *vm);

void do_gc(ComoVM *vm, ana_frame *frame);

int ana_eval(ComoVM *vm, ana_object *code, char *function);

#define make_symbol(vm, symbol) \
  (ana_array_push(vm->symbols, ana_stringfromstring((symbol))), \
    (ana_array_size(vm->symbols) - 1))
  
#define ana_eval_ex(vm, code, trace) ana_eval(vm, code, NULL)
#define ana_eval_func_ex(vm, code, fn, trace) ana_eval(vm, code, fn)

#define should_grow(f) ana_frame_growstack((ana_object *)f)

#define gc_new(vm, frame, obj) \
  gc_new_ex(vm, frame, obj, 1, __FUNCTION__, __FILE__, __LINE__)

#define gc_new_noref(vm, frame, obj) \
  gc_new_ex(vm, frame, obj, 0, __FUNCTION__, __FILE__, __LINE__)

static inline ana_object *gc_new_ex(ComoVM *vm,
  ana_frame *frame, ana_object *obj, int initrefcount, const char *fn, 
  const char *file, int lineno)
{  

COMO_UNUSED(fn);
COMO_UNUSED(file);
COMO_UNUSED(lineno);
COMO_UNUSED(frame);
COMO_UNUSED(vm);

  if (frame->nobjs >= frame->mxobjs) {
    do_gc(vm, frame);
  }

  obj->next = frame->root;
  obj->flags = initrefcount;

  frame->root = obj;

  frame->nobjs++;

  return obj;
}


#endif
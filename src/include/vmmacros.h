#define ANA_TRACE_DEBUG_ENABLED

#define vm_case(o) switch(o)
#define vm_target(x) case x: \
  TRACE(x, #x, oparg, 0, 1); \

#define vm_continue() break

#ifdef ANA_TRACE_DEBUG_ENABLED
# define DO_TRACE(op, strop, arg, flag, argused) \
  do \
  { \
    fprintf(stdout, "%-5d: ", (int)frame->pc -1); \
    if(argused) \
    { \
      fprintf(stdout, "%-25s%-10d\n", strop, arg); \
    } \
    else \
    { \
      fprintf(stdout, "%-25s\n", strop); \
    } \
  } while(0)
  
# define TRACE(op, strop, arg, flag, argused) \
  if(1) \
  { \
    if(vm->flags & COMO_VM_TRACING) \
    { \
      DO_TRACE(op, strop, arg, flag, argused); \
      if(op != IRETURN) \
        break; \
      else \
        goto exit; \
      } \
    else if(vm->flags & COMO_VM_LIVE_TRACING) \
    { \
      DO_TRACE(op, strop, arg, flag, argused); \
    } \
  } \

#else
#   define TRACE(op, strop, arg, flag, argused) do {} while(0)
#endif


#define fetch() do { \
  assert(frame->pc >= 0 && frame->pc < ana_container_size(frame->code)); \
  \
  ana_object *pc = ana_longfromlong((long)frame->pc); \
  ana_object *theline = ana_map_get(frame->line_mapping, pc); \
  \
  if(theline) \
    current_line = (int)ana_get_long(theline)->value; \
  else \
    current_line = 0; \
  ana_object_dtor(pc); \
  \
  opline = (ana_uint32_t)((unsigned long)(code[frame->pc])); \
  opcode = (opline >> 24) & 0xff; \
  oparg = (opline >> 8) & 0xffff; \
  opflag =  (opline) & 0xff; \
  frame->pc++;  \
  \
  ana_uint32_t next_op_line = (ana_uint32_t)((unsigned long)(code[frame->pc])); \
  next_op_code = (next_op_line >> 24) & 0xff; \
} while(0)

#define get_const(x) (constants[x])
#define get_arg() (oparg)

#define push_ex(frame, arg) do { \
  assert(frame); \
  if(frame->sp >= frame->sz) \
  { \
    frame->sz = frame->sz * 2; \
    frame->stack = realloc(frame->stack, sizeof(ana_object *) * frame->sz); \
  } \
  frame->stack[frame->sp++] = arg; \
  if(frame->loop->stack_size != 0) { \
    ana_size_t pos = frame->loop->stack_position == 0 ? 0 : (frame->loop->stack_position - 1);\
    frame->loop->stack[pos]->stack_obj_count++; \
  } \
} while(0)

#define push(arg) push_ex(frame, arg)

#define pushto(frame, arg) \
  push_ex(frame, arg)
  
#define xpop() \
 (frame->stack[--frame->sp])

#define pop_ex() \
 ((frame->loop->stack_size != 0 ? (--frame->loop->stack[frame->loop->stack_position == 0 ? 0 : \
    (frame->loop->stack_position - 1)]->stack_obj_count) : -1), (frame->stack[--frame->sp]))

#define pop2(frame) \
 ((frame->loop->stack_size != 0 ? (--frame->loop->stack[frame->loop->stack_position == 0 ? 0 : \
    (frame->loop->stack_position - 1)]->stack_obj_count) : -1), (frame->stack[--frame->sp]))

#define pop pop_ex

#define empty() \
  (frame->sp == 0)

#define set_except(exception_type, fmt, ...) do { \
  ex_type = exception_type; \
  ex = make_except(fmt, ##__VA_ARGS__); \
} while(0)

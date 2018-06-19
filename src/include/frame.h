#ifndef COMO_FRAME_H
#define COMO_FRAME_H

# if !(defined(COMO_BASE_INCLUDED))
#   error "Please do not include class.h directly"
#endif

/* Internal use only */
typedef struct _ana_basic_block {
  union {
    int jmpto;
    int stack_obj_count;
  };
  struct _ana_basic_block *next;
} ana_basic_block;

typedef struct ana_generic_block
{
  ana_basic_block **stack;
  ana_basic_block  *root;
  ana_size_t        stack_size;
  ana_size_t        stack_capacity;
  ana_size_t        stack_position;
} ana_generic_block;

typedef struct _ana_frame ana_frame;

struct _ana_frame {
  /* 24 bytes */
  ana_object  base;
  ana_object *code;             /* this is not allocated or deallocated here, it's provided by a function defn */
  ana_size_t  pc;               /* current index into the code array, reset to 0 at EOO (end of execution) */
  ana_object **stack;           /* this is dynamically allocated */
  ana_size_t sz;                /* keeps its size across reuse */
  ana_size_t sp;                /* always reset to 0, at EOO */
  ana_object *locals;           /* hash map of names to values */
  ana_object *globals;          /* global variables */
  ana_object          *self;    /* the current object */
  ana_generic_block  *loop;     
  ana_generic_block  *exception;
  ana_object         *jump_targets;
  ana_object *name;             /* the name of the function being executed */
  ana_size_t  activation_line_number;
  ana_object *line_mapping;
  ana_object *filename;
  ana_frame  *caller;
  ana_object *retval;
};

ana_frame *ana_frame_new(
    ana_object *code, 
    ana_object *jumptargets, 
    ana_object *line_mapping, 
    ana_object *global_variables, 
    ana_object *name, 
    ana_frame  *caller, 
    int line, 
    ana_object *filename
);

#define ana_get_frame(o) ((ana_frame *)((o)))

extern ana_type ana_frame_type;

COMO_OBJECT_API ana_basic_block *ana_basic_block_new(int targetaddress);

COMO_OBJECT_API void ana_frame_growstack(ana_object *);

#define COMO_FRAME_STACK_SIZE     8
#define COMO_FRAME_CONSTANTS_SIZE 8
#define COMO_FRAME_LOCALS_SIZE    8
#define COMO_FRAME_MAX_OBJECTS    16
#define COMO_CODE_SIZE            8
#define COMO_PARAMS_SIZE          4
#define COMO_BLOCK_STACK_MAX      16
#define COMO_FRAME_DEFN           (1 << 0)
#define COMO_FRAME_EXEC           (1 << 1)
#define COMO_FRAME_MODULE         (1 << 2)

#endif

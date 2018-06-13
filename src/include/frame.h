#ifndef COMO_FRAME_H
#define COMO_FRAME_H

# if !(defined(COMO_BASE_INCLUDED))
#   error "Please do not include class.h directly"
#endif

#define COMO_BLOCK_STACK_MAX      16

/* Internal use only */
typedef struct _ana_basic_block {
  union {
    int jmpto;
    int stack_obj_count;
  };
  struct _ana_basic_block *next;
} ana_basic_block;

typedef struct _ana_frame 
{
  ana_object base;
  ana_size_t pc;
  ana_object *code;
  ana_object **stack;
  ana_size_t sz;
  ana_size_t sp;


  ana_basic_block **loopstack;
  ana_size_t loopp;
  ana_size_t loopsize;
  ana_basic_block *looproot;

  ana_object *calling_module;
  ana_size_t ready;
  ana_size_t finalized;
  ana_size_t frameflags;
  ana_object *name; 
  ana_object *params;  /* parameter names, in reverse calling order */
  ana_object *constants;
  ana_object *locals;
  ana_object *root;  
  ana_size_t nobjs;   /* Total objects currently in cycle */
  ana_size_t nobjslt; /* Lifetime allocation count        */
  ana_size_t mxobjs;  /* Max objects */

  ana_object *parent;  /* The calling frame */
  ana_object *globals;
  ana_object *self;   /* instance */
  ana_object *linemapping; /* pc to source line mapping */
  ana_object *filename;
  ana_object *jmptargets;
  ana_object *retval;
  
  ana_basic_block  **blockstack;
  ana_basic_block   *blockroot; /* for tracking */

  ana_size_t bp;
  ana_size_t bpsize;
  ana_size_t lineno; /* line this frame was activated on */
} ana_frame;

#define ana_get_frame(o) ((ana_frame *)((o)))

extern ana_type ana_frame_type;

COMO_OBJECT_API ana_frame *ana_frame_base_new(
  ana_object *name, 
  ana_object *filename,
  ana_object *callingframe,
  ana_object *code,
  ana_object *constants,
  ana_object *jmptargets,
  ana_object *linemapping,
  ana_size_t bpsize
);

COMO_OBJECT_API ana_object *ana_frame_new(char *name, char *filename, 
  ana_object *callingframe);

COMO_OBJECT_API ana_basic_block *ana_basic_block_new(int targetaddress);

COMO_OBJECT_API void ana_frame_growstack(ana_object *);

#define COMO_FRAME_STACK_SIZE     8
#define COMO_FRAME_CONSTANTS_SIZE 8
#define COMO_FRAME_LOCALS_SIZE    8
#define COMO_FRAME_MAX_OBJECTS    16
#define COMO_CODE_SIZE            8
#define COMO_PARAMS_SIZE          4
#define COMO_FRAME_DEFN           0x00000001
#define COMO_FRAME_EXEC           0x00000002

#endif

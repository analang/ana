#include <ana.h>
#include <assert.h>

COMO_OBJECT_API ana_frame *ana_frame_base_new(
  ana_object *name, 
  ana_object *filename,
  ana_object *callingframe,
  ana_object *code,
  ana_object *constants,
  ana_object *jmptargets,
  ana_object *linemapping,
  ana_size_t bpsize
  )
{
  ana_frame *obj = malloc(sizeof(*obj));
  ana_size_t i;

  obj->base.type = &ana_frame_type;
  obj->base.next = NULL;
  obj->base.scope = NULL;
  obj->base.flags = 0;

  obj->frameflags = COMO_FRAME_EXEC;
  obj->name       = name;
  obj->params     = NULL;
  obj->code       = code;
  obj->constants  = constants;
  obj->stack      = malloc(sizeof(ana_object *) * COMO_FRAME_STACK_SIZE); 
  assert(obj->stack);
  obj->root       = NULL;
  obj->pc         = 0;
  obj->sz         = COMO_FRAME_STACK_SIZE;
  obj->sp         = 0;
  obj->nobjs      = 0;
  obj->nobjslt    = 0;
  
  obj->mxobjs     = COMO_FRAME_MAX_OBJECTS;
  obj->parent     = callingframe;
  obj->ready      = 0;

  obj->globals    = NULL;
  obj->self        = NULL;
  obj->linemapping = linemapping;
  obj->filename    = filename;
  obj->finalized   = 0;
  obj->jmptargets  = jmptargets;
  obj->bp          = 0;
  obj->bpsize      = bpsize;
  obj->retval      = NULL;
  obj->lineno      = 0;
  obj->blockstack  = NULL;
  obj->blockroot   = NULL;

  for(i = 0; i < obj->sz; i++)
    obj->stack[i] = NULL;

  return obj;
}


COMO_OBJECT_API ana_object *ana_frame_new(char *name, char *filename,
  ana_object *callingframe)
{
  ana_frame *obj = malloc(sizeof(*obj));
  ana_size_t i;

  obj->base.type = &ana_frame_type;
  obj->base.next = NULL;
  obj->base.scope = NULL;
  obj->base.flags = 0;

  obj->frameflags = COMO_FRAME_DEFN;
  obj->name       = ana_stringfromstring(name);
  obj->params     = NULL;
  obj->code       = ana_code_new(COMO_CODE_SIZE);
  obj->constants  = ana_array_new(COMO_FRAME_CONSTANTS_SIZE);
  obj->locals     = NULL,
  obj->stack      = malloc(sizeof(ana_object *) * COMO_FRAME_STACK_SIZE); 
  obj->root       = NULL;
  obj->pc         = 0;
  obj->sz         = COMO_FRAME_STACK_SIZE;
  obj->sp         = 0;
  obj->nobjs      = 0;
  obj->nobjslt    = 0;
  
  obj->mxobjs     = COMO_FRAME_MAX_OBJECTS;
  obj->parent     = callingframe;
  obj->ready      = 0;

  obj->globals    = NULL;
  obj->self        = NULL;
  obj->linemapping = ana_map_new(4);
  obj->filename    = ana_stringfromstring(filename);
  obj->finalized   = 0;
  obj->jmptargets  = ana_array_new(4);
  obj->bp          = 0;
  obj->bpsize      = 0;
  obj->retval      = NULL;
  obj->lineno      = 0;

  obj->blockstack = NULL;
  obj->blockroot  = NULL;

  for(i = 0; i < obj->sz; i++)
    obj->stack[i] = NULL;

  return (ana_object *)obj;
}

COMO_OBJECT_API ana_basic_block *ana_basic_block_new(int targetaddress)
{
  ana_basic_block *block = malloc(sizeof(*block));
  block->jmpto = targetaddress;

  return block;
}

COMO_OBJECT_API void ana_frame_growstack(ana_object *obj)
{
  ana_frame *self = ana_get_frame(obj);

  if(self->sp >= self->sz)
  {
    self->sz = self->sz * 2;
    self->stack = realloc(self->stack, sizeof(ana_object *) * self->sz);
  }
}

static void frame_init(ana_object *obj)
{
  ana_frame *self = ana_get_frame(obj);

//  fprintf(stdout, "frame_init: for frame `%s`\n", 
//    ana_cstring(self->name));

  self->locals = ana_map_new(COMO_FRAME_LOCALS_SIZE);
  self->pc = 0;
  self->ready = 1;
  self->finalized = 0;
  self->sp = 0;
  self->blockstack = malloc(sizeof(ana_basic_block *) * COMO_BLOCK_STACK_MAX);

  ana_size_t i;
  for(i = 0; i < COMO_BLOCK_STACK_MAX; i++)
    self->blockstack[i] = NULL;
}

static void frame_deinit(ana_object *obj)
{
  ana_frame *self = ana_get_frame(obj);


 // fprintf(stdout, "frame_deinit: for frame `%s`\n", 
 //    ana_cstring(self->name));

  self->pc = 0;
  self->ready = 0;
  self->parent = NULL;
  self->finalized = 1;
  self->retval = NULL;
  self->bp = 0;
  self->self = NULL;
  self->lineno = 0;

  ana_basic_block *blockroot = self->blockroot;

  while(blockroot)
  {
    ana_basic_block *next = blockroot->next;
    free(blockroot);
    blockroot = next;
  }

  free(self->blockstack);

  assert(self->locals != NULL);
  ana_object_dtor(self->locals);
  
  self->bpsize = 0;
}

static void frame_dtor(ana_object *obj)
{
  ana_frame *self = ana_get_frame(obj);

  if(self->ready)
    frame_deinit(obj);
  
  if(self->frameflags & COMO_FRAME_EXEC)
  {
    if(obj->scope)
      ana_object_dtor(obj->scope);

    free(self->stack);
    free(self);
    return;
  }

#ifdef ANA_GC_DEBUG
  fprintf(stdout, "frame_dtor: for frame `%s`\n", 
    ana_cstring(self->name));
#endif
  
  ana_object_dtor(self->name);
  ana_object_dtor(self->code);
  ana_object_dtor(self->filename);

  ana_array_foreach_apply(self->jmptargets, ana_object_dtor);
  ana_object_dtor(self->jmptargets);
  ana_array_foreach_apply(self->constants, ana_object_dtor);
  
  ana_object_dtor(self->constants);

  ana_map_foreach(self->linemapping, key, value) 
  {
    ana_object_dtor(key);
    ana_object_dtor(value);
  } ana_map_foreach_end();

  ana_object_dtor(self->linemapping);

  if(self->params) 
  {
    ana_array_foreach_apply(self->params, ana_object_dtor);
    ana_object_dtor(self->params);
  }


  if(obj->scope)
    ana_object_dtor(obj->scope);

  free(self->stack);
  free(self);
}

static void frame_print(ana_object *obj)
{
  ana_frame *self = ana_get_frame(obj);

  fprintf(stdout, "<frame %s at %p>", 
    ((ana_string*)self->name)->value, (void *)obj);
}

static ana_object *frame_string(ana_object *obj)
{
  ana_frame *self = ana_get_frame(obj);
  char *buffer = NULL;
  int size = 0;
  size = snprintf(buffer, size, "<frame %s at %p>", 
    ((ana_string*)self->name)->value, (void *)obj);

  size++;

  buffer = malloc(size);

  snprintf(buffer, size, "<frame %s at %p>", ((ana_string*)self->name)->value, 
            (void *)obj);

  buffer[size - 1] = '\0';

  ana_object *retval = ana_stringfromstring(buffer);
  
  free(buffer);

  return retval; 
}

static int frame_equals(ana_object *obj, ana_object *b)
{
  return obj == b;
}

static int frame_bool(ana_object *obj)
{
  COMO_UNUSED(obj);

  return 1;
}

static ana_object *frame_equals_wrap(ana_object *obj, ana_object *x)
{
  return ana_boolfromint(frame_equals(obj, x));
}

static ana_comparison_ops compops = {
  .obj_eq  = frame_equals_wrap,
  .obj_neq = NULL,
  .obj_gt  = NULL,
  .obj_lt  = NULL,
  .obj_gte = NULL,
  .obj_lte = NULL
};

ana_type ana_frame_type = {
  .obj_name    = "frame",
  .obj_print   = frame_print,
  .obj_dtor    = frame_dtor,
  .obj_equals  = frame_equals,
  .obj_bool    = frame_bool,
  .obj_hash    = NULL,
  .obj_str     = frame_string,
  .obj_init    = frame_init,
  .obj_deinit  = frame_deinit,
  .obj_binops  = NULL,
  .obj_unops   = NULL,
  .obj_compops = &compops,
  .obj_seqops  = NULL
};
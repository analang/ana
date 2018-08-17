#include <ana.h>
#include <assert.h>

ana_frame *ana_frame_new(
  ana_object *code, 
  ana_object *jumptargets, 
  ana_object *line_mapping, 
  ana_object *global_variables, 
  ana_object *name, 
  ana_frame *caller, 
  int line, 
  ana_object *filename)
{
  ana_frame *obj = malloc(sizeof(*obj));

  obj->base.type = &ana_frame_type;
  obj->base.next = NULL;
  obj->base.flags = 0;
  obj->base.refcount = 0;
  obj->base.is_tracked = 0;

  obj->code             = code;
  obj->flags            = 0;
  obj->pc               = 0;
  obj->current_line     = 0;
  obj->stack            = malloc(sizeof(ana_object *) * COMO_FRAME_STACK_SIZE); 
  obj->sz               = COMO_FRAME_STACK_SIZE;
  obj->sp               = 0;
  obj->locals           = ana_map_new(4);
  obj->globals          = global_variables;
  obj->self             = NULL;

  obj->loop                     = malloc(sizeof(*obj->loop));  
  obj->loop->stack              = malloc(sizeof(ana_basic_block *) * COMO_BLOCK_STACK_MAX);
  obj->loop->root               = NULL;
  obj->loop->stack_size         = 0;
  obj->loop->stack_capacity     = COMO_BLOCK_STACK_MAX;
  obj->loop->stack_position     = 0;


  obj->exception                 = malloc(sizeof(*obj->exception));
  obj->exception->stack          = malloc(sizeof(ana_basic_block *) * COMO_BLOCK_STACK_MAX);
  obj->exception->root           = NULL;
  obj->exception->stack_size     = 0;
  obj->exception->stack_capacity = COMO_FRAME_STACK_SIZE;
  obj->exception->stack_position = 0;

  obj->jump_targets           = jumptargets;
  obj->name                   = name;

  obj->activation_line_number = line;
  obj->line_mapping           = line_mapping;
  obj->filename               = filename;
  obj->caller                 = caller;
  obj->retval                 = NULL;
  obj->module                 = NULL;
  
  return obj;
}

/* Only free what was allocated in ana_frame_new */
static void frame_dtor(ana_object *obj)
{
  ana_frame *self = ana_get_frame(obj);
  ana_basic_block *loop_root = self->loop->root;
  ana_basic_block *exception_root = self->exception->root;
  ana_basic_block *next;

  while(loop_root)
  {
    next = loop_root->next;

    free(loop_root);
    
    loop_root = next;
  }

  while(exception_root)
  {
    next = exception_root->next;
    
    free(exception_root);

    exception_root = next;
  }

  free(self->loop->stack);
  free(self->loop);

  free(self->exception->stack);
  free(self->exception);

  free(self->stack);

  if(!(self->flags & COMO_FRAME_MODULE))
  {
    ana_object_dtor(self->locals);
  }

  free(self);
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
  COMO_UNUSED(obj);
}

static void frame_deinit(ana_object *obj)
{
  ana_frame *self = ana_get_frame(obj);

  COMO_UNUSED(self);
}

static void frame_print(ana_object *obj)
{
  ana_frame *self = ana_get_frame(obj);

  fprintf(stdout, "<frame '%s' at %p>", ana_cstring(self->name), 
      (void *)obj);
}

static ana_object *frame_string(ana_object *obj)
{
  ana_frame *self = ana_get_frame(obj);

  return ana_stringfromstring(
      ana_build_str("<frame '%s' at %p>", ana_cstring(self->name), (void *)self)
  );
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
  .obj_seqops  = NULL,
  .obj_get_attr = NULL,
  .obj_props    = NULL,
  .obj_iter     = NULL,
  .obj_iter_next = NULL
};
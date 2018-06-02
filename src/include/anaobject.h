#ifndef COMO_OBJECT_H
#define COMO_OBJECT_H

#define COMO_OBJECT_API __attribute__((__visibility__("default")))

/* We're modeling the object system with all of C's operators */
typedef struct _ana_type   ana_type;
typedef struct _ana_object ana_object;
typedef struct _ana_binary_ops ana_binary_ops;
typedef struct _ana_unary_ops ana_unary_ops;
typedef struct _ana_comparison_ops ana_comparison_ops;
typedef struct _ana_logical_ops ana_logical_ops;


typedef long ana_size_t;

struct _ana_object {
  ana_size_t flags;
  struct _ana_type   *type;
  struct _ana_object *next;
  struct _ana_object *scope;
};

#define ana_object_add(self, x) \
  ((ana_object *)self)->type->obj_binops->obj_add((self), (x))

struct _ana_binary_ops {
  ana_object *(*obj_add)(ana_object *, ana_object *);
  ana_object *(*obj_mul)(ana_object *, ana_object *);
  ana_object *(*obj_div)(ana_object *, ana_object *);
  ana_object *(*obj_sub)(ana_object *, ana_object *);
  ana_object *(*obj_rem)(ana_object *, ana_object *);
};

struct _ana_unary_ops {
  ana_object *(*obj_plus)(ana_object *);
  ana_object *(*obj_minus)(ana_object *);
};

/* Technically, we can do this with 2 operations, 
   but let's just make it easy for now 
*/
struct _ana_comparison_ops {
  ana_object *(*obj_eq)(ana_object *, ana_object *);
  ana_object *(*obj_neq)(ana_object *, ana_object *);
  ana_object *(*obj_gt)(ana_object *, ana_object *);
  ana_object *(*obj_lt)(ana_object *, ana_object *);
  ana_object *(*obj_gte)(ana_object *, ana_object *);
  ana_object *(*obj_lte)(ana_object *, ana_object *);
};

struct _ana_logical_ops {
  ana_object *(*obj_not)(ana_object *);
  ana_object *(*obj_and)(ana_object *, ana_object *);
  ana_object *(*obj_or)(ana_object *, ana_object *);
};

struct _ana_seq_ops {
  ana_object *(*get)(ana_object *, ana_object *);
  ana_object *(*set)(ana_object *, ana_object *, ana_object *);
};

struct _ana_type {
  const char *obj_name;
  void(*obj_print)(ana_object *);
  void(*obj_dtor)(ana_object *);
  int(*obj_equals)(ana_object *, ana_object *);
  int(*obj_bool)(ana_object *);
  ana_usize_t(*obj_hash)(ana_object *);
  ana_object *(*obj_str)(ana_object *);
  void(*obj_init)(ana_object *);
  void(*obj_deinit)(ana_object *);
  struct _ana_binary_ops      *obj_binops;
  struct _ana_unary_ops       *obj_unops;
  struct _ana_comparison_ops  *obj_compops;
  struct _ana_seq_ops         *obj_seqops;
};

#define ana_object_init(x) ana_object_ctor(x)

#define ana_object_finalize(o) \
  (((ana_object *)(o))->type->obj_deinit((ana_object *)(o)))

#define ana_object_ctor(o) \
  (((ana_object *)(o))->type->obj_init((ana_object *)(o)))

#define ana_type_is(ob, tp) \
  (((ana_object *)(ob))->type == &tp)

#define ana_type_check_both(left, right, type) \
  (ana_type_is((left), (type)) && ana_type_is((right), (type)))

#define ana_type_name(o) \
  (((ana_object *)(o))->type->obj_name)

#define ana_object_print(o) do { \
  assert(((ana_object *)((o)))->type->obj_print != NULL); \
  (((ana_object *)((o)))->type->obj_print((ana_object *)((o)))); \
} while(0)

#define ana_object_tostring(o) \
  (((ana_object *)(o))->type->obj_str((ana_object *)(o)))

#define ana_object_dtor(o) \
  (((ana_object *)(o))->type->obj_dtor((ana_object *)(o)));

#define ana_get_base(o) \
  (((ana_object *)(o)))

#define ana_tostring_fast(obj, block) do { \
  assert(((ana_object *)(obj))->type->obj_str != NULL); \
  ana_object *_value = ana_object_tostring((obj)); \
  char *value = ana_cstring(_value); \
  block \
  ana_object_dtor(_value); \
} while(0)

#define ana_array_foreach_apply(ar, fn) \
  ana_array_foreach(ar, index, value) { \
    (void)index; \
    fn(value); \
  } ana_array_foreach_end()


#define ana_map_foreach_apply(map, fn) \
  ana_map_foreach(map, key, value) { \
    (void)key; \
    fn(value); \
  } ana_map_foreach_end()

#endif

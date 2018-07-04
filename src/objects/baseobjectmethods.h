typedef struct ana_base_object_methods {
  ana_object*(*toString)(ana_object *self);
  ana_object*(*getType)(ana_object *self);
} ana_base_object_methods;
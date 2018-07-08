#include <ana.h>

static inline void trace_frame(ana_vm *vm, ana_frame *frame)
{
  fprintf(stdout, "Ana Virtual Machine Disassembler\n");
  fprintf(stdout, "  Frame: %s\n", ana_cstring(frame->name));
  fprintf(stdout, "  Constant Table: \n");
  fprintf(stdout, "   %-15s%-13s%-11s\n", "Index", "Value", "Type"); 

  ana_size_t i;

  for(i = 0; i < ana_array_size(vm->constants); i++)
  {
    ana_object *value = ana_array_get(vm->constants, i);
    assert(value); 
    ana_string *_sval = (ana_string *)ana_object_tostring(value);
    char *sval = _sval->value;

    if(_sval->len > 13)
    {
      sval[12] = '\0';
    }

    fprintf(stdout, "   %-15d%-13.25s%-11s\n", (int)i, sval, 
      value->type->obj_name);

    ana_object_dtor(_sval);
  }

  fprintf(stdout, "  Jump Targets: \n");
  
  ana_array_foreach(frame->jump_targets, i, value) {
    ana_string *_sval = (ana_string *)ana_object_tostring(value);
    char *sval = _sval->value;
    fprintf(stdout, "   %-15d%-13.25s%-11s\n", (int)i, sval, 
      value->type->obj_name);
    ana_object_dtor(_sval);
  } ana_array_foreach_end();

  fprintf(stdout, "  VM Symbols:\n");

  ana_array_foreach(vm->symbols, i, value) {
    ana_tostring_fast(value, {
      fprintf(stdout, "   %-15d%-13.25s\n", (int)i, value); 
    });
  } ana_array_foreach_end();

  fputc('\n', stdout);
}
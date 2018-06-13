#include <ana.h>
#include <assert.h>

#include "ana_ast.h"
#include "opcodes.h"
#include "vm.h"
#include "builtin.h"


#define PACK_INSTRUCTION(opcode, argument, flag) \
    (ana_uint32_t)((((uint8_t)(opcode)) << 24) | \
    (((uint16_t)(argument)) << 8) | \
    ((uint8_t)(flag)))

#define emit(frame, opcode, arg, flag) \
  ana_code_push((frame)->code, PACK_INSTRUCTION(opcode, arg, flag))

#define emit_xx(frame, opcode, arg, flag, ast) do { \
  emit(frame, opcode, arg, flag); \
  ana_map_put(frame->linemapping, \
      ana_longfromlong( \
        (long)ana_container_size(frame->code)), ana_longfromlong((long)ast->line)); \
} while(0)

#define emitx(frame, opcode, arg, flag) emit_xx(frame, opcode, arg, flag, ast)

#define STUB_gc_new(vm, frame, obj) obj

#define int_const(frame, value) \
  (ana_array_push(frame->constants, STUB_gc_new(vm, frame, ana_longfromlong(value))), \
  (ana_array_size(frame->constants) - 1))

#define str_const(frame, value) \
  (ana_array_push(frame->constants, STUB_gc_new(vm, frame, ana_stringfromstring(value))), \
  (ana_array_size(frame->constants) - 1))

#define double_const(frame, value) \
  (ana_array_push(frame->constants, STUB_gc_new(vm, frame, ana_doublefromdouble(value))), \
  (ana_array_size(frame->constants) - 1))

#define bool_const(frame, value) \
  (ana_array_push(frame->constants, STUB_gc_new(vm, frame, ana_boolfromint(value))), \
  (ana_array_size(frame->constants) - 1))

static void ana_compile_unit(ComoVM *vm, ana_frame *frame, 
  node *ast);

static ana_object *get_identifier_list(node *params)
{
  ana_object *retval = ana_array_new(4);

  int i;
  for(i = 0; i < params->nchild; i++)
  {
    node *node = params->children[i];

    assert(node->kind == COMO_AST_ID);

    node_id *idnode = (node_id *)node;

    ana_array_push(retval, ana_stringfromstring(idnode->value));
  }

  return retval;
}

static inline ana_object *create_symbol(ComoVM *vm, char *name)
{
  ana_size_t symbol_index = make_symbol(vm, name);
  
  return 
    ana_get_array(vm->symbols)->items[symbol_index];
}

static void compile_func(ComoVM *vm, ana_frame *frame, node *ast)
{
  node_id *name = (node_id *)(ast->children[0]);
  node *params = ast->children[1];
  node *body = ast->children[2];
  
  ana_frame *fnframe = (ana_frame *)ana_frame_new(name->value,
    ana_cstring(frame->filename), NULL);

  char *fnname = ana_get_fn_name(fnframe);
  ana_get_base(fnframe)->scope = ana_stringfromstring(fnname);
  free(fnname);

  ((ana_object *)fnframe)->type->obj_init((ana_object *)fnframe); 

  fnframe->params = params->nchild > 0 ? get_identifier_list(params) : NULL;

  ana_compile_unit(vm, fnframe, body);

  if(body->nchild > 0) 
  {
    if(body->children[body->nchild - 1]->kind != COMO_AST_RETURN) 
    {
      emit(fnframe, LOAD_CONST, int_const(fnframe, 0), 0);
      emit(fnframe, IRETURN, 0, 1);
    }
  } 
  else
  {
    emit(fnframe, LOAD_CONST, int_const(fnframe, 0), 0);
    emit(fnframe, IRETURN, 0, 1);    
  }

  ana_object *fnimpl = ana_functionfromframe(fnframe);

  VM_FUNC_DEFN_TRACK(vm, fnimpl);

  ana_map_put(frame->locals, create_symbol(vm, (char *)name->value), fnimpl);

  char *fnname2 = ana_get_fn_name(fnframe);
  fnimpl->scope = ana_stringfromstring(fnname2);
  free(fnname2);
}

static void compile_class(ComoVM *vm, ana_frame *frame, node *ast)
{
  node_id *classname = (node_id *)(ast->children[0]);

  node *functions = ast->children[1];

  assert(functions->kind == COMO_AST_CLASS_STMTS);

  ana_object *members = ana_map_new(4);

  ana_size_t i;
  
  for(i = 0; i < functions->nchild; i++)
  {
    node *classmethod = functions->children[i];
    node *function = classmethod->children[0];    
    node_id *name = (node_id *)(function->children[0]);
    node *params = function->children[1];
    node *body = function->children[2];

    ana_frame *fnframe = (ana_frame *)ana_frame_new(name->value, 
      ana_cstring(frame->filename), NULL);
    
    ((ana_object *)fnframe)->type->obj_init((ana_object *)fnframe); 

    fnframe->params = params->nchild > 0 ? get_identifier_list(params) : NULL;

    ana_compile_unit(vm, fnframe, body);

    if(body->nchild > 0) 
    {
      if(body->children[body->nchild - 1]->kind != COMO_AST_RETURN) 
      {
        emit(fnframe, LOAD_CONST, int_const(fnframe, 0), 0);
        emit(fnframe, IRETURN, 0, 1);
      }
    } 
    else
    {
      emit(fnframe, LOAD_CONST, int_const(fnframe, 0), 0);
      emit(fnframe, IRETURN, 0, 1);    
    }

    ana_object *thefn = ana_functionfromframe(fnframe);

    VM_FUNC_DEFN_TRACK(vm, thefn);

    ana_map_put(members, create_symbol(vm, (char *)name->value), thefn);
  }

  ana_object *theclass = ana_class_new(NULL, 
    ana_stringfromstring(classname->value), members);

  VM_FUNC_DEFN_TRACK(vm, theclass);

  ana_map_put(frame->locals, create_symbol(vm, classname->value), theclass);
}

static void compile_try(ComoVM *vm, ana_frame *frame, node *stmt)
{
  assert(stmt->kind == COMO_AST_TRY);
  assert(stmt->nchild == 3);

  node *trybody = stmt->children[0];
  node *exceptionvar = stmt->children[1];
  node *catchbody = stmt->children[2];

  /* place holder */
  ana_array_push(frame->jmptargets, NULL);
  int jmptargetindex_exhandler = ana_array_size(frame->jmptargets) - 1;
  
  ana_array_push(frame->jmptargets, NULL);
  int jmptargetindex_skipexhandler = ana_array_size(frame->jmptargets) - 1;

  emit_xx(frame, TRY, jmptargetindex_exhandler, 0, trybody);
  
  ana_compile_unit(vm, frame, trybody);
  
  emit_xx(frame, JMP, jmptargetindex_skipexhandler, 0, catchbody);
  
  ana_array_push_index(frame->jmptargets, jmptargetindex_exhandler,
    ana_longfromlong((long)ana_container_size(frame->code)));

  assert(exceptionvar->kind == COMO_AST_ID);
  
  char *exceptionvariablename = ((node_id *)exceptionvar)->value;
  
  emit_xx(frame, SETUP_CATCH, str_const(frame, exceptionvariablename), 0, 
    exceptionvar);

  ana_compile_unit(vm, frame, catchbody);

  ana_array_push_index(frame->jmptargets, jmptargetindex_skipexhandler,
    ana_longfromlong((long)ana_container_size(frame->code)));
}

#define SETUP_JMP_TARGET() \
  (ana_array_push(frame->jmptargets, ana_longfromlong(-1L)), ana_array_size(frame->jmptargets) - 1)

/* represent the address of the next instruction */
#define CURRENT_ADDRESS() \
  (ana_longfromlong((long)ana_container_size(frame->code)))

/*
 * this function only gets called for staterments with if else if, else 
 */
static void compile_compound_if(ComoVM *vm, ana_frame *frame, 
  node *ast)
{
  node *expression = ast->children[0];
  node *if_statement = ast->children[1];
  node *else_if_statements = ast->children[2];
  node *else_statement = ast->children[3];
  ana_object *jmp_targets     = ana_array_new(4);
  int exit_address            = SETUP_JMP_TARGET();
  int else_address            = SETUP_JMP_TARGET();
  int first_jmp_address       = SETUP_JMP_TARGET();
  int i;
  ana_object *jmp_map = ana_array_new(4);


  ana_compile_unit(vm, frame, expression);
    /* first_jmp_address is guaranteed to be start of an else if statemet */
    emit_xx(frame, JMPZ, first_jmp_address, 0, expression);
  ana_compile_unit(vm, frame, if_statement);
    emit_xx(frame, JMP, exit_address, 0, if_statement);

  ana_array_push_index(frame->jmptargets,
    first_jmp_address, CURRENT_ADDRESS());

  for(i = 0; i < else_if_statements->nchild; i++)
  {
    node *else_if_statement  = else_if_statements->children[i];
    node *else_if_expression = else_if_statement->children[0];
    node *else_if_body       = else_if_statement->children[1];
  

    if(i != 0)
      ana_array_push(jmp_targets, CURRENT_ADDRESS()); 

    ana_compile_unit(vm, frame, else_if_expression);

    if(else_if_statements->nchild == 1)
    {
      emit_xx(frame, JMPZ, else_address, 0, if_statement);
    }
    else
    {
      if(i + 1 == else_if_statement->nchild)
      {
        /* now we go to the else statement */
        emit_xx(frame, JMPZ, else_address, 0, if_statement); 
      }
      else 
      {
        int addr = SETUP_JMP_TARGET();
        ana_array_push(jmp_map, ana_longfromlong((long)addr));
        emit_xx(frame, JMPZ, addr, 0, if_statement);
      }
    }

    ana_compile_unit(vm, frame, else_if_body);

    emit_xx(frame, JMP, exit_address, 0, if_statement);
  }

  ana_array_push_index(frame->jmptargets,
    else_address, CURRENT_ADDRESS());

  ana_compile_unit(vm, frame, else_statement->children[0]);

  emit_xx(frame, JMP, exit_address, 0, else_statement->children[0]);

  ana_array_push_index(frame->jmptargets,
    exit_address, CURRENT_ADDRESS());


  if(!(ana_array_size(jmp_map) == ana_array_size(jmp_targets)))
  { 
    printf("jmp_map:\n");
    ana_object_print(jmp_map);
    printf("\n");

    printf("jmp_targets\n");
    ana_object_print(jmp_targets);
    printf("\n");
  }

  assert(ana_array_size(jmp_map) == ana_array_size(jmp_targets));

  int current_index = ana_array_size(jmp_targets);

  ana_array_foreach(jmp_map, index, value) {
    (void)index;
    long jmp_target_index = ana_get_long(value)->value;
    ana_get_long(
      ana_get_array(frame->jmptargets)->items[jmp_target_index]
    )->value = ana_get_long(ana_get_array(jmp_targets)->items[--current_index])->value;
  } ana_array_foreach_end();


  /* todo free, jmp_targets, jmp_map */
}

static void compile_if(ComoVM *vm, ana_frame *frame, 
  node *ast)
{
  node *expression = ast->children[0];
  node *body = ast->children[1];
  node *else_statement = NULL;

  /* dead code elimination */
  if(body->nchild == 0)
    return;

  if(ast->nchild == 3) 
  {
    else_statement = ast->children[2];
    assert(else_statement->nchild == 1);
    else_statement = else_statement->children[0];
  }


  ana_array_push(frame->jmptargets, NULL);
  int jmptargetindex_skiphandler = ana_array_size(frame->jmptargets) - 1;

  ana_array_push(frame->jmptargets, NULL);
  int jmptargetindex_pass = ana_array_size(frame->jmptargets) - 1;

  ana_compile_unit(vm, frame, expression);  
  emit_xx(frame, JMPZ, jmptargetindex_skiphandler, 0, body);

  ana_compile_unit(vm, frame, body);
  emit_xx(frame, JMP, jmptargetindex_pass, 0, body);

  ana_size_t start_of_else = ana_container_size(frame->code);

  if(else_statement)
    ana_compile_unit(vm, frame, else_statement);

  ana_array_push_index(frame->jmptargets, jmptargetindex_skiphandler,
    ana_longfromlong((long)start_of_else));

  ana_array_push_index(frame->jmptargets, jmptargetindex_pass,
    ana_longfromlong((long)ana_container_size(frame->code)));
}

static void compile_while(ComoVM *vm, ana_frame *frame, node *ast)
{
  node *expression = ast->children[0];
  node *body = ast->children[1];

  /* while(<expression> is true) { */

  /* this represents the next instruction address */

  ana_array_push(frame->jmptargets, NULL);
  int jmptargetindex_skiphandler = ana_array_size(frame->jmptargets) - 1;
  
  emit_xx(frame, BEGIN_LOOP, 0, 0, body);

  ana_array_push(frame->jmptargets, ana_longfromlong((long)ana_container_size(frame->code)));
  
  int jmptargetindex_start = ana_array_size(frame->jmptargets) - 1;

  ana_compile_unit(vm, frame, expression);  

  emit_xx(frame, JMPZ, jmptargetindex_skiphandler, 0, body);

  ana_compile_unit(vm, frame, body);

  emit_xx(frame, EXIT_LOOP_CONTINUE, 0, 0, body);
  
  emit_xx(frame, JMP, jmptargetindex_start, 0, body);

  ana_array_push_index(frame->jmptargets, jmptargetindex_skiphandler,
    ana_longfromlong((long)ana_container_size(frame->code)));

  emit_xx(frame, END_LOOP, 0, 0, body);
}

static int tracing = 0;
static char *prefix = "";

#define TARGET(kind) case kind: \
  if(tracing) \
    printf("%s%s\n", prefix, astkind(kind)); \

static void ana_compile_unit(ComoVM *vm, ana_frame *frame, 
  node *ast)
{
  switch(ast->kind)
  {
    TARGET(COMO_AST_IMPORT) 
    {
      char *name, *asname;
      node *importlist;

      importlist = ast->children[0];

      if(ast->nchild == 2 && ast->children[1] != NULL)
        asname = ((node_id *)(ast->children[1]))->value;
      else
        asname = NULL;

      int i;
      ana_object *import_tree = ana_array_new(4);
      size_t len = 0;
      size_t pos = 0;
      char *buffer;

      for(i = 0; i < importlist->nchild; i++)
      {
        char *level = ((node_id *)(importlist->children[i]))->value;
        len += strlen(level);  
        ana_array_push(import_tree, ana_stringfromstring(level));
      }

      buffer = malloc(len + importlist->nchild + 1);

      ana_array_foreach(import_tree, index, value) {
        (void)index;
        char *val = ana_cstring(value);        
        memcpy(buffer + pos, val, ana_get_string(value)->len);
        pos += ana_get_string(value)->len;
        
        if(index + 1 != ana_get_array(import_tree)->size)
          *(buffer + pos++) = '.'; 

        ana_object_dtor(value);
      } ana_array_foreach_end();

      buffer[pos] = '\0';

      ana_object_dtor(import_tree);

      name = buffer;
      
      if(asname)
      {
        emitx(frame, LOAD_CONST, str_const(frame, name), 0);
        emitx(frame, IIMPORTAS,  make_symbol(vm, asname), 0);
      }
      else
      {
        emitx(frame, IIMPORT, make_symbol(vm, name), 0);
      }

      free(buffer);
      
      break;
    }
    TARGET(COMO_AST_WHILE) {
      compile_while(vm, frame, ast);
      break;
    }
    TARGET(COMO_AST_UNARY_MINUS) {
      ana_compile_unit(vm, frame, ast->children[0]);
      emitx(frame, IUNARYMINUS, 0, 0);
      break;  
    }
    TARGET(COMO_AST_UNARY_PLUS) {
      ana_compile_unit(vm, frame, ast->children[0]);
      emitx(frame, IUNARYPLUS, 0, 0);
      break;  
    }
    TARGET(COMO_AST_UNARY_NOT) {
      ana_compile_unit(vm, frame, ast->children[0]);
      emitx(frame, IUNARYNOT, 0, 0);
      break;  
    }
    TARGET(COMO_AST_THROW) {
      ana_compile_unit(vm, frame, ast->children[0]);
      emitx(frame, ITHROW, 0, 0);  
      break;
    }
    TARGET(COMO_AST_IF)
      compile_if(vm, frame, ast);
      break;
    TARGET(COMO_AST_COMPOUND_IF_STATEMENT)
      compile_compound_if(vm, frame, ast);
      break;
    TARGET(COMO_AST_TRY) {
      compile_try(vm, frame, ast);
      break;
    }
    TARGET(COMO_AST_INDEX) {
      ana_compile_unit(vm, frame, ast->children[0]);
      ana_compile_unit(vm, frame, ast->children[1]);
      emitx(frame, LOAD_SUBSCRIPT, 0, 0);  
      break;
    }
    TARGET(COMO_AST_OBJECT) {
      node *keyvpair_list = ast->children[0];
      
      int start = keyvpair_list->nchild;

      while(start--)
      {
        node *n = keyvpair_list->children[start];
        node *key = n->children[0];
        node *value = n->children[1];

        ana_compile_unit(vm, frame, key);
        ana_compile_unit(vm, frame, value);
      }

      emitx(frame, INITOBJ, keyvpair_list->nchild, 0); 
      break;
    }
    TARGET(COMO_AST_ARRAY) {
      int start = ast->children[0]->nchild;
      while(start--)
      {
        ana_compile_unit(vm, frame, ast->children[0]->children[start]);
      }
      emitx(frame, INITARRAY, ast->children[0]->nchild, 0);
      break;
    }
    TARGET(COMO_AST_PROP) {
      ana_compile_unit(vm, frame, ast->children[0]);
      char *id = ((node_id *)ast->children[1])->value;
      emitx(frame, GETPROP, make_symbol(vm, id), 0);
      break;
    }
    TARGET(COMO_AST_LIST) {
      int i;
      for(i = 0; i < ast->nchild; i++)
        ana_compile_unit(vm, frame, ast->children[i]);
      break;
    }
    TARGET(COMO_AST_CALL) {
      node *callable_expression = ast->children[0];
      node *args = ast->children[1];
      int callflags = 0;
      int i;
      for(i = 0; i < args->nchild; i++) {
        ana_compile_unit(vm, frame, args->children[i]);
      }

      ana_compile_unit(vm, frame, callable_expression);
      emitx(frame, CALL, args->nchild, callflags);
      break;
    }
    TARGET(COMO_AST_INT) {
      node_int *n = (node_int *)ast;
      emitx(frame, LOAD_CONST, int_const(frame, n->value), 0);
      break;
    }
    TARGET(COMO_AST_TRUE) {
      emitx(frame, LOAD_CONST, bool_const(frame, 1), 0);
      break;
    }
    TARGET(COMO_AST_FALSE) {
      emitx(frame, LOAD_CONST, bool_const(frame, 0), 0);
      break;
    }
    TARGET(COMO_AST_DOUBLE) {
      node_double *n = (node_double *)ast;
      emitx(frame, LOAD_CONST, double_const(frame, n->value), 0);
      break;
    }
    TARGET(COMO_AST_RETURN) {
      int retvalused = ast->nchild == 1;

      if(retvalused) {
        ana_compile_unit(vm, frame, ast->children[0]);
      }
      else
        emitx(frame, LOAD_CONST, int_const(frame, 0), 0);

      emitx(frame, IRETURN, 0, 1);
      break;
    }
    TARGET(COMO_AST_ID) {
      char *id = ((node_id *)ast)->value;
      emitx(frame, LOAD_NAME, make_symbol(vm, id), 0);
      break;
    }
    TARGET(COMO_AST_STRING) {
      char *id = ((node_id *)ast)->value;
      emitx(frame, LOAD_CONST, str_const(frame, id), 0);
      break;
    }
    TARGET(COMO_AST_FUNCTION) 
      compile_func(vm, frame, ast);
      break;
    TARGET(COMO_AST_CLASS)
      compile_class(vm, frame, ast);
      break;
    TARGET(COMO_AST_BINOP) {
      prefix = "    ";
      switch(ast->attributes) {
        TARGET(COMO_AST_IN) {
          ana_compile_unit(vm, frame, ast->children[0]);
          ana_compile_unit(vm, frame, ast->children[1]);
          emitx(frame, IIN, 0, 0);
          break;
        }
        TARGET(COMO_AST_REM) {
          ana_compile_unit(vm, frame, ast->children[0]);
          ana_compile_unit(vm, frame, ast->children[1]);
          emitx(frame, IREM, 0, 0);
          break;         
        }
        TARGET(COMO_AST_LTE) {
          ana_compile_unit(vm, frame, ast->children[0]);
          ana_compile_unit(vm, frame, ast->children[1]);
          emitx(frame, ILTE, 0, 0);
          break;
        }
        TARGET(COMO_AST_EQUAL) {
          ana_compile_unit(vm, frame, ast->children[0]);
          ana_compile_unit(vm, frame, ast->children[1]);
          emitx(frame, IEQUAL, 0, 0);
          break;
        }
        TARGET(COMO_AST_NOT_EQUAL) {
          ana_compile_unit(vm, frame, ast->children[0]);
          ana_compile_unit(vm, frame, ast->children[1]);
          emitx(frame, INEQUAL, 0, 0);
          break;
        }
        TARGET(COMO_AST_DIV) {
          ana_compile_unit(vm, frame, ast->children[0]);
          ana_compile_unit(vm, frame, ast->children[1]);
          emitx(frame, IDIV, 0, 0);
          break;
        }
        TARGET(COMO_AST_ADD)
          ana_compile_unit(vm, frame, ast->children[0]);
          ana_compile_unit(vm, frame, ast->children[1]);
          emitx(frame, IADD, 0, 0);
          break;
        TARGET(COMO_AST_SUB)
          ana_compile_unit(vm, frame, ast->children[0]);
          ana_compile_unit(vm, frame, ast->children[1]);
          emitx(frame, IMINUS, 0, 0);   
          break;  
        TARGET(COMO_AST_ASSIGN) {
          /* Check property */
          node *left = ast->children[0];

          if(left->kind == COMO_AST_PROP)
          {
            /* GETPROP, with flag set, */
            ana_compile_unit(vm, frame, left->children[0]);
            char *id = ((node_id *)left->children[1])->value;

            ana_compile_unit(vm, frame, ast->children[1]);
            emitx(frame, SETPROP, make_symbol(vm, id), 1);
          }
          else if(left->kind == COMO_AST_INDEX)
          {
            ana_compile_unit(vm, frame, left->children[0]); /* the container */
            ana_compile_unit(vm, frame, left->children[1]); /* the index */

            ana_compile_unit(vm, frame, ast->children[1]);  /* the value */
            emitx(frame, STORE_SUBSCRIPT, 0, 0);
          }
          else 
          {
            assert(left->kind == COMO_AST_ID);
            node_id *name = (node_id *)ast->children[0];
            ana_compile_unit(vm, frame, ast->children[1]);
            emitx(frame, STORE_NAME, make_symbol(vm, name->value), 0);
          }
          break;
        }
        TARGET(COMO_AST_MUL) {
          ana_compile_unit(vm, frame, ast->children[0]);
          ana_compile_unit(vm, frame, ast->children[1]);
          emitx(frame, ITIMES, 0, 0);
          break;
        }
      } 
      prefix = "";
      break;
    }

    default:
      break;
  }
}

static void init_builtins(ComoVM *vm, ana_frame *frame)
{
  ana_size_t readline_symbol_index = make_symbol(vm, "readline");
  ana_object *readline_symbol 
  = ana_get_array(vm->symbols)->items[readline_symbol_index];

  ana_object *fn = ana_functionfromhandler(ana__builtin_readline);
  fn->scope = ana_stringfromstring(ana_get_base(frame)->scope ? 
    ana_cstring(ana_get_base(frame)->scope) : "__main__");

  VM_FUNC_DEFN_TRACK(vm, fn);

  ana_map_put(frame->locals, readline_symbol, fn);

  ana_size_t print_symbol_index = make_symbol(vm, "print");
  ana_object *print_symbol = 
    ana_get_array(vm->symbols)->items[print_symbol_index];

  fn = ana_functionfromhandler(ana__builtin_print);
  fn->scope = ana_stringfromstring(ana_get_base(frame)->scope ? 
    ana_cstring(ana_get_base(frame)->scope) : "__main__");

  VM_FUNC_DEFN_TRACK(vm, fn);

  ana_map_put(frame->locals, print_symbol, fn);
}

ana_object *ana_compileast(char *filename, ComoVM *vm, node *ast)
{
  ana_bool_type_init();
  
  assert(ast->kind == COMO_AST_PROGRAM);

  ana_object *mainframe = ana_frame_new("__main__", filename, NULL);
  
  mainframe->type->obj_init(mainframe);

  ana_map_put(((ana_frame *)mainframe)->locals, 
    create_symbol(vm, "__name__"), ((ana_frame *)mainframe)->name);

  init_builtins(vm, (ana_frame *)mainframe);

  int i;

  for(i = 0; i < ast->nchild; i++)
    ana_compile_unit(vm, (ana_frame *)mainframe, ast->children[i]);

  emit((ana_frame *)mainframe, LOAD_CONST, 
    int_const(((ana_frame *)mainframe), 0), 0);

  emit((ana_frame *)mainframe, IRETURN, 0, 0);

  return mainframe;
}

ana_object *ana_compilemodule(char *modulename, char *filename, 
  ComoVM *vm, node *ast)
{
  ana_object *mainframe = ana_frame_new(modulename, filename, NULL);
  
  mainframe->type->obj_init(mainframe);

  ana_map_put(((ana_frame *)mainframe)->locals, create_symbol(vm, "__name__"), 
    ((ana_frame *)mainframe)->name);
  
  char *fnname = ana_get_fn_name((ana_frame *)mainframe);
  ana_get_base(mainframe)->scope = ana_stringfromstring(fnname);
  free(fnname);

  init_builtins(vm, (ana_frame *)mainframe);

  int i;

  for(i = 0; i < ast->nchild; i++) 
  {
    ana_compile_unit(vm, (ana_frame *)mainframe, ast->children[i]);
  }

  emit((ana_frame *)mainframe, LOAD_CONST, 
    int_const(((ana_frame *)mainframe), 0), 0);

  emit((ana_frame *)mainframe, IRETURN, 0, 0);

  return mainframe;
}
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

#define SETUP_JMP_TARGET() \
  (ana_array_push(func->jump_targets, ana_longfromlong(-1L)), ana_array_size(func->jump_targets) - 1)

#define DEFINE_JUMP() \
  (ana_array_push(func->jump_targets, CURRENT_ADDRESS(), ana_array_size(func->jump_targets) - 1)

/* represent the address of the next instruction */
#define CURRENT_ADDRESS() \
  (ana_longfromlong((long)ana_container_size(func->code)))

#define EMIT(func, opcode, arg, flag) \
  ana_code_push(func->code, PACK_INSTRUCTION(opcode, arg, flag));

#define NEW_INT_CONST(vm, value) \
  (ana_array_push(vm->constants, ana_longfromlong((long)(value))), ana_array_size(vm->constants) - 1)

#define NEW_STR_CONST(vm, value) \
  (ana_array_push(vm->constants, ana_stringfromstring(value)), (ana_array_size(vm->constants) - 1))

#define NEW_FUNCTION_CONST(vm, value) \
  (ana_array_push(vm->constants, (value)), (ana_array_size(vm->constants) - 1))

#define NEW_BOOL_CONST(vm, value) \
  (ana_array_push(vm->constants, ana_boolfromint(value)), (ana_array_size(vm->constants) - 1))

#define NEW_DOUBLE_CONST(vm, value) \
  (ana_array_push(vm->constants, ana_doublefromdouble(value)), ana_array_size(vm->constants) - 1)

#define EMITX(vm, func, opcode, arg, flag, ast) do { \
  EMIT(func, opcode, arg, flag); \
  ana_map_put(func->line_mapping, \
      ana_longfromlong(ana_container_size(func->code)), \
      ana_longfromlong((long)ast->line) \
  ); \
} while(0)


static void ana_compile_return_statement(ana_vm *vm, ana_function_defn *func);
static void ana_compile_unit(ana_vm *vm, ana_object *funcobj, node *ast);

static void compile_func(ana_vm *vm, ana_object *parentfuncobj, node *ast)
{
  ana_function_defn *parentfunc = ANA_GET_FUNCTION_DEF(parentfuncobj);
  node_id *name = (node_id *)(ast->children[0]);
  node *params = ast->children[1];
  node *body = ast->children[2];
  int i;
  ana_object *funcobj = ana_function_defn_new(
    ana_cstring(ana_get_function(parentfuncobj)->filename),
    name->value
  );
  ana_function_defn *func = ANA_GET_FUNCTION_DEF(funcobj);

  for(i = 0; i < params->nchild; i++)
  {
    node *node = params->children[i];

    assert(node->kind == COMO_AST_ID);

    node_id *idnode = (node_id *)node;

    ana_array_push(func->parameters, ana_stringfromstring(idnode->value));
  }

  ana_compile_unit(vm, funcobj, body);

  if(body->nchild > 0) 
  {
    if(body->children[body->nchild - 1]->kind != COMO_AST_RETURN) 
    {
      ana_compile_return_statement(vm, func);
    }
  } 
  else
  {
    ana_compile_return_statement(vm, func); 
  }

  /* put this object in the constant table */

  EMITX(vm, parentfunc, LOAD_CONST, NEW_FUNCTION_CONST(vm, funcobj), 0, ast);
  EMITX(vm, parentfunc, LOAD_CONST, NEW_STR_CONST(vm, name->value), 0,  ast);
  EMITX(vm, parentfunc, DEFINE_FUNCTION, 0, 0, ast);
}

static void compile_class(ana_vm *vm, ana_object *parentfuncobj, node *ast)
{
  ana_function_defn *parentfunc = ANA_GET_FUNCTION_DEF(parentfuncobj);
  node_id *classname = (node_id *)(ast->children[0]);
  node *functions = ast->children[1];
  ana_size_t i;
  
  for(i = 0; i < functions->nchild; i++)
  {
    node *classmethod = functions->children[i];
    node *function = classmethod->children[0];    

    node_id *name = (node_id *)(function->children[0]);
    node *params = function->children[1];
    node *body = function->children[2];

    ana_object *funcobj = ana_function_defn_new(
      ana_cstring(ana_get_function(parentfuncobj)->filename),
      name->value
    );

    ana_function_defn *func = ANA_GET_FUNCTION_DEF(funcobj);

    for(i = 0; i < params->nchild; i++)
    {
      node *node = params->children[i];

      assert(node->kind == COMO_AST_ID);

      node_id *idnode = (node_id *)node;

      ana_array_push(func->parameters, ana_stringfromstring(idnode->value));
    }

    ana_compile_unit(vm, funcobj, body);

    if(body->nchild > 0) 
    {
      if(body->children[body->nchild - 1]->kind != COMO_AST_RETURN) 
      {
        ana_compile_return_statement(vm, func);
      }
    } 
    else
    {
      ana_compile_return_statement(vm, func); 
    }

    EMITX(vm, parentfunc, LOAD_CONST, NEW_FUNCTION_CONST(vm, funcobj), 0, ast);
    EMITX(vm, parentfunc, LOAD_CONST, NEW_STR_CONST(vm, name->value), 0,  ast);
  }

  EMITX(vm, parentfunc, LOAD_CONST, NEW_INT_CONST(vm, functions->nchild), 0, ast);
  EMITX(vm, parentfunc, LOAD_CONST, NEW_STR_CONST(vm, classname->value), 0,  ast);
  EMITX(vm, parentfunc, DEFINE_CLASS, 0, 0, ast);
}

static void compile_try(ana_vm *vm, ana_object *parentfuncobj, node *stmt)
{
  ana_function_defn *parentfunc = ANA_GET_FUNCTION_DEF(parentfuncobj);

  assert(stmt->kind == COMO_AST_TRY);
  assert(stmt->nchild == 3);

  node *trybody = stmt->children[0];
  node *exceptionvar = stmt->children[1];
  node *catchbody = stmt->children[2];

  /* place holder */
  ana_array_push(parentfunc->jump_targets, NULL);
  int jmptargetindex_exhandler = ana_array_size(parentfunc->jump_targets) - 1;
  
  ana_array_push(parentfunc->jump_targets, NULL);
  int jmptargetindex_skipexhandler = ana_array_size(parentfunc->jump_targets) - 1;

  EMITX(vm, parentfunc, TRY, jmptargetindex_exhandler, 0, trybody);
  
  ana_compile_unit(vm, parentfuncobj, trybody);
  
  EMITX(vm, parentfunc, JMP, jmptargetindex_skipexhandler, 0, catchbody);
  
  ana_array_push_index(parentfunc->jump_targets, jmptargetindex_exhandler,
    ana_longfromlong((long)ana_container_size(parentfunc->code)));

  assert(exceptionvar->kind == COMO_AST_ID);
  
  char *exceptionvariablename = ((node_id *)exceptionvar)->value;
  
  EMITX(vm, parentfunc, SETUP_CATCH, NEW_STR_CONST(vm, exceptionvariablename), 0, 
    exceptionvar);

  ana_compile_unit(vm, parentfuncobj, catchbody);

  ana_array_push_index(parentfunc->jump_targets, jmptargetindex_skipexhandler,
    ana_longfromlong((long)ana_container_size(parentfunc->code)));
}

/*
 * this function only gets called for staterments with if else if, else 
 */
static void compile_compound_if(ana_vm *vm, ana_object *funcobj, 
  node *ast)
{
  ana_function_defn *func = ANA_GET_FUNCTION_DEF(funcobj);
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

  ana_compile_unit(vm, funcobj, expression);
    /* first_jmp_address is guaranteed to be start of an else if statemet */
    EMITX(vm, func, JMPZ, first_jmp_address, 0, expression);
  ana_compile_unit(vm, funcobj, if_statement);
    EMITX(vm, func, JMP, exit_address, 0, if_statement);

  ana_array_push_index(func->jump_targets,
    first_jmp_address, CURRENT_ADDRESS());

  for(i = 0; i < else_if_statements->nchild; i++)
  {
    node *else_if_statement  = else_if_statements->children[i];
    node *else_if_expression = else_if_statement->children[0];
    node *else_if_body       = else_if_statement->children[1];
  

    if(i != 0)
      ana_array_push(jmp_targets, CURRENT_ADDRESS()); 

    ana_compile_unit(vm, funcobj, else_if_expression);

    if(else_if_statements->nchild == 1)
    {
      EMITX(vm, func, JMPZ, else_address, 0, if_statement);
    }
    else
    {
      if(i + 1 == else_if_statement->nchild)
      {
        /* now we go to the else statement */
        EMITX(vm, func, JMPZ, else_address, 0, if_statement); 
      }
      else 
      {
        int addr = SETUP_JMP_TARGET();
        ana_array_push(jmp_map, ana_longfromlong((long)addr));
        EMITX(vm, func, JMPZ, addr, 0, if_statement);
      }
    }

    ana_compile_unit(vm, funcobj, else_if_body);

    EMITX(vm, func, JMP, exit_address, 0, if_statement);
  }

  ana_array_push_index(func->jump_targets,
    else_address, CURRENT_ADDRESS());

  ana_compile_unit(vm, funcobj, else_statement->children[0]);

  EMITX(vm, func, JMP, exit_address, 0, else_statement->children[0]);

  ana_array_push_index(func->jump_targets,
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
      ana_get_array(func->jump_targets)->items[jmp_target_index]
    )->value = ana_get_long(ana_get_array(jmp_targets)->items[--current_index])->value;
  } ana_array_foreach_end();


  /* todo free, jmp_targets, jmp_map */
}

static void compile_if(ana_vm *vm, ana_object *funcobj, 
  node *ast)
{
  ana_function_defn *func = ANA_GET_FUNCTION_DEF(funcobj);
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


  ana_array_push(func->jump_targets, NULL);
  int jmptargetindex_skiphandler = ana_array_size(func->jump_targets) - 1;

  ana_array_push(func->jump_targets, NULL);
  int jmptargetindex_pass = ana_array_size(func->jump_targets) - 1;

  ana_compile_unit(vm, funcobj, expression);  
  EMITX(vm, func, JMPZ, jmptargetindex_skiphandler, 0, body);

  ana_compile_unit(vm, funcobj, body);
  EMITX(vm, func, JMP, jmptargetindex_pass, 0, body);

  ana_size_t start_of_else = ana_container_size(func->code);

  if(else_statement)
    ana_compile_unit(vm, funcobj, else_statement);

  ana_array_push_index(func->jump_targets, jmptargetindex_skiphandler,
    ana_longfromlong((long)start_of_else));

  ana_array_push_index(func->jump_targets, jmptargetindex_pass,
    ana_longfromlong((long)ana_container_size(func->code)));
}

static void compile_while(ana_vm *vm, ana_object *funcobj, node *ast)
{
  ana_function_defn *func = ANA_GET_FUNCTION_DEF(funcobj);

  node *expression = ast->children[0];
  node *body = ast->children[1];

  /* while(<expression> is true) { */

  /* this represents the next instruction address */
  ana_array_push(func->jump_targets, NULL);
  int jmptargetindex_skiphandler = ana_array_size(func->jump_targets) - 1;
  
  EMITX(vm, func, BEGIN_LOOP, 0, 0, body);

  ana_array_push(func->jump_targets, 
    ana_longfromlong((long)ana_container_size(func->code))); 

  int jmptargetindex_start = ana_array_size(func->jump_targets) - 1;

  ana_compile_unit(vm, funcobj, expression);  

  EMITX(vm, func, JMPZ, jmptargetindex_skiphandler, 0, body);

  ana_compile_unit(vm, funcobj, body);

  EMITX(vm, func, EXIT_LOOP_CONTINUE, 0, 0, body);
  
  EMITX(vm, func, JMP, jmptargetindex_start, 0, body);

  ana_array_push_index(func->jump_targets, jmptargetindex_skiphandler,
    ana_longfromlong((long)ana_container_size(func->code)));

  EMITX(vm, func, END_LOOP, 0, 0, body);
}

static void compile_for(ana_vm *vm, ana_object *funcobj, node *ast)
{
  ana_function_defn *func = ANA_GET_FUNCTION_DEF(funcobj);

  /* expression executed only once */
  node *init = ast->children[0];

  /* before start of the loop body, if evalatues to false, jump out*/
  node *condition = ast->children[1];

  /* at the end of the block for every iteration this is executed */
  node *loop = ast->children[2];

  node *statements = ast->children[3];

  ana_array_push(func->jump_targets, NULL);
  int jmptargetindex_skiphandler = ana_array_size(func->jump_targets) - 1;

  ana_compile_unit(vm, funcobj, init);

  EMITX(vm, func, BEGIN_LOOP, 0, 0, init);

  ana_array_push(func->jump_targets, 
    ana_longfromlong((long)ana_container_size(func->code))); 

  int jmptargetindex_start = ana_array_size(func->jump_targets) - 1;

  ana_compile_unit(vm, funcobj, condition);

  EMITX(vm, func, JMPZ, jmptargetindex_skiphandler, 0, condition);

  ana_compile_unit(vm, funcobj, statements);

  ana_compile_unit(vm, funcobj, loop);

  EMITX(vm, func, EXIT_LOOP_CONTINUE, 0, 0, statements);

  EMITX(vm, func, JMP, jmptargetindex_start, 0, loop);

  ana_array_push_index(func->jump_targets, jmptargetindex_skiphandler,
    ana_longfromlong((long)ana_container_size(func->code)));

  EMITX(vm, func, END_LOOP, 0, 0, statements);
}

static int tracing = 0;
static char *prefix = "";

#define TARGET(kind) case kind: \
  if(tracing) \
    printf("%s%s\n", prefix, astkind(kind)); \

static void ana_compile_unit(ana_vm *vm, ana_object *funcobj, 
  node *ast)
{
  ana_function_defn *func = ANA_GET_FUNCTION_DEF(funcobj);

  switch(ast->kind)
  {
    TARGET(COMO_AST_FOR)
    {
      compile_for(vm, funcobj, ast);
      break;
    }
    TARGET(COMO_AST_PROGRAM)
    {
      int i;

      for(i = 0; i < ast->nchild; i++)
      {
        ana_compile_unit(vm, funcobj, ast->children[i]);
      }

      break;
    }

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
        EMITX(vm, func, LOAD_CONST, NEW_STR_CONST(vm, name), 0,  ast);
        EMITX(vm, func, IIMPORTAS,  NEW_SYMBOL(vm, asname),  0, ast);
      }
      else
      {
        EMITX(vm, func, IIMPORT, NEW_SYMBOL(vm, name), 0, ast);
      }

      free(buffer);
      
      break;
    }
    TARGET(COMO_AST_WHILE) {
      compile_while(vm, funcobj, ast);
      break;
    }
    TARGET(COMO_AST_UNARY_MINUS) {
      ana_compile_unit(vm, funcobj, ast->children[0]);
      EMITX(vm, func, IUNARYMINUS, 0, 0, ast);
      break;  
    }
    TARGET(COMO_AST_UNARY_PLUS) {
      ana_compile_unit(vm, funcobj, ast->children[0]);
      EMITX(vm, func, IUNARYPLUS, 0, 0, ast);
      break;  
    }
    TARGET(COMO_AST_UNARY_NOT) {
      ana_compile_unit(vm, funcobj, ast->children[0]);
      EMITX(vn, func, IUNARYNOT, 0, 0, ast);
      break;  
    }
    TARGET(COMO_AST_THROW) {
      ana_compile_unit(vm, funcobj, ast->children[0]);
      EMITX(vm, func, ITHROW, 0, 0, ast);  
      break;
    }
    TARGET(COMO_AST_IF)
      compile_if(vm, funcobj, ast);
      break;
    TARGET(COMO_AST_COMPOUND_IF_STATEMENT)
      compile_compound_if(vm, funcobj, ast);
      break;
    TARGET(COMO_AST_TRY) {
      compile_try(vm, funcobj, ast);
      break;
    }
    TARGET(COMO_AST_INDEX) {
      ana_compile_unit(vm, funcobj, ast->children[0]);
      ana_compile_unit(vm, funcobj, ast->children[1]);
      EMITX(vn, func, LOAD_SUBSCRIPT, 0, 0, ast);  
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

        ana_compile_unit(vm, funcobj, key);
        ana_compile_unit(vm, funcobj, value);
      }

      EMITX(vm, func, INITOBJ, keyvpair_list->nchild, 0, ast); 
      break;
    }
    TARGET(COMO_AST_ARRAY) {
      int start = ast->children[0]->nchild;
      while(start--)
      {
        ana_compile_unit(vm, funcobj, ast->children[0]->children[start]);
      }
      EMITX(vm, func, INITARRAY, ast->children[0]->nchild, 0, ast->children[0]);
      break;
    }
    TARGET(COMO_AST_PROP) {
      ana_compile_unit(vm, funcobj, ast->children[0]);
      char *id = ((node_id *)ast->children[1])->value;
      EMITX(vm, func, GETPROP, NEW_SYMBOL(vm, id), 0, ast);
      break;
    }
    TARGET(COMO_AST_LIST) {
      int i;
      for(i = 0; i < ast->nchild; i++)
        ana_compile_unit(vm, funcobj, ast->children[i]);
      break;
    }
    TARGET(COMO_AST_CALL) {
      node *callable_expression = ast->children[0];
      node *args = ast->children[1];
      int callflags = 0;
      int i;
      
      for(i = 0; i < args->nchild; i++) {
        ana_compile_unit(vm, funcobj, args->children[i]);
      }

      /* check if this is DOt notation */

      if(callable_expression->kind == COMO_AST_PROP)
      {
        ana_compile_unit(vm, funcobj, callable_expression->children[0]);
        ana_compile_unit(vm, funcobj, callable_expression);   
        EMITX(vm, func, CALL_METHOD, args->nchild, callflags, callable_expression);
      } 
      else 
      {
        ana_compile_unit(vm, funcobj, callable_expression);   
        EMITX(vm, func, CALL, args->nchild, callflags, callable_expression);
      }
      
      break;
    }
    TARGET(COMO_AST_INT) {
      node_int *n = (node_int *)ast;
      EMITX(vm, func, LOAD_CONST, NEW_INT_CONST(vm, n->value), 0, ast);
      break;
    }
    TARGET(COMO_AST_TRUE) {
      EMITX(vm, func, LOAD_CONST, NEW_BOOL_CONST(vm, 1), 0, ast);
      break;
    }
    TARGET(COMO_AST_FALSE) {
      EMITX(vm, func, LOAD_CONST, NEW_BOOL_CONST(vm, 0), 0, ast);
      break;
    }
    TARGET(COMO_AST_DOUBLE) {
      node_double *n = (node_double *)ast;
      EMITX(vm, func, LOAD_CONST, NEW_DOUBLE_CONST(vm, n->value), 0, ast);
      break;
    }
    TARGET(COMO_AST_RETURN) {
      int retvalused = ast->nchild == 1;

      if(retvalused) {
        ana_compile_unit(vm, funcobj, ast->children[0]);
      }
      else
        EMITX(vm, func, LOAD_CONST, NEW_INT_CONST(vm, 0), 0, ast);

      EMITX(vm, func, IRETURN, 0, 1, ast);
      break;
    }
    TARGET(COMO_AST_ID) {
      char *id = ((node_id *)ast)->value;
      EMITX(vm, func, LOAD_NAME, NEW_SYMBOL(vm, id), 0, ast);
      break;
    }
    TARGET(COMO_AST_STRING) {
      char *id = ((node_id *)ast)->value;
      EMITX(vm, func, LOAD_CONST, NEW_STR_CONST(vm, id), 0, ast);
      break;
    }
    TARGET(COMO_AST_FUNCTION) 
      compile_func(vm, funcobj, ast);
      break;
    TARGET(COMO_AST_CLASS)
      compile_class(vm, funcobj, ast);
      break;
    TARGET(COMO_AST_BINOP) {
      prefix = "    ";
      switch(ast->attributes) {
        TARGET(COMO_AST_LEFT_SHIFT) {
          ana_compile_unit(vm, funcobj, ast->children[0]);
          ana_compile_unit(vm, funcobj, ast->children[1]);
          EMITX(vm, func, ILSHFT, 0, 0, ast);
          break;
        }
        TARGET(COMO_AST_RIGHT_SHIFT) {
          ana_compile_unit(vm, funcobj, ast->children[0]);
          ana_compile_unit(vm, funcobj, ast->children[1]);
          EMITX(vm, func, IRSHFT, 0, 0, ast);
          break;
        }
        TARGET(COMO_AST_IN) {
          ana_compile_unit(vm, funcobj, ast->children[0]);
          ana_compile_unit(vm, funcobj, ast->children[1]);
          EMITX(vm, func, IIN, 0, 0, ast);
          break;
        }
        TARGET(COMO_AST_REM) {
          ana_compile_unit(vm, funcobj, ast->children[0]);
          ana_compile_unit(vm, funcobj, ast->children[1]);
          EMITX(vm, func, IREM, 0, 0, ast);
          break;         
        }
        TARGET(COMO_AST_LTE) {
          ana_compile_unit(vm, funcobj, ast->children[0]);
          ana_compile_unit(vm, funcobj, ast->children[1]);
          EMITX(vm, func, ILTE, 0, 0, ast);
          break;
        }

        TARGET(COMO_AST_GTE) {
          ana_compile_unit(vm, funcobj, ast->children[0]);
          ana_compile_unit(vm, funcobj, ast->children[1]);
          EMITX(vm, func, IGTE, 0, 0, ast);
          break;
        }

        TARGET(COMO_AST_LT) {
          ana_compile_unit(vm, funcobj, ast->children[0]);
          ana_compile_unit(vm, funcobj, ast->children[1]);
          EMITX(vm, func, ILT, 0, 0, ast);
          break;
        }

        TARGET(COMO_AST_GT) {
          ana_compile_unit(vm, funcobj, ast->children[0]);
          ana_compile_unit(vm, funcobj, ast->children[1]);
          EMITX(vm, func, IGT, 0, 0, ast);
          break;
        }

        TARGET(COMO_AST_EQUAL) {
          ana_compile_unit(vm, funcobj, ast->children[0]);
          ana_compile_unit(vm, funcobj, ast->children[1]);
          EMITX(vm, func, IEQUAL, 0, 0, ast);
          break;
        }
        TARGET(COMO_AST_NOT_EQUAL) {
          ana_compile_unit(vm, funcobj, ast->children[0]);
          ana_compile_unit(vm, funcobj, ast->children[1]);
          EMITX(vm, func, INEQUAL, 0, 0, ast);
          break;
        }
        TARGET(COMO_AST_DIV) {
          ana_compile_unit(vm, funcobj, ast->children[0]);
          ana_compile_unit(vm, funcobj, ast->children[1]);
          EMITX(vm, func, IDIV, 0, 0, ast);
          break;
        }
        TARGET(COMO_AST_ADD)
          ana_compile_unit(vm, funcobj, ast->children[0]);
          ana_compile_unit(vm, funcobj, ast->children[1]);
          EMITX(vm, func, IADD, 0, 0, ast);
          break;
        TARGET(COMO_AST_SUB)
          ana_compile_unit(vm, funcobj, ast->children[0]);
          ana_compile_unit(vm, funcobj, ast->children[1]);
          EMITX(vm, func, IMINUS, 0, 0, ast);
          break;  
        TARGET(COMO_AST_ASSIGN) {
          /* Check property */
          node *left = ast->children[0];

          if(left->kind == COMO_AST_PROP)
          {
            /* GETPROP, with flag set, */
            ana_compile_unit(vm, funcobj, left->children[0]);

            char *id = ((node_id *)left->children[1])->value;

            ana_compile_unit(vm, funcobj, ast->children[1]);

            EMITX(vm, func, SETPROP, NEW_SYMBOL(vm, id), 1, ast);
          }
          else if(left->kind == COMO_AST_INDEX)
          {
            ana_compile_unit(vm, funcobj, left->children[0]); /* the container */
            ana_compile_unit(vm, funcobj, left->children[1]); /* the index */
            ana_compile_unit(vm, funcobj, ast->children[1]);  /* the value */
            EMITX(vm, func, STORE_SUBSCRIPT, 0, 0, ast);
          }
          else 
          {
            assert(left->kind == COMO_AST_ID);
            node_id *name = (node_id *)ast->children[0];

            ana_compile_unit(vm, funcobj, ast->children[1]);

            EMITX(vm, func, STORE_NAME, NEW_SYMBOL(vm, name->value), 0, ast);
          }
          break;
        }
        TARGET(COMO_AST_MUL) {
          ana_compile_unit(vm, funcobj, ast->children[0]);
          ana_compile_unit(vm, funcobj, ast->children[1]);
          EMITX(vm, func, ITIMES, 0, 0, ast);
          break;
        }
      } 
      prefix = "";
      break;
    }

    TARGET(COMO_AST_PREFIXINC) 
    {
      node *left = ast->children[0];

      if(left->kind == COMO_AST_PROP)
      {
        // for GETPROP, container is on stack, property is an index on the
        // opline

        /* compile the container */
        ana_compile_unit(vm, funcobj, left->children[0]);

        char *id = ((node_id *)left->children[1])->value;
       
        /* compile to GETPROP */ 
        ana_compile_unit(vm, funcobj, left);
        
        EMITX(vm, func, LOAD_CONST, NEW_INT_CONST(vm, 1), 1, left);
        EMITX(vm, func, IADD,       0,                    0, left);
        EMITX(vm, func, SETPROP, NEW_SYMBOL(vm, id), 1, ast);
      }
      else if(left->kind == COMO_AST_INDEX)
      {

        ana_compile_unit(vm, funcobj, left->children[0]); /* the container */
        ana_compile_unit(vm, funcobj, left->children[1]); /* the index */
        /* this will compile into LOAD_SUBSCRIPT */
        ana_compile_unit(vm, funcobj, left);

        EMITX(vm, func, LOAD_CONST, NEW_INT_CONST(vm, 1), 1, left);
        EMITX(vm, func, IADD,       0,                    0, left);
        EMITX(vm, func, STORE_SUBSCRIPT, 0, 0, ast);
      }
      else 
      {
        assert(left->kind == COMO_AST_ID);

        node_id *name = (node_id *)left;

        EMITX(vm, func, LOAD_NAME,  NEW_SYMBOL(vm, name->value), 1, left);
        EMITX(vm, func, LOAD_NAME,  NEW_SYMBOL(vm, name->value), 1, left);
        EMITX(vm, func, LOAD_CONST, NEW_INT_CONST(vm, 1), 1, left);
        EMITX(vm, func, IADD,     0,                    0, left);
        EMITX(vm, func, STORE_NAME, NEW_SYMBOL(vm, name->value), 0, left);
      }

      break;
    }

    TARGET(COMO_AST_PREFIXDEC)
    {
      node *left = ast->children[0];

      if(left->kind == COMO_AST_PROP)
      {
        // for GETPROP, container is on stack, property is an index on the
        // opline

        /* compile the container */
        ana_compile_unit(vm, funcobj, left->children[0]);

        char *id = ((node_id *)left->children[1])->value;
       
        /* compile to GETPROP */ 
        ana_compile_unit(vm, funcobj, left);
        
        EMITX(vm, func, LOAD_CONST, NEW_INT_CONST(vm, 1), 1, left);
        EMITX(vm, func, IMINUS,       0,                  0, left);
        EMITX(vm, func, SETPROP,    NEW_SYMBOL(vm, id),   1, ast);
      }
      else if(left->kind == COMO_AST_INDEX)
      {

        ana_compile_unit(vm, funcobj, left->children[0]); /* the container */
        ana_compile_unit(vm, funcobj, left->children[1]); /* the index */
        /* this will compile into LOAD_SUBSCRIPT */
        ana_compile_unit(vm, funcobj, left);

        EMITX(vm, func, LOAD_CONST, NEW_INT_CONST(vm, 1), 1, left);
        EMITX(vm, func, IMINUS,       0,                    0, left);
        EMITX(vm, func, STORE_SUBSCRIPT, 0, 0, ast);
      }
      else 
      {
        assert(left->kind == COMO_AST_ID);

        node_id *name = (node_id *)left;

        EMITX(vm, func, LOAD_NAME,  NEW_SYMBOL(vm, name->value), 1, left);
        EMITX(vm, func, LOAD_CONST, NEW_INT_CONST(vm, 1), 1, left);
        EMITX(vm, func, IMINUS,     0,                    0, left);
        EMITX(vm, func, STORE_NAME, NEW_SYMBOL(vm, name->value), 0, left);
      }

      break;    
    }

    TARGET(COMO_AST_POSTFIXINC) 
    {
      node *left = ast->children[0];

      if(left->kind == COMO_AST_PROP)
      {
        /* compile the previous value */
        ana_compile_unit(vm, funcobj, left->children[0]);
        char *id = ((node_id *)left->children[1])->value;
        EMITX(vm, func, GETPROP, NEW_SYMBOL(vm, id), 0, ast);
        /* end */

        /* push container onto stack, for the later SETPROP opcode */
        ana_compile_unit(vm, funcobj, left->children[0]);

        /* now compile the GETPROP opcode */
        ana_compile_unit(vm, funcobj, left);

        EMITX(vm, func, LOAD_CONST, NEW_INT_CONST(vm, 1), 1, left);
        EMITX(vm, func, IADD,       0,                    0, left);
        EMITX(vm, func, SETPROP,    NEW_SYMBOL(vm, id),   1,  ast);
      }
      else if(left->kind == COMO_AST_INDEX)
      {
        /* the value returned for this expression */
        ana_compile_unit(vm, funcobj, left->children[0]); 
        ana_compile_unit(vm, funcobj, left->children[1]);
        EMITX(vm, func, LOAD_SUBSCRIPT, 0, 0, ast);

        /* For the STORE_SUBSCRIPT */
        ana_compile_unit(vm, funcobj, left->children[0]); 
        ana_compile_unit(vm, funcobj, left->children[1]);

        /* to get the value for the IADD */
        ana_compile_unit(vm, funcobj, left->children[0]); 
        ana_compile_unit(vm, funcobj, left->children[1]);
        EMITX(vm, func, LOAD_SUBSCRIPT, 0, 0, ast);
        
        EMITX(vm, func, LOAD_CONST, NEW_INT_CONST(vm, 1), 1, left);
        EMITX(vm, func, IADD,       0,                    0, left);

        /* pops the above two values */
        EMITX(vm, func, STORE_SUBSCRIPT, 0, 1, ast);
      }
      else 
      {
        assert(left->kind == COMO_AST_ID);

        node_id *name = (node_id *)left;

        EMITX(vm, func, LOAD_NAME,  NEW_SYMBOL(vm, name->value), 1, left);
        EMITX(vm, func, LOAD_NAME,  NEW_SYMBOL(vm, name->value), 1, left);
        EMITX(vm, func, LOAD_CONST, NEW_INT_CONST(vm, 1), 1, left);
        EMITX(vm, func, IADD,       0,                    0, left);
        EMITX(vm, func, STORE_NAME, NEW_SYMBOL(vm, name->value), 1, left);
      }

      break;
    }

    TARGET(COMO_AST_POSTFIXDEC)
    {
      node *left = ast->children[0];

      if(left->kind == COMO_AST_PROP)
      {
        /* compile the previous value */
        ana_compile_unit(vm, funcobj, left->children[0]);
        char *id = ((node_id *)left->children[1])->value;
        EMITX(vm, func, GETPROP, NEW_SYMBOL(vm, id), 0, ast);
        /* end */

        /* push container onto stack, for the later SETPROP opcode */
        ana_compile_unit(vm, funcobj, left->children[0]);

        /* now compile the GETPROP opcode */
        ana_compile_unit(vm, funcobj, left);
        
        EMITX(vm, func, LOAD_CONST, NEW_INT_CONST(vm, 1), 1, left);
        EMITX(vm, func, IMINUS,       0,                  0, left);
        EMITX(vm, func, SETPROP,    NEW_SYMBOL(vm, id),   1, ast);
      }
      else if(left->kind == COMO_AST_INDEX)
      {
        /* the value returned for this expression */
        ana_compile_unit(vm, funcobj, left->children[0]); 
        ana_compile_unit(vm, funcobj, left->children[1]);
        EMITX(vm, func, LOAD_SUBSCRIPT, 0, 0, ast);

        /* For the STORE_SUBSCRIPT */
        ana_compile_unit(vm, funcobj, left->children[0]); 
        ana_compile_unit(vm, funcobj, left->children[1]);

        /* to get the value for the IADD */
        ana_compile_unit(vm, funcobj, left->children[0]); 
        ana_compile_unit(vm, funcobj, left->children[1]);
        EMITX(vm, func, LOAD_SUBSCRIPT, 0, 0, ast);
        
        EMITX(vm, func, LOAD_CONST, NEW_INT_CONST(vm, 1), 1, left);
        EMITX(vm, func, IMINUS,       0,                    0, left);

        /* pops the above two values */
        EMITX(vm, func, STORE_SUBSCRIPT, 0, 1, ast);
      }
      else 
      {
        assert(left->kind == COMO_AST_ID);

        node_id *name = (node_id *)left;

        /* Store the value before it was decremented */
        EMITX(vm, func, LOAD_NAME,  NEW_SYMBOL(vm, name->value), 1, left);
        EMITX(vm, func, LOAD_NAME,  NEW_SYMBOL(vm, name->value), 1, left);
        EMITX(vm, func, LOAD_CONST, NEW_INT_CONST(vm, 1), 1, left);
        EMITX(vm, func, IMINUS,     0,                    0, left);
        /* pass 1 to oparg, to tell it not to push this value to the stack */
        EMITX(vm, func, STORE_NAME, NEW_SYMBOL(vm, name->value), 1, left);
      }

      break;    
    }

    default:
      printf("compile: %s is not implemented\n", astkind(ast->kind));
      exit(1);
  }
}

static void ana_compile_return_statement(ana_vm *vm, ana_function_defn *func)
{
  EMIT(func, LOAD_CONST, NEW_INT_CONST(vm, 0), 0);
  EMIT(func, IRETURN, 0, 0);
}

static void ana_compile_builtins(ana_vm *vm, ana_function *funcobj, node *ast)
{
  ana_function_defn *func = ANA_GET_FUNCTION_DEF(funcobj);

  ana_object *readline_handler = 
    ana_functionfromhandler(ana_cstring(funcobj->filename), "readline", ana__builtin_readline);

  EMITX(vm, func, LOAD_CONST, NEW_FUNCTION_CONST(vm, readline_handler), 0, ast);
  EMITX(vm, func, LOAD_CONST, NEW_STR_CONST(vm, "readline"), 0,  ast);
  EMITX(vm, func, DEFINE_FUNCTION, 0, 0, ast);

  ana_object *print_handler =
    ana_functionfromhandler(ana_cstring(funcobj->filename), "print", ana__builtin_print);

  EMITX(vm, func, LOAD_CONST, NEW_FUNCTION_CONST(vm, print_handler), 0, ast);
  EMITX(vm, func, LOAD_CONST, NEW_STR_CONST(vm, "print"), 0,  ast);
  EMITX(vm, func, DEFINE_FUNCTION, 0, 0, ast);
}


ana_function *ana_compileast(char *filename, ana_vm *vm, node *ast)
{
  ana_object *func = ana_function_defn_new(filename, "__main__");

  ana_compile_builtins(vm, ANA_GET_FUNCTION(func), ast);

  ana_compile_unit(vm, func, ast);

  ana_compile_return_statement(vm, ANA_GET_FUNCTION_DEF(func));
  
  return ana_get_function(func);
}

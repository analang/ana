#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <stddef.h>

#include "ana_ast.h"
#include "ana_parser.h"
#include "ana_lexer.h"

int ana_astfromstring(const char *cmd, int argc, char **argv, 
  ana_parser_state *pstate)
{
  (void)argc;
  (void)argv;

  yyscan_t scanner;
  YY_BUFFER_STATE state;
  int retval;
  
  if(yylex_init_extra(pstate, &scanner)) 
    retval = 1;  
  else {
    state = yy_scan_string(cmd, scanner);
    retval = yyparse(pstate, scanner);
    yy_delete_buffer(state, scanner);
    yylex_destroy(scanner);
  }

  return retval;
}

int ana_astfromfile(
  FILE *fp,
  const char *fname, 
  int argc, 
  char **argv, 
  ana_parser_state *pstate)
{
  (void)fname;
  (void)argc;
  (void)argv;
  
  yyscan_t scanner;
  int retval;

  if(yylex_init_extra(pstate, &scanner)) 
    retval = 1;  
  else {
    yyset_in(fp, scanner);
    retval = yyparse(pstate, scanner);
    yylex_destroy(scanner);
  }

  return retval;
}

void visit(node *node, int tabs)
{ 
  int i;
  for(i = 0; i < tabs; i++)
    fprintf(stdout, " ");

  fprintf(stdout, "%s", node->info);
  
  if(node->attribute_visit)
    node->attribute_visit(node);
  
  fputc('\n', stdout);

  tabs++;

  if(node->nchild) {
    for(i = 0; i < node->nchild; i++) {
      assert(node->children[i]->visit != NULL);
      node->children[i]->visit(node->children[i], tabs);
    }
  }

}

static void binop_attribute_visitor(node *n)
{
  const char *op = "";

  if(n->attributes == COMO_AST_ASSIGN) 
    op = "=";
  else if(n->attributes == COMO_AST_ADD)
    op = "+";
  else if(n->attributes == COMO_AST_SUB)
    op = "-";
  else if(n->attributes == COMO_AST_MUL)
    op = "*";
  else if(n->attributes == COMO_AST_DIV)
    op = "/";

  fprintf(stdout, "(%s)", op);
}

static void visit_id(node *n, int tabs)
{
  assert(n->kind == COMO_AST_ID);

  char *value = ((node_id *)n)->value;
  int i;

  for(i = 0; i < tabs; i++)
    fprintf(stdout, " ");
  
  fprintf(stdout, "Identifier(=%s)\n", value);
}

static void visit_string(node *n, int tabs)
{
  assert(n->kind == COMO_AST_STRING);

  char *value = ((node_string *)n)->value;
  int i;

  for(i = 0; i < tabs; i++)
    fprintf(stdout, " ");
  
  fprintf(stdout, "String('%s')\n", value);
}

static void visit_int(node *n, int tabs)
{
  int i;
  for(i = 0; i < tabs; i++)
    fprintf(stdout, " ");

  fprintf(stdout, "Int(%ld)\n", ((node_int *)n)->value);
}

static void visit_double(node *n, int tabs)
{
  int i;
  for(i = 0; i < tabs; i++)
    fprintf(stdout, " ");

  fprintf(stdout, "Double(%.17g)\n", ((node_double *)n)->value);
}

static void visit_bool(node *n, int tabs)
{
  int i;
  for(i = 0; i < tabs; i++)
    fprintf(stdout, " ");

  fprintf(stdout, "Bool(%s)\n", ((node_int *)n)->value ? "true" : "false");
}

static void visit_if(node *n, int tabs)
{
  int i;
  int j;
  for(i = 0; i < tabs; i++)
    fprintf(stdout, " ");

  printf("%s\n", astkind(n->kind));

 // if(n->nchild == 4) 
 // {

    if(n->nchild == 4)
      i = 2;
    else 
      i = 0;

    for(; i < n->nchild; i++)
    {
      for(j = 0; j < tabs; j++)
        fprintf(stdout, " ");

      printf("%s\n", astkind(n->children[i]->kind));

      if(n->children[i]->kind == COMO_AST_ELSE_IF_SUITE)
      {
        int x;
        for(x = 0; x < n->children[i]->nchild; x++)
        {
          printf("  %s\n", astkind(n->children[i]->children[x]->kind));
        }
      }
    }
 // }

}

static void visit_compound_if(node *n, int tabs)
{
  int i;
  int j;
  int x;

  for(i = 0; i < tabs; i++)
    fprintf(stdout, " ");

  printf("%s\n", astkind(n->kind));
 
  for(i = 2; i < n->nchild; i++)
  {
    for(j = 0; j < tabs; j++)
      fprintf(stdout, " ");

    printf("%s\n", astkind(n->children[i]->kind));

    if(n->children[i]->kind == COMO_AST_ELSE_IF_SUITE)
    {
      for(x = 0; x < n->children[i]->nchild; x++)
      {
        printf("  %s\n", astkind(n->children[i]->children[x]->kind));
      }
    }
  }
}

node *list_node(ana_parser_state *state, node_kind kind)
{
  node *n = (node *)ana_arena_alloc(state->arena, sizeof(*n));

  assert(n);

  n->kind = kind;
  n->attributes = 0;
  n->extendedattributes = 0;
  n->line = state->first_line;
  n->info = "List";
  n->nchild = 0;
  n->childcap = 4;
  n->children = (node **)ana_arena_alloc(state->arena, sizeof(*n) * 4);

  assert(n->children);

  n->attribute_visit = NULL;
  if(kind == COMO_AST_IF || kind == COMO_AST_ELSE || kind == COMO_AST_ELSE_IF) 
  {
    n->visit = visit_if;
  }
  else 
  {
    n->visit = visit;
  }
  return n;
}


node *list_node_sized(ana_parser_state *state, node_kind kind, size_t size)
{
  node *n = (node *)ana_arena_alloc(state->arena, sizeof(*n));

  assert(n);

  n->kind = kind;
  n->attributes = 0;
  n->extendedattributes = 0;
  n->line = state->first_line;
  n->info = "List";
  n->nchild = 0;
  n->childcap = size;
  n->children = (node **)ana_arena_alloc(state->arena, sizeof(*n) * size);

  assert(n->children);

  n->attribute_visit = NULL;
  n->visit = visit;
  return n;
}



node *add_child(ana_parser_state *state, node *parent, node *child)
{
  if(parent->nchild >= parent->childcap)
  {
    int i;
    parent->childcap *= 2;

    node **newchildren = (node **)ana_arena_alloc(state->arena, 
      sizeof(node) * parent->childcap);

    assert(newchildren);

    for(i = 0; i < parent->nchild; i++)
      newchildren[i] = parent->children[i];

    newchildren[parent->nchild++] = child;

    parent->children = newchildren;
  } 
  else 
  {
    parent->children[parent->nchild++] = child;
  }

  return parent;
}

node *int_node(ana_parser_state *state, long val)
{
  node_int *n = (node_int *)ana_arena_alloc(state->arena, sizeof(*n));

  assert(n);

  n->base.kind       = COMO_AST_INT;
  n->base.attributes = 0;
  n->base.extendedattributes = 0;
  n->base.line       = state->first_line;
  n->base.nchild     = 0;
  n->base.childcap   = 0;
  n->base.info       = "Int";
  n->base.children   = NULL;
  n->base.attribute_visit = NULL;
  n->base.visit      = visit_int;
  n->value = val;

  return (node *)n;
}

node *bool_node(ana_parser_state *state, int val)
{
  node_int *n = (node_int *)ana_arena_alloc(state->arena, sizeof(*n));

  assert(n);

  n->base.kind       = val ? COMO_AST_TRUE : COMO_AST_FALSE;
  n->base.attributes = 0;
  n->base.extendedattributes = 0;
  n->base.line       = state->first_line;
  n->base.nchild     = 0;
  n->base.childcap   = 0;
  n->base.info       = "Bool";
  n->base.children   = NULL;
  n->base.attribute_visit = NULL;
  n->base.visit      = visit_bool;
  n->value = val;

  return (node *)n;
}

node *double_node(ana_parser_state *state, double val)
{
  node_double *n = (node_double *)ana_arena_alloc(state->arena, sizeof(*n));

  n->base.kind       = COMO_AST_DOUBLE;
  n->base.attributes = 0;
  n->base.extendedattributes = 0;
  n->base.line       = state->first_line;
  n->base.nchild     = 0;
  n->base.childcap   = 0;
  n->base.info       = "Double";
  n->base.children   = NULL;
  n->base.attribute_visit = NULL;
  n->base.visit      = visit_double;
  n->value = val;

  return (node *)n;
}


node *id_node(ana_parser_state *state, char *name)
{
  node_id *n = (node_id *)ana_arena_alloc(state->arena, 
    sizeof(*n));

  assert(n);

  n->base.kind       = COMO_AST_ID;
  n->base.attributes = 0;
  n->base.extendedattributes = 0;
  n->base.line       = state->first_line;
  n->base.nchild     = 0;
  n->base.childcap   = 0;
  n->base.info            = "Identifier";
  n->base.children        = NULL;
  n->base.attribute_visit = NULL;
  n->base.visit           = visit_id;
  n->value = name;

/* This isn't neccessary as the lexer will allocate from the same arena 
   n->value = ana_arena_alloc(state->arena, strlen(name) + 1);
   memcpy(n->value, name, strlen(name) + 1);
*/

  return (node *)n;
}

node *string_node(ana_parser_state *state, char *value)
{
  node_id *n = (node_id *)ana_arena_alloc(state->arena, 
    sizeof(*n));

  assert(n);

  n->base.kind       = COMO_AST_STRING;
  n->base.attributes = 0;
  n->base.extendedattributes = 0;
  n->base.line       = state->first_line;
  n->base.nchild     = 0;
  n->base.childcap   = 0;
  n->base.info       = "String";
  n->base.children   = NULL;
  n->base.attribute_visit = NULL;
  n->base.visit      = visit_string;
  n->value = value;

  return (node *)n;
}

node *binary_op(ana_parser_state *state, int op)
{
  node *list = list_node(state, COMO_AST_BINOP);
  list->attributes = op;
  list->attribute_visit = binop_attribute_visitor;
  list->visit = visit;
  list->info = "BinaryOp";

  return list;
}

node *array_node(ana_parser_state *state, node *args)
{
  node *list = list_node(state, COMO_AST_ARRAY);
  list->attribute_visit = NULL;
  list->visit = visit;
  list->info = "ArrayLiteral";

  assert(list->nchild == 0);
  assert(list->childcap >= 1);
  assert(args->kind == COMO_AST_LIST);

  list->children[list->nchild++] = args;

  return list;
}

node *function_node(ana_parser_state *state, char *name, node *params, 
  node *body)
{
  node *list = list_node(state, COMO_AST_FUNCTION);
  list->attribute_visit = NULL;
  list->visit = visit;
  list->info = "Function";

  add_child(state, list, id_node(state, name));
  add_child(state, list, params);
  add_child(state, list, body);

  return list;
}

node *object_node(ana_parser_state *state, node *init)
{
  node *list = list_node(state, COMO_AST_OBJECT);
  list->attribute_visit = NULL;
  list->visit = visit;
  list->info = "Object";

  add_child(state, list, init);

  return list;
}

node *class_node(ana_parser_state *state, char *name, node *baseclass, node *init)
{
  node *list = list_node(state, COMO_AST_CLASS);
  list->attribute_visit = NULL;
  list->visit = visit;
  list->info = "ClassDefn";

  add_child(state, list, id_node(state, name));
  add_child(state, list, baseclass);
  add_child(state, list, init);
  return list;
}

node *return_node(ana_parser_state *state, node *expression)
{
  node *list = list_node_sized(state, COMO_AST_RETURN, 1);
  list->attribute_visit = NULL;
  list->visit = visit;
  list->info = "Return";

  if(expression) {
    add_child(state, list, expression);
  }

  return list;
}

node *import_node(ana_parser_state *state, node *expression)
{
  node *list = list_node_sized(state, COMO_AST_IMPORT, 1);
  list->attribute_visit = NULL;
  list->visit = visit;
  list->info = "Import";

  add_child(state, list, expression);

  return list;
}

node *call_node(ana_parser_state *state, node *expr, node *args)
{
  node *list = list_node_sized(state, COMO_AST_CALL, 2);
  list->attribute_visit = NULL;
  list->visit = visit;
  list->info = "Call";

  add_child(state, list, expr);
  add_child(state, list, args);

  return list;
}

node *property_node(ana_parser_state *state, node *expr, char *prop)
{
  node *list = list_node_sized(state, COMO_AST_PROP, 2);
  list->attribute_visit = NULL;
  list->visit = visit;
  list->info = "Property";

  add_child(state, list, expr);
  add_child(state, list, id_node(state, prop));

  return list;
}

node *node_new(ana_parser_state *state, node *expr)
{
  node *list = list_node_sized(state, COMO_AST_NEW, 1);
  list->attribute_visit = NULL;
  list->visit = visit;
  list->info = "New";

  add_child(state, list, expr);

  return list;
}

node *program_node(ana_parser_state *state)
{
  node *list = list_node(state, COMO_AST_PROGRAM);
  list->attribute_visit = NULL;
  list->visit = visit;
  list->info = "Program";

  return list;
}

node *class_stmts_node(ana_parser_state *state)
{
  node *list = list_node(state, COMO_AST_CLASS_STMTS);
  list->attribute_visit = NULL;
  list->visit = visit;
  list->info = "ClassStatements";

  return list; 
}

node *class_method_node(ana_parser_state *state, int access_modifier, 
  node *func)
{
  node *list = list_node(state, COMO_AST_CLASS_METHOD);
  list->attribute_visit = NULL;
  list->attributes = access_modifier;
  list->visit = visit;
  list->info = "ClassMethod";

  add_child(state, list, func);

  return list; 
}

node *index_node(ana_parser_state *state, node *container, node *expr)
{
  node *list = list_node(state, COMO_AST_INDEX);
  list->attribute_visit = NULL;
  list->attributes = 0;
  list->visit = visit;
  list->info = "Index";

  add_child(state, list, container);
  add_child(state, list, expr);

  return list;  
}

node *throw_node(ana_parser_state *state, node *val)
{
  node *list = list_node(state, COMO_AST_THROW);
  list->attribute_visit = NULL;
  list->attributes = 0;
  list->visit = visit;
  list->info = "Throw";

  add_child(state, list, val);

  return list;  
}

node *unary_node(ana_parser_state *state, node *operand, int type)
{
  node *list = list_node(state, type);
  list->attribute_visit = NULL;
  list->attributes = 0;
  list->visit = visit;
  list->info = "Unary";

  add_child(state, list, operand);

  return list;  
}

node *compound_if_node(ana_parser_state *state, 
  node *expression,
  node *if_statement,
  node *else_if_stmts,
  node *else_stmts)
{
  node *list = list_node(state, COMO_AST_COMPOUND_IF_STATEMENT);
  list->attribute_visit = NULL;
  list->attributes = 0;
  list->visit = visit_compound_if;
  list->info = "CompoundIfStatement";

  add_child(state, list, expression);
  add_child(state, list, if_statement);
  add_child(state, list, else_if_stmts);
  add_child(state, list, else_stmts);

  return list;  
}

node *postfix_node(ana_parser_state *state, node *left, int type)
{
  node *list = list_node(state, type);
  list->attribute_visit = NULL;
  list->attributes = 0;
  list->visit = visit;
  list->info = "Postfix";

  add_child(state, list, left);

  return list;  
}


node *break_node(ana_parser_state *state)
{
  node *list = list_node(state, COMO_AST_BREAK);
  list->attribute_visit = NULL;
  list->attributes = 0;
  list->visit = visit;
  list->info = "Break";

  return list;  
}

node *continue_node(ana_parser_state *state)
{
  node *list = list_node(state, COMO_AST_CONTINUE);
  list->attribute_visit = NULL;
  list->attributes = 0;
  list->visit = visit;
  list->info = "Continue";

  return list;    
}

#include "astkind.c"

const char *astkind(int kind)
{
  return kind_str[kind];
}



















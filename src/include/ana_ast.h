#ifndef GRAPH_H
#define GRAPH_H

#include <stdint.h>

#include "arena.h"

typedef struct _node node;

#define COMO_PARSER_ERROR_BUFFER 4096
#define COMO_LINE_MAX 2048

typedef struct _ps {
  ana_arena *arena;
  int debug;
  const char *filename;
  FILE *fp;
  char error[COMO_PARSER_ERROR_BUFFER];
  node *ast;
  int line;
  char currlinestr[COMO_LINE_MAX];
  int first_line;
  int first_column;
  int last_line;
  int last_column;
} ana_parser_state;

typedef enum node_kind {
  COMO_AST_VARARG,
  COMO_AST_LEFT_SHIFT,
  COMO_AST_RIGHT_SHIFT,
  COMO_AST_COMPOUND_IF_STATEMENT,
  COMO_AST_ELSE_SUITE,
  COMO_AST_ELSE_IF_SUITE,
  COMO_AST_IN,
  COMO_AST_REM,
  COMO_AST_EQUAL,
  COMO_AST_NOT_EQUAL,
  COMO_AST_UNARY_MINUS,
  COMO_AST_UNARY_PLUS,
  COMO_AST_UNARY_NOT,
  COMO_AST_INT,
  COMO_AST_INDEX,
  COMO_AST_CLASS,
  COMO_AST_LIST,
  COMO_AST_STRING,
  COMO_AST_DOUBLE,
  COMO_AST_ID,
  COMO_AST_BINOP,
  COMO_AST_LTE,
  COMO_AST_GTE,
  COMO_AST_LT,
  COMO_AST_GT,
  COMO_AST_ASSIGN,
  COMO_AST_ADD,
  COMO_AST_WHILE,
  COMO_AST_IF,
  COMO_AST_ELSE_IF,
  COMO_AST_ELSE,
  COMO_AST_FOR,
  COMO_AST_FOREACH,
  COMO_AST_CONTINUE,
  COMO_AST_BREAK,
  COMO_AST_RETURN,
  COMO_AST_ARRAY,
  COMO_AST_TRY,
  COMO_AST_IMPORT,
  COMO_AST_OBJECT,
  COMO_AST_NEW,
  COMO_AST_FUNCTION,
  COMO_AST_SUB,
  COMO_AST_MUL,
  COMO_AST_DIV,
  COMO_AST_POSTFIXINC,
  COMO_AST_POSTFIXDEC,
  COMO_AST_PREFIXINC,
  COMO_AST_PREFIXDEC,
  COMO_AST_TRUE,
  COMO_AST_FALSE,
  COMO_AST_OR,
  COMO_AST_AND,
  COMO_AST_CALL,
  COMO_AST_PROP,
  COMO_AST_PROGRAM,
  COMO_AST_CLASS_STMTS,
  COMO_AST_CLASS_METHOD,
  COMO_AST_PROTECTED,
  COMO_AST_PUBLIC,
  COMO_AST_PRIVATE,
  COMO_AST_THROW,
  COMO_AST_PLUS_ASSIGN,
  COMO_AST_TOP  /* Represents the root of the graph */
} node_kind;

typedef int node_attributes;
typedef int node_line;

struct _node {
  node_kind       kind;
  node_attributes attributes;
  node_attributes extendedattributes;
  node_line       line;
  int   nchild;
  int   childcap;
  const char     *info;
  struct _node **children;
  void(*attribute_visit)(struct _node *);
  void(*visit)(struct _node *, int);
};

/* edge nodes */
typedef struct _node_int {
  node base;
  long value;
} node_int;

typedef struct _node_double {
  node base;
  double value;
} node_double;

typedef struct _node_string {
  node base;
  char *value;
} node_string;

typedef struct _node_id {
  node base;
  char *value;
} node_id;

void visit(node *node, int tabs);

node *list_node(ana_parser_state *state, node_kind kind);
node *add_child(ana_parser_state *state, node *parent, node *child);
node *int_node(ana_parser_state *state, long value);
node *id_node(ana_parser_state *state, char *id);
node *binary_op(ana_parser_state *state, int op);
node *string_node(ana_parser_state *state, char *value);
node *array_node(ana_parser_state *state, node *args);
node *function_node(ana_parser_state *state, char *name, node *params, 
  node *body);
node *object_node(ana_parser_state *state, node *init);
node *class_node(ana_parser_state *state, char *name, node *baseclass, node *init);
node *return_node(ana_parser_state *state, node *expression);
node *import_node(ana_parser_state *state, node *expression);
node *call_node(ana_parser_state *state, node *expr, node *args);
node *property_node(ana_parser_state *state, node *expr, char *prop);
node *node_new(ana_parser_state *state, node *expr);
node *program_node(ana_parser_state *state);
node *class_stmts_node(ana_parser_state *state);
node *class_method_node(ana_parser_state *state, int access_modifier, 
  node *func);
node *index_node(ana_parser_state *state, node *container, node *expr);
node *double_node(ana_parser_state *state, double val);
node *bool_node(ana_parser_state *state, int val);
node *throw_node(ana_parser_state *state, node *val);
node *unary_node(ana_parser_state *state, node *operand, int type);
node *compound_if_node(ana_parser_state *state, 
  node *expression,
  node *if_statement,
  node *else_if_stmts,
  node *else_stmts);
node *break_node(ana_parser_state *state);
node *continue_node(ana_parser_state *state);
node *vararg_node(ana_parser_state *state, char *name);
node *postfix_node(ana_parser_state *state, node *left, int type);

int ana_astfromfile(
  FILE *fp, const char *fname, int argc, char **argv, ana_parser_state *state);

int ana_astfromstring(const char *cmd, int argc, char **argv, 
  ana_parser_state *pstate);

const char *astkind(int kind);

#endif

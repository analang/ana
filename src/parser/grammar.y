%{
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "ana_ast.h"
#include "ana_parser.h"
#include "ana_lexer.h"

int yyerror(YYLTYPE *loc, 
  ana_parser_state *state, 
  yyscan_t scanner, 
  const char* msg)
{
  (void)scanner;
  (void)loc;

  char *truncated = strstr(msg, "syntax error, ");

  if(!truncated) 
    truncated = (char *)msg;
  else
    truncated = truncated + (sizeof("syntax error, ") - 1);

  snprintf(state->error, COMO_PARSER_ERROR_BUFFER, "%s", truncated);

  return 1;
}
%}

%code requires {
#include "ana_ast.h"
#ifndef YY_TYPEDEF_YY_SCANNER_T
#define YY_TYPEDEF_YY_SCANNER_T
typedef void* yyscan_t;
#endif
}

%start program

%expect 0
%define parse.error verbose

%no-lines

%define api.pure full
%locations
%token-table

%lex-param   { yyscan_t scanner}
%parse-param { ana_parser_state *pstate}
%parse-param { yyscan_t scanner }

%union {
  long lval;
  double dval;
  char* id;
  char* stringliteral; 
  node *ast;
  node_kind token;
}


%left '+' '-'
%left '*' '/' '%'
%left T_EQ

%token <lval> T_INT "integer literal"
%token <dval> T_DOUBLE "double literal"
%token <id> T_ID "identifier"
%token <stringliteral> T_STR_LIT "string literal"

%token T_FUNCTION "keyword function"
%token T_CLASS "keyword class"
%token T_PUBLIC "keyword public"
%token T_PROTECTED "keyword protected"
%token T_PRIVATE "keyword private"
%token T_NEW "keyword new"
%token T_IMPORT "keyword import"
%token T_AS "keyword as"
%token T_LTE "<="
%token T_GTE ">="
%token T_EQ "=="
%token T_NEQ "!="
%token T_INC "++"
%token T_DEC "--"
%token T_LA "&&"
%token T_LO "||"
%token T_BT "true literal"
%token T_BF "false literal"
%token T_LS "<<"
%token T_RS ">>"

%token T_IF       "keyword if"
%token T_ELSE_IF  "keyword else if"
%token T_ELSE     "keyword else"
%token T_FOR      "keyword for"
%token T_FOREACH  "keyword foreach"
%token T_WHILE    "keyword while"
%token T_CONTINUE "keyword continue"
%token T_BREAK    "keyword break"
%token T_RETURN   "keyword return"
%token T_IN       "keyword in"
%token T_THROW    "keyword throw"
%token T_TRY      "keyword try"
%token T_CATCH    "keyword catch"

%token END 0 "EOF"
%token UNDEFINED 2 "character"

%type<ast> function_statement function_statements function_def statements 
%type<ast> statement program constant primary_expression assignment_expression 
%type<ast> unary_expression optional_argument_list argument_list argument 
%type<ast> postfix_expression
%type<ast> key opt_key_vpairs key_vpairs keyvpair
%type<ast> class_statements class_statement class_def
%type<ast> import_stmt dotted_name import_name optional_as_modifier
%type<ast> trycatch_stmt stmt_group stmt relational_expression
%type<ast> function_expression
%type<ast> selection_stmt
%type<ast> if_statements if_statement else_if_stmts elseif else_stmt optional_assignment_expression

%type<ast> basic_expression logical_or_expression logical_and_expression
%type<ast> inclusive_or_expression shift_expression equality_expression
%type<ast> additive_expression multiplicative_expression and_expression exclusive_or_expression
%type<ast> function_params function_param optional_parameter_list jump_statement

%type<token> access_modifier

%initial-action
{
  /* To get rid of $undefined */
  /* Do stuff here for things to do before the parsing starts */
};

%%
program: 
  statements { pstate->ast = $1; }
;

statements:
  statements statement { $$ = add_child(pstate, $1, $2); }
| %empty               { $$ = program_node(pstate);      }
;

statement:
  assignment_expression ';'            { $$ = $1; }
| function_def                         { $$ = $1; }
| import_stmt ';'                      { $$ = $1; }
| class_def                            { $$ = $1; }
| trycatch_stmt                        { $$ = $1; }
| selection_stmt                       { $$ = $1; }
;

selection_stmt:
  T_IF '(' assignment_expression ')' '{' if_statements '}' { 
    $$ = list_node(pstate, COMO_AST_IF);
    // assignment_expression
    add_child(pstate, $$, $3);
    // if_statement suite
    add_child(pstate, $$, $6);
  }
|
  T_IF '(' assignment_expression ')' '{' if_statements '}' else_if_stmts {
    $$ = list_node(pstate, COMO_AST_IF);
    add_child(pstate, $$, $3);
    add_child(pstate, $$, $6);  
    add_child(pstate, $$, $8);
  }
|
  T_IF '(' assignment_expression ')' '{' if_statements '}' else_stmt { 
    $$ = list_node(pstate, COMO_AST_IF);
    // assignment_expression
    add_child(pstate, $$, $3);
    // statement_suite
    add_child(pstate, $$, $6);
    // optional else_stmt
    add_child(pstate, $$, $8);
  }
|
  T_IF '(' assignment_expression ')' '{' if_statements '}' else_if_stmts else_stmt {
    //$$ = list_node(pstate, COMO_AST_COMPOUND_IF_STATEMENT);
    
    $$ = compound_if_node(pstate, $3, $6, $8, $9);

    // assignment_expression
    //add_child(pstate, $$, $3);

    //if_statements
    //add_child(pstate, $$, $6);  
  
    // else_if_stmts
    //add_child(pstate, $$, $8);

    // else_stmt
    //add_child(pstate, $$, $9);
  }
|
  T_WHILE '(' assignment_expression ')' '{' if_statements '}' {
    $$ = list_node(pstate, COMO_AST_WHILE);
    add_child(pstate, $$, $3);
    add_child(pstate, $$, $6);     
  }
|
  T_FOR '(' assignment_expression ';' assignment_expression ';' assignment_expression ')' '{' if_statements '}' {
    $$ = list_node(pstate, COMO_AST_FOR);
    add_child(pstate, $$, $3);
    add_child(pstate, $$, $5);
    add_child(pstate, $$, $7);     
    add_child(pstate, $$, $10);
  }
|
  T_FOREACH '(' T_ID T_IN assignment_expression ')' '{' if_statements '}' {
    $$ = list_node(pstate, COMO_AST_FOREACH);
    add_child(pstate, $$, id_node(pstate, $3));
    add_child(pstate, $$, $5);
    add_child(pstate, $$, $8);
  }
;

else_if_stmts:
  else_if_stmts elseif { 
    $$ = $1;
    add_child(pstate, $$, $2); 
  }
| elseif { 
    $$ = list_node(pstate, COMO_AST_ELSE_IF_SUITE); 
    add_child(pstate, $$, $1);
  }
;

elseif:
  T_ELSE_IF '(' assignment_expression ')' '{' if_statements '}' {
    $$ = list_node(pstate, COMO_AST_ELSE_IF);
    // assignment expression
    add_child(pstate, $$, $3);
    // statements
    add_child(pstate, $$, $6);
  }
;
  
else_stmt:
  T_ELSE '{' if_statements '}' {
    $$ = list_node(pstate, COMO_AST_ELSE_SUITE);
    add_child(pstate, $$, $3);
  }
;

if_statements:
  if_statements if_statement {
    $$ = add_child(pstate, $1, $2); 
  }
| %empty                     { 
    $$ = list_node(pstate, COMO_AST_LIST);
  }

if_statement:
  assignment_expression ';' { $$ = $1; }
| import_stmt ';'           { $$ = $1; }
| trycatch_stmt             { $$ = $1; }
| selection_stmt            { $$ = $1; }
| jump_statement ';'        { $$ = $1; }
;

trycatch_stmt:
  T_TRY '{' stmt_group '}' T_CATCH '(' T_ID ')' '{' stmt_group '}'{
    $$ = list_node(pstate, COMO_AST_TRY);
    add_child(pstate, $$, $3);
    add_child(pstate, $$, id_node(pstate, $7));
    add_child(pstate, $$, $10);
  }
;

stmt_group:
  stmt_group stmt { $$ = $1;
    add_child(pstate, $1, $2);
  }
| %empty          { $$ = list_node(pstate, COMO_AST_LIST); }

stmt:
  assignment_expression ';' { $$ = $1; }
| import_stmt ';'           { $$ = $1; }
| trycatch_stmt             { $$ = $1; }
| selection_stmt            { $$ = $1; }
| jump_statement ';'        { $$ = $1; }
;

import_stmt:
  T_IMPORT dotted_name optional_as_modifier {
    $$ = import_node(pstate, $2);
    add_child(pstate, $$, $3);
  }
;

dotted_name:
// Each dotted name can be a directory, until the last 
// one where it should be a file
  import_name { 
    $$ = list_node(pstate, COMO_AST_LIST); 
    add_child(pstate, $$, $1);
  }
| dotted_name '.' import_name   { 
    $$ = $1;
    add_child(pstate, $1, $3);
  }
;

import_name:
  T_ID { 
    $$ = id_node(pstate, $1); 
  }
;

// So we don't have nested functions 
function_statements:
  function_statements function_statement { 
    $$ = $1; 
    add_child(pstate, $$, $2);
  }
| %empty { 
    $$ = list_node(pstate, COMO_AST_LIST); 
  }
;

function_statement:
  assignment_expression ';' { $$ = $1; }
| import_stmt ';'           { $$ = $1; }
| trycatch_stmt             { $$ = $1; }
| selection_stmt            { $$ = $1; }
| jump_statement ';'        { $$ = $1; }
;

jump_statement:
  T_RETURN optional_assignment_expression {
    $$ = return_node(pstate, $2);
  }
;
  
optional_assignment_expression:
  assignment_expression { $$ = $1; }
| %empty { $$ = NULL; }
;

optional_as_modifier:
  T_AS T_ID {
    $$ = id_node(pstate, $2);
  }
| %empty { 
    $$ = NULL; 
  }
;

function_def:
  T_FUNCTION T_ID '(' optional_parameter_list ')' '{' function_statements '}' { ;
    $$ = function_node(pstate, $2, $4, $7); 
  }
;

function_expression:
  T_FUNCTION '(' optional_parameter_list ')' '{' function_statements '}' { 
    $$ = function_node(pstate, "<anonymous>", $3, $6);
  }
;

access_modifier:
  T_PUBLIC {
    $$ = COMO_AST_PUBLIC;
  }
  | T_PROTECTED {
    $$ = COMO_AST_PROTECTED;
  } 
  | T_PRIVATE {
    $$ = COMO_AST_PRIVATE;
  }
;

class_statements:
  class_statements class_statement { 
    $$ = $1;  
    add_child(pstate, $$, $2); 
  }
| %empty { 
    $$ = class_stmts_node(pstate); 
  }
 
class_statement:
  access_modifier function_def { 
    $$ = class_method_node(pstate, $1, $2);
  }
;

class_def:
  T_CLASS T_ID '{' class_statements '}' { 
    $$ = class_node(pstate, $2, $4); 
  }
;

primary_expression:
  T_ID {
    $$ = id_node(pstate, $1); 
  }
| constant { 
    $$ = $1;                                
  }
| '(' assignment_expression ')' { 
    $$ = $2;                      
   }
| '{' opt_key_vpairs '}' { 
    $$ = object_node(pstate, $2);                  
  }
| '[' optional_argument_list ']' { 
    $$ = array_node(pstate, $2); 
  }
| function_expression { $$ = $1; }
;

opt_key_vpairs:
  key_vpairs { 
    $$ = $1; 
  }
| %empty { 
    $$ = list_node(pstate, COMO_AST_LIST); 
  }
;

key_vpairs:
  keyvpair {     
    $$ = list_node(pstate, COMO_AST_LIST);
    add_child(pstate, $$, $1);
  }
| key_vpairs ',' keyvpair { 
    add_child(pstate, $1, $3);
    $$ = $1; 
  }
;

keyvpair:
  key ':' assignment_expression { 
    $$ = list_node(pstate, COMO_AST_LIST);
    add_child(pstate, $$, $1);
    add_child(pstate, $$, $3);
  }
;

key:
  T_ID       { $$ = string_node(pstate, $1); }
| T_STR_LIT  { $$ = string_node(pstate, $1); } 
;

optional_parameter_list:
  function_params { $$ = $1; }
| %empty { $$ = list_node(pstate, COMO_AST_LIST); }
;

function_params:
  function_param { 
    $$ = list_node(pstate, COMO_AST_LIST);
    add_child(pstate, $$, $1);
  }
| function_params ',' function_param { 
    add_child(pstate, $1, $3);
    $$ = $1;
 }
;

function_param:
  T_ID { 
    $$ = id_node(pstate, $1);
  }
;

constant: 
  T_INT            { $$ = int_node(pstate, $1);           }
| T_DOUBLE         { $$ = double_node(pstate, $1);        }
| T_STR_LIT        { $$ = string_node(pstate, $1);        }
| T_BT             { $$ = bool_node(pstate, 1);           }
| T_BF             { $$ = bool_node(pstate, 0);           }
;

basic_expression:
  logical_or_expression { $$ = $1; }
;

logical_or_expression:
  logical_and_expression { $$ = $1; }
| logical_or_expression T_LO logical_and_expression { $$ = $1; }
;

logical_and_expression:
  inclusive_or_expression { $$ = $1; }
| logical_and_expression T_LA inclusive_or_expression { $$ = $1; }
;

inclusive_or_expression:
  exclusive_or_expression { $$ = $1; }
| inclusive_or_expression '|' exclusive_or_expression { $$ = $1; }
;

exclusive_or_expression:
  and_expression { $$ = $1; }
| exclusive_or_expression '^' and_expression { $$ = $1; }
;

and_expression:
  equality_expression { $$ = $1; }
| and_expression '&' equality_expression { $$ = $1; }
;

equality_expression:
  relational_expression { $$ = $1; }
| equality_expression T_EQ relational_expression { 
    $$ = binary_op(pstate, COMO_AST_EQUAL);
    add_child(pstate, $$, $1);
    add_child(pstate, $$, $3);
  }
| equality_expression T_NEQ relational_expression {
    $$ = binary_op(pstate, COMO_AST_NOT_EQUAL);
    add_child(pstate, $$, $1);
    add_child(pstate, $$, $3);
  }
| equality_expression T_IN relational_expression {
    $$ = binary_op(pstate, COMO_AST_IN);
    add_child(pstate, $$, $1);
    add_child(pstate, $$, $3);   
  }
;

relational_expression:
  shift_expression { $$ = $1; }
| relational_expression '<'   shift_expression    { $$ = $1; }
| relational_expression '>'   shift_expression    { $$ = $1; }
| relational_expression T_LTE shift_expression    { 
    $$ = binary_op(pstate, COMO_AST_LTE);
    add_child(pstate, $$, $1);
    add_child(pstate, $$, $3);
  }
| relational_expression T_GTE shift_expression    { $$ = $1; }
;

shift_expression:
  additive_expression { $$ = $1; }
| shift_expression T_LS additive_expression { $$ = $1; }
| shift_expression T_RS additive_expression { $$ = $1; }
;

additive_expression:
  multiplicative_expression { $$ = $1; }
| additive_expression '+' multiplicative_expression { 
    $$ = binary_op(pstate, COMO_AST_ADD);
    add_child(pstate, $$, $1);
    add_child(pstate, $$, $3);
  }
| additive_expression '-' multiplicative_expression { 
    $$ = binary_op(pstate, COMO_AST_SUB);
    add_child(pstate, $$, $1);
    add_child(pstate, $$, $3); 
  }
;

multiplicative_expression:
  unary_expression { $$ = $1; }
| multiplicative_expression '*'  unary_expression { 
    $$ = binary_op(pstate, COMO_AST_MUL);
    add_child(pstate, $$, $1);
    add_child(pstate, $$, $3);
  }
| multiplicative_expression '/'  unary_expression { 
    $$ = binary_op(pstate, COMO_AST_DIV);
    add_child(pstate, $$, $1);
    add_child(pstate, $$, $3);
  }
| multiplicative_expression '%'  unary_expression { 
    $$ = binary_op(pstate, COMO_AST_REM);
    add_child(pstate, $$, $1);
    add_child(pstate, $$, $3);
  }
;

assignment_expression:
  basic_expression { $$ = $1; }
| unary_expression '=' assignment_expression { 
    $$ = binary_op(pstate, COMO_AST_ASSIGN);
    add_child(pstate, $$, $1);
    add_child(pstate, $$, $3);
  }
;

unary_expression: 
  postfix_expression { 
    $$ = $1; 
  }
| T_THROW unary_expression { 
    $$ = throw_node(pstate, $2); 
  }
| T_DEC   unary_expression { 
    $$ = unary_node(pstate, $2, COMO_AST_PREFIXDEC);
  }
| T_INC   unary_expression { 
    $$ = unary_node(pstate, $2, COMO_AST_PREFIXINC);
  }
| '-' unary_expression { 
    $$ = unary_node(pstate, $2, COMO_AST_UNARY_MINUS);
  }
| '+' unary_expression { 
    $$ = unary_node(pstate, $2, COMO_AST_UNARY_PLUS); 
  }
| '!' unary_expression { 
    $$ = unary_node(pstate, $2, COMO_AST_UNARY_NOT); 
  }
;

optional_argument_list:
  argument_list { $$ = $1;                               }
| %empty        { $$ = list_node(pstate, COMO_AST_LIST); }
;

argument_list:
  argument { 
    $$ = list_node(pstate, COMO_AST_LIST);
    add_child(pstate, $$, $1);
  }
| argument_list ',' argument { 
    add_child(pstate, $1, $3);
    $$ = $1;
  }
;

argument:
  assignment_expression { $$ = $1; }
;

postfix_expression:
  primary_expression  { 
    $$ = $1; 
  }
| postfix_expression '[' assignment_expression ']' { 
    $$ = index_node(pstate, $1, $3);
  }
| postfix_expression '(' optional_argument_list ')' { 
    $$ = call_node(pstate, $1, $3);
  }
| postfix_expression '.' T_ID { 
    $$ = property_node(pstate, $1, $3);
  }
| postfix_expression T_INC { 
    $$ = postfix_node(pstate, $1, COMO_AST_POSTFIXINC); 
  }
| postfix_expression T_DEC { 
    $$ = postfix_node(pstate, $1, COMO_AST_POSTFIXDEC); 
  }
;

%%

/* A Bison parser, made by GNU Bison 3.0.4.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

#ifndef YY_YY_INCLUDE_ANA_PARSER_H_INCLUDED
# define YY_YY_INCLUDE_ANA_PARSER_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif
/* "%code requires" blocks.  */


#include "ana_ast.h"
#ifndef YY_TYPEDEF_YY_SCANNER_T
#define YY_TYPEDEF_YY_SCANNER_T
typedef void* yyscan_t;
#endif



/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    END = 0,
    T_EQ = 258,
    T_INT = 259,
    T_DOUBLE = 260,
    T_ID = 261,
    T_STR_LIT = 262,
    T_FUNCTION = 263,
    T_CLASS = 264,
    T_PUBLIC = 265,
    T_PROTECTED = 266,
    T_PRIVATE = 267,
    T_NEW = 268,
    T_IMPORT = 269,
    T_AS = 270,
    T_LTE = 271,
    T_GTE = 272,
    T_NEQ = 273,
    T_INC = 274,
    T_DEC = 275,
    T_LA = 276,
    T_LO = 277,
    T_BT = 278,
    T_BF = 279,
    T_LS = 280,
    T_RS = 281,
    T_VARARG = 282,
    T_IF = 283,
    T_ELSE_IF = 284,
    T_ELSE = 285,
    T_FOR = 286,
    T_FOREACH = 287,
    T_WHILE = 288,
    T_CONTINUE = 289,
    T_BREAK = 290,
    T_RETURN = 291,
    T_IN = 292,
    T_THROW = 293,
    T_TRY = 294,
    T_CATCH = 295,
    UNDEFINED = 2
  };
#endif
/* Tokens.  */
#define END 0
#define T_EQ 258
#define T_INT 259
#define T_DOUBLE 260
#define T_ID 261
#define T_STR_LIT 262
#define T_FUNCTION 263
#define T_CLASS 264
#define T_PUBLIC 265
#define T_PROTECTED 266
#define T_PRIVATE 267
#define T_NEW 268
#define T_IMPORT 269
#define T_AS 270
#define T_LTE 271
#define T_GTE 272
#define T_NEQ 273
#define T_INC 274
#define T_DEC 275
#define T_LA 276
#define T_LO 277
#define T_BT 278
#define T_BF 279
#define T_LS 280
#define T_RS 281
#define T_VARARG 282
#define T_IF 283
#define T_ELSE_IF 284
#define T_ELSE 285
#define T_FOR 286
#define T_FOREACH 287
#define T_WHILE 288
#define T_CONTINUE 289
#define T_BREAK 290
#define T_RETURN 291
#define T_IN 292
#define T_THROW 293
#define T_TRY 294
#define T_CATCH 295
#define UNDEFINED 2

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED

union YYSTYPE
{


  long lval;
  double dval;
  char* id;
  char* stringliteral; 
  node *ast;
  node_kind token;


};

typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif

/* Location type.  */
#if ! defined YYLTYPE && ! defined YYLTYPE_IS_DECLARED
typedef struct YYLTYPE YYLTYPE;
struct YYLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
};
# define YYLTYPE_IS_DECLARED 1
# define YYLTYPE_IS_TRIVIAL 1
#endif



int yyparse (ana_parser_state *pstate, yyscan_t scanner);

#endif /* !YY_YY_INCLUDE_ANA_PARSER_H_INCLUDED  */

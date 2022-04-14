/* A Bison parser, made by GNU Bison 3.5.1.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2020 Free Software Foundation,
   Inc.

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

/* Undocumented macros, especially those whose name start with YY_,
   are private implementation details.  Do not rely on them.  */

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
    T_NEW = 265,
    T_IMPORT = 266,
    T_AS = 267,
    T_LTE = 268,
    T_GTE = 269,
    T_NEQ = 270,
    T_INC = 271,
    T_DEC = 272,
    T_LA = 273,
    T_LO = 274,
    T_BT = 275,
    T_BF = 276,
    T_LS = 277,
    T_RS = 278,
    T_VARARG = 279,
    T_PLUS_EQUAL = 280,
    T_IF = 281,
    T_ELSE_IF = 282,
    T_ELSE = 283,
    T_FOR = 284,
    T_FOREACH = 285,
    T_WHILE = 286,
    T_CONTINUE = 287,
    T_BREAK = 288,
    T_RETURN = 289,
    T_IN = 290,
    T_THROW = 291,
    T_TRY = 292,
    T_CATCH = 293,
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
#define T_NEW 265
#define T_IMPORT 266
#define T_AS 267
#define T_LTE 268
#define T_GTE 269
#define T_NEQ 270
#define T_INC 271
#define T_DEC 272
#define T_LA 273
#define T_LO 274
#define T_BT 275
#define T_BF 276
#define T_LS 277
#define T_RS 278
#define T_VARARG 279
#define T_PLUS_EQUAL 280
#define T_IF 281
#define T_ELSE_IF 282
#define T_ELSE 283
#define T_FOR 284
#define T_FOREACH 285
#define T_WHILE 286
#define T_CONTINUE 287
#define T_BREAK 288
#define T_RETURN 289
#define T_IN 290
#define T_THROW 291
#define T_TRY 292
#define T_CATCH 293
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

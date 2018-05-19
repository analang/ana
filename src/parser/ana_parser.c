/* A Bison parser, made by GNU Bison 3.0.4.  */

/* Bison implementation for Yacc-like parsers in C

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

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "3.0.4"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 2

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* Copy the first part of user declarations.  */


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



# ifndef YY_NULLPTR
#  if defined __cplusplus && 201103L <= __cplusplus
#   define YY_NULLPTR nullptr
#  else
#   define YY_NULLPTR 0
#  endif
# endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 1
#endif

/* In a future release of Bison, this section will be replaced
   by #include "ana_parser.h".  */
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
    T_IF = 282,
    T_ELSE_IF = 283,
    T_ELSE = 284,
    T_FOR = 285,
    T_FOREACH = 286,
    T_WHILE = 287,
    T_CONTINUE = 288,
    T_BREAK = 289,
    T_RETURN = 290,
    T_IN = 291,
    T_THROW = 292,
    T_TRY = 293,
    T_CATCH = 294,
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
#define T_IF 282
#define T_ELSE_IF 283
#define T_ELSE 284
#define T_FOR 285
#define T_FOREACH 286
#define T_WHILE 287
#define T_CONTINUE 288
#define T_BREAK 289
#define T_RETURN 290
#define T_IN 291
#define T_THROW 292
#define T_TRY 293
#define T_CATCH 294
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

/* Copy the second part of user declarations.  */



#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif

#ifndef YY_ATTRIBUTE
# if (defined __GNUC__                                               \
      && (2 < __GNUC__ || (__GNUC__ == 2 && 96 <= __GNUC_MINOR__)))  \
     || defined __SUNPRO_C && 0x5110 <= __SUNPRO_C
#  define YY_ATTRIBUTE(Spec) __attribute__(Spec)
# else
#  define YY_ATTRIBUTE(Spec) /* empty */
# endif
#endif

#ifndef YY_ATTRIBUTE_PURE
# define YY_ATTRIBUTE_PURE   YY_ATTRIBUTE ((__pure__))
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# define YY_ATTRIBUTE_UNUSED YY_ATTRIBUTE ((__unused__))
#endif

#if !defined _Noreturn \
     && (!defined __STDC_VERSION__ || __STDC_VERSION__ < 201112)
# if defined _MSC_VER && 1200 <= _MSC_VER
#  define _Noreturn __declspec (noreturn)
# else
#  define _Noreturn YY_ATTRIBUTE ((__noreturn__))
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(E) ((void) (E))
#else
# define YYUSE(E) /* empty */
#endif

#if defined __GNUC__ && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN \
    _Pragma ("GCC diagnostic push") \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")\
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# define YY_IGNORE_MAYBE_UNINITIALIZED_END \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif


#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL \
             && defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
  YYLTYPE yyls_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE) + sizeof (YYLTYPE)) \
      + 2 * YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYSIZE_T yynewbytes;                                            \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / sizeof (*yyptr);                          \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, (Count) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYSIZE_T yyi;                         \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  3
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   827

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  63
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  53
/* YYNRULES -- Number of rules.  */
#define YYNRULES  132
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  250

/* YYTRANSLATE[YYX] -- Symbol number corresponding to YYX as returned
   by yylex, with out-of-bounds checking.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   294

#define YYTRANSLATE(YYX)                                                \
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, without out-of-bounds checking.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,    45,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    62,     2,     2,     2,     7,    58,     2,
      47,    48,     5,     3,    54,     4,    51,     6,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    55,    46,
      59,    61,    60,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    52,     2,    53,    57,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    49,    56,    50,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     8,     9,
      10,    11,    12,    13,    14,    15,    16,    17,    18,    19,
      20,    21,    22,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    32,    33,    34,    35,    36,    37,    38,    39,
      40,    41,    42,    43,    44
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   137,   137,   141,   142,   146,   147,   148,   149,   150,
     151,   155,   161,   168,   175,   183,   189,   197,   206,   209,
     215,   223,   230,   231,   234,   235,   236,   237,   238,   242,
     251,   254,   257,   258,   259,   260,   261,   265,   274,   278,
     285,   292,   296,   302,   303,   304,   305,   306,   310,   316,
     317,   321,   324,   330,   336,   342,   345,   348,   354,   358,
     363,   369,   375,   378,   381,   384,   387,   390,   394,   397,
     403,   407,   414,   422,   426,   427,   431,   435,   442,   448,
     449,   450,   451,   452,   456,   460,   461,   465,   466,   470,
     471,   475,   476,   480,   481,   485,   486,   491,   499,   500,
     501,   502,   507,   511,   512,   513,   517,   518,   523,   531,
     532,   537,   542,   550,   551,   559,   560,   561,   562,   563,
     566,   569,   575,   576,   580,   584,   591,   595,   596,   599,
     602,   605,   606
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 1
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"EOF\"", "error", "$undefined", "'+'", "'-'", "'*'", "'/'", "'%'",
  "\"==\"", "\"integer literal\"", "\"double literal\"", "\"identifier\"",
  "\"string literal\"", "\"keyword function\"", "\"keyword class\"",
  "\"keyword public\"", "\"keyword protected\"", "\"keyword private\"",
  "\"keyword new\"", "\"keyword import\"", "\"keyword as\"", "\"<=\"",
  "\">=\"", "\"!=\"", "\"++\"", "\"--\"", "\"&&\"", "\"||\"",
  "\"true literal\"", "\"false literal\"", "\"<<\"", "\">>\"",
  "\"keyword if\"", "\"keyword else if\"", "\"keyword else\"",
  "\"keyword for\"", "\"keyword foreach\"", "\"keyword while\"",
  "\"keyword continue\"", "\"keyword break\"", "\"keyword return\"",
  "\"keyword in\"", "\"keyword throw\"", "\"keyword try\"",
  "\"keyword catch\"", "\"character\"", "';'", "'('", "')'", "'{'", "'}'",
  "'.'", "'['", "']'", "','", "':'", "'|'", "'^'", "'&'", "'<'", "'>'",
  "'='", "'!'", "$accept", "program", "statements", "statement",
  "selection_stmt", "else_if_stmt", "elseif", "else_stmt", "if_statements",
  "if_statement", "trycatch_stmt", "stmt_group", "stmt", "import_stmt",
  "dotted_name", "import_name", "function_statements",
  "function_statement", "jump_statement", "optional_assignment_expression",
  "optional_as_modifier", "function_def", "function_expression",
  "access_modifier", "class_statements", "class_statement", "class_def",
  "primary_expression", "opt_key_vpairs", "key_vpairs", "keyvpair", "key",
  "optional_parameter_list", "function_params", "function_param",
  "constant", "basic_expression", "logical_or_expression",
  "logical_and_expression", "inclusive_or_expression",
  "exclusive_or_expression", "and_expression", "equality_expression",
  "relational_expression", "shift_expression", "additive_expression",
  "multiplicative_expression", "assignment_expression", "unary_expression",
  "optional_argument_list", "argument_list", "argument",
  "postfix_expression", YY_NULLPTR
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[NUM] -- (External) token number corresponding to the
   (internal) symbol number NUM (which must be that of a token).  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,    43,    45,    42,    47,    37,   258,   259,
     260,   261,   262,   263,   264,   265,   266,   267,   268,   269,
     270,   271,   272,   273,   274,   275,   276,   277,   278,   279,
     280,   281,   282,   283,   284,   285,   286,   287,   288,   289,
     290,   291,   292,   293,   294,     2,    59,    40,    41,   123,
     125,    46,    91,    93,    44,    58,   124,    94,    38,    60,
      62,    61,    33
};
# endif

#define YYPACT_NINF -176

#define yypact_value_is_default(Yystate) \
  (!!((Yystate) == (-176)))

#define YYTABLE_NINF -1

#define yytable_value_is_error(Yytable_value) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
    -176,    22,   765,  -176,     2,     2,  -176,  -176,  -176,  -176,
       5,    29,    34,     2,     2,  -176,  -176,     8,    16,    32,
      35,     2,    27,     2,    37,     2,     2,  -176,  -176,  -176,
      38,  -176,  -176,  -176,  -176,  -176,  -176,    23,    57,    30,
      31,    33,     9,   -13,    -6,    25,    64,    41,    28,    43,
      45,  -176,  -176,    46,    86,    49,  -176,   -10,  -176,  -176,
    -176,     2,     2,    89,     2,  -176,  -176,    54,  -176,    53,
      50,  -176,    51,  -176,    55,    56,  -176,  -176,  -176,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,  -176,     2,  -176,
    -176,     2,    94,     2,    86,  -176,    59,    58,  -176,  -176,
      98,    34,  -176,    63,    67,    73,    72,   225,  -176,  -176,
      37,     2,  -176,     2,    57,  -176,    30,    31,    33,     9,
     -13,   -13,    -6,    -6,    -6,    -6,    25,    25,    64,    64,
    -176,  -176,  -176,  -176,    74,  -176,    70,    76,    77,    86,
       3,  -176,  -176,    78,     2,     2,    79,     2,    81,  -176,
    -176,  -176,    84,   103,   104,  -176,  -176,  -176,  -176,  -176,
     102,  -176,  -176,  -176,  -176,  -176,  -176,   141,  -176,  -176,
     110,   109,  -176,  -176,  -176,   111,  -176,  -176,  -176,  -176,
     279,   148,  -176,   333,     2,   112,   387,   149,   441,  -176,
    -176,  -176,   116,  -176,   117,   118,     4,  -176,  -176,  -176,
     119,   120,   121,   122,  -176,  -176,   123,  -176,  -176,  -176,
    -176,   125,   124,     4,  -176,  -176,  -176,  -176,  -176,   126,
     495,   127,     2,  -176,  -176,  -176,  -176,  -176,  -176,   129,
     549,   603,   657,   130,  -176,  -176,  -176,  -176,   711,  -176
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       4,     0,     2,     1,     0,     0,    79,    80,    62,    81,
       0,     0,     0,     0,     0,    82,    83,     0,     0,     0,
       0,     0,     0,     0,    69,   123,     0,     3,    10,     9,
       0,     6,    67,     8,   127,    63,   113,    84,    85,    87,
      89,    91,    93,    95,    98,   103,   106,     0,   109,   115,
       0,   120,   119,     0,    75,     0,    40,    52,    38,   118,
     117,     0,     0,     0,     0,   116,    31,     0,    73,     0,
      68,    70,     0,   126,     0,   122,   124,   121,     7,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     5,     0,   131,
     132,   123,     0,     0,    75,    78,     0,    74,    76,    59,
       0,     0,    37,     0,     0,     0,     0,     0,    64,    65,
       0,     0,    66,     0,    86,   109,    88,    90,    92,    94,
      96,    97,   101,   102,    99,   100,   104,   105,   107,   108,
     110,   111,   112,   114,     0,   130,     0,     0,     0,     0,
       0,    51,    39,     0,     0,     0,     0,    50,     0,    35,
      34,    30,     0,     0,     0,    71,    72,   125,   129,   128,
       0,    42,    77,    55,    56,    57,    61,     0,    58,    23,
       0,     0,    23,    48,    49,     0,    33,    36,    32,    42,
       0,     0,    60,     0,     0,     0,     0,     0,     0,    54,
      46,    45,     0,    41,     0,     0,    11,    27,    22,    26,
       0,     0,     0,     0,    23,    15,     0,    53,    44,    47,
      43,     0,     0,    12,    19,    13,    25,    28,    24,     0,
       0,     0,     0,    23,    18,    14,    23,    17,    31,     0,
       0,     0,     0,     0,    21,    16,    29,    23,     0,    20
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -176,  -176,  -176,  -176,    -1,  -176,   -55,   -54,  -175,  -176,
       0,   -64,  -176,     1,  -176,    69,   -11,  -176,  -113,  -176,
    -176,     6,  -176,  -176,  -176,  -176,  -176,  -176,  -176,  -176,
      61,  -176,    80,  -176,    36,  -176,  -176,  -176,   107,   113,
     101,   105,   128,   -42,   -53,   -16,   -12,    -2,    52,    93,
    -176,    82,  -176
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     1,     2,    27,   207,   223,   224,   225,   193,   208,
     209,   117,   161,   210,    57,    58,   190,   203,   211,   183,
     112,    31,    32,   177,   150,   178,    33,    34,    69,    70,
      71,    72,   106,   107,   108,    35,    36,    37,    38,    39,
      40,    41,    42,    43,    44,    45,    46,   212,    48,    74,
      75,    76,    49
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_uint8 yytable[] =
{
      47,    28,    29,    30,   163,     4,     5,   196,    86,    87,
     110,     6,     7,     8,     9,    50,    53,    84,   173,   174,
     175,    67,     3,    73,    90,    91,    13,    14,    92,    93,
      15,    16,    85,   132,   133,   134,   135,   221,   222,   230,
      55,   111,   130,   131,    21,    56,    88,    89,    68,    23,
      79,    24,    54,   176,    25,    61,    51,    52,   240,   113,
     114,   241,   116,    62,    26,    59,    60,    99,   100,    94,
      95,    96,   248,    65,   136,   137,    66,   204,    77,    63,
     138,   139,    64,    80,    78,   204,    81,    97,    82,    98,
     101,    83,    54,   104,   102,   103,   143,   105,   109,    73,
     115,   146,   118,   119,   120,   145,   121,   148,   122,   151,
     123,   153,   149,   154,   155,   164,   159,   160,   162,   166,
     156,    73,   168,   169,   170,   185,   171,   179,   182,   163,
     186,   125,   125,   125,   125,   125,   125,   125,   125,   125,
     125,   125,   125,   125,   125,   125,   140,   141,   142,   187,
     188,   189,   180,   181,   191,   184,   194,   195,   197,    53,
     216,   214,   218,   219,   220,   226,   227,   228,   234,   235,
     229,   231,   232,   233,   242,   236,   238,   243,   198,   247,
     152,   165,   127,   192,   147,   172,   124,   128,   205,   200,
     201,   202,   213,   126,   144,     0,   205,   200,   201,   202,
       0,     0,     0,     0,     0,   167,     0,     0,     0,     0,
       0,   129,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     4,     5,
     239,     0,     0,     0,     6,     7,     8,     9,    50,     0,
     164,   159,   160,   162,    12,     0,     0,     0,     0,    13,
      14,     0,     0,    15,    16,     0,     0,    17,     0,     0,
      18,    19,    20,     0,     0,   157,     0,    21,    22,     0,
       0,     0,    23,     0,    24,   158,     0,    25,     0,     0,
       0,     0,     4,     5,     0,     0,     0,    26,     6,     7,
       8,     9,    50,     0,     0,     0,     0,     0,    12,     0,
       0,     0,     0,    13,    14,     0,     0,    15,    16,     0,
       0,    17,     0,     0,    18,    19,    20,     0,     0,   157,
       0,    21,    22,     0,     0,     0,    23,     0,    24,   199,
       0,    25,     0,     0,     0,     0,     4,     5,     0,     0,
       0,    26,     6,     7,     8,     9,    50,     0,     0,     0,
       0,     0,    12,     0,     0,     0,     0,    13,    14,     0,
       0,    15,    16,     0,     0,    17,     0,     0,    18,    19,
      20,     0,     0,   157,     0,    21,    22,     0,     0,     0,
      23,     0,    24,   206,     0,    25,     0,     0,     0,     0,
       4,     5,     0,     0,     0,    26,     6,     7,     8,     9,
      50,     0,     0,     0,     0,     0,    12,     0,     0,     0,
       0,    13,    14,     0,     0,    15,    16,     0,     0,    17,
       0,     0,    18,    19,    20,     0,     0,   157,     0,    21,
      22,     0,     0,     0,    23,     0,    24,   215,     0,    25,
       0,     0,     0,     0,     4,     5,     0,     0,     0,    26,
       6,     7,     8,     9,    50,     0,     0,     0,     0,     0,
      12,     0,     0,     0,     0,    13,    14,     0,     0,    15,
      16,     0,     0,    17,     0,     0,    18,    19,    20,     0,
       0,   157,     0,    21,    22,     0,     0,     0,    23,     0,
      24,   217,     0,    25,     0,     0,     0,     0,     4,     5,
       0,     0,     0,    26,     6,     7,     8,     9,    50,     0,
       0,     0,     0,     0,    12,     0,     0,     0,     0,    13,
      14,     0,     0,    15,    16,     0,     0,    17,     0,     0,
      18,    19,    20,     0,     0,   157,     0,    21,    22,     0,
       0,     0,    23,     0,    24,   237,     0,    25,     0,     0,
       0,     0,     4,     5,     0,     0,     0,    26,     6,     7,
       8,     9,    50,     0,     0,     0,     0,     0,    12,     0,
       0,     0,     0,    13,    14,     0,     0,    15,    16,     0,
       0,    17,     0,     0,    18,    19,    20,     0,     0,   157,
       0,    21,    22,     0,     0,     0,    23,     0,    24,   244,
       0,    25,     0,     0,     0,     0,     4,     5,     0,     0,
       0,    26,     6,     7,     8,     9,    50,     0,     0,     0,
       0,     0,    12,     0,     0,     0,     0,    13,    14,     0,
       0,    15,    16,     0,     0,    17,     0,     0,    18,    19,
      20,     0,     0,   157,     0,    21,    22,     0,     0,     0,
      23,     0,    24,   245,     0,    25,     0,     0,     0,     0,
       4,     5,     0,     0,     0,    26,     6,     7,     8,     9,
      50,     0,     0,     0,     0,     0,    12,     0,     0,     0,
       0,    13,    14,     0,     0,    15,    16,     0,     0,    17,
       0,     0,    18,    19,    20,     0,     0,   157,     0,    21,
      22,     0,     0,     0,    23,     0,    24,   246,     0,    25,
       0,     0,     0,     0,     4,     5,     0,     0,     0,    26,
       6,     7,     8,     9,    50,     0,     0,     0,     0,     0,
      12,     0,     0,     0,     0,    13,    14,     0,     0,    15,
      16,     0,     0,    17,     0,     0,    18,    19,    20,     0,
       0,   157,     0,    21,    22,     0,     0,     0,    23,     0,
      24,   249,     0,    25,     0,     0,     0,     0,     4,     5,
       0,     0,     0,    26,     6,     7,     8,     9,    10,    11,
       0,     0,     0,     0,    12,     0,     0,     0,     0,    13,
      14,     0,     0,    15,    16,     0,     0,    17,     0,     0,
      18,    19,    20,     0,     0,     0,     0,    21,    22,     0,
       0,     0,    23,     0,    24,     0,     0,    25,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    26
};

static const yytype_int16 yycheck[] =
{
       2,     2,     2,     2,   117,     3,     4,   182,    21,    22,
      20,     9,    10,    11,    12,    13,    11,     8,    15,    16,
      17,    23,     0,    25,    30,    31,    24,    25,     3,     4,
      28,    29,    23,    86,    87,    88,    89,    33,    34,   214,
      11,    51,    84,    85,    42,    11,    59,    60,    11,    47,
      27,    49,    47,    50,    52,    47,     4,     5,   233,    61,
      62,   236,    64,    47,    62,    13,    14,    24,    25,     5,
       6,     7,   247,    21,    90,    91,    49,   190,    26,    47,
      92,    93,    47,    26,    46,   198,    56,    46,    57,    61,
      47,    58,    47,    47,    51,    52,    98,    11,    49,   101,
      11,   103,    48,    50,    54,    11,    55,    48,    53,    11,
      54,    48,    54,    46,    41,   117,   117,   117,   117,   121,
      48,   123,    48,    53,    48,    44,    49,    49,    49,   242,
      46,    79,    80,    81,    82,    83,    84,    85,    86,    87,
      88,    89,    90,    91,    92,    93,    94,    95,    96,    46,
      46,    49,   154,   155,    13,   157,    46,    48,    47,    11,
      11,    49,    46,    46,    46,    46,    46,    46,   223,   223,
      48,    48,    47,    49,   238,    49,    49,    48,   189,    49,
     111,   120,    81,   177,   104,   149,    79,    82,   190,   190,
     190,   190,   194,    80,   101,    -1,   198,   198,   198,   198,
      -1,    -1,    -1,    -1,    -1,   123,    -1,    -1,    -1,    -1,
      -1,    83,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,     3,     4,
     232,    -1,    -1,    -1,     9,    10,    11,    12,    13,    -1,
     242,   242,   242,   242,    19,    -1,    -1,    -1,    -1,    24,
      25,    -1,    -1,    28,    29,    -1,    -1,    32,    -1,    -1,
      35,    36,    37,    -1,    -1,    40,    -1,    42,    43,    -1,
      -1,    -1,    47,    -1,    49,    50,    -1,    52,    -1,    -1,
      -1,    -1,     3,     4,    -1,    -1,    -1,    62,     9,    10,
      11,    12,    13,    -1,    -1,    -1,    -1,    -1,    19,    -1,
      -1,    -1,    -1,    24,    25,    -1,    -1,    28,    29,    -1,
      -1,    32,    -1,    -1,    35,    36,    37,    -1,    -1,    40,
      -1,    42,    43,    -1,    -1,    -1,    47,    -1,    49,    50,
      -1,    52,    -1,    -1,    -1,    -1,     3,     4,    -1,    -1,
      -1,    62,     9,    10,    11,    12,    13,    -1,    -1,    -1,
      -1,    -1,    19,    -1,    -1,    -1,    -1,    24,    25,    -1,
      -1,    28,    29,    -1,    -1,    32,    -1,    -1,    35,    36,
      37,    -1,    -1,    40,    -1,    42,    43,    -1,    -1,    -1,
      47,    -1,    49,    50,    -1,    52,    -1,    -1,    -1,    -1,
       3,     4,    -1,    -1,    -1,    62,     9,    10,    11,    12,
      13,    -1,    -1,    -1,    -1,    -1,    19,    -1,    -1,    -1,
      -1,    24,    25,    -1,    -1,    28,    29,    -1,    -1,    32,
      -1,    -1,    35,    36,    37,    -1,    -1,    40,    -1,    42,
      43,    -1,    -1,    -1,    47,    -1,    49,    50,    -1,    52,
      -1,    -1,    -1,    -1,     3,     4,    -1,    -1,    -1,    62,
       9,    10,    11,    12,    13,    -1,    -1,    -1,    -1,    -1,
      19,    -1,    -1,    -1,    -1,    24,    25,    -1,    -1,    28,
      29,    -1,    -1,    32,    -1,    -1,    35,    36,    37,    -1,
      -1,    40,    -1,    42,    43,    -1,    -1,    -1,    47,    -1,
      49,    50,    -1,    52,    -1,    -1,    -1,    -1,     3,     4,
      -1,    -1,    -1,    62,     9,    10,    11,    12,    13,    -1,
      -1,    -1,    -1,    -1,    19,    -1,    -1,    -1,    -1,    24,
      25,    -1,    -1,    28,    29,    -1,    -1,    32,    -1,    -1,
      35,    36,    37,    -1,    -1,    40,    -1,    42,    43,    -1,
      -1,    -1,    47,    -1,    49,    50,    -1,    52,    -1,    -1,
      -1,    -1,     3,     4,    -1,    -1,    -1,    62,     9,    10,
      11,    12,    13,    -1,    -1,    -1,    -1,    -1,    19,    -1,
      -1,    -1,    -1,    24,    25,    -1,    -1,    28,    29,    -1,
      -1,    32,    -1,    -1,    35,    36,    37,    -1,    -1,    40,
      -1,    42,    43,    -1,    -1,    -1,    47,    -1,    49,    50,
      -1,    52,    -1,    -1,    -1,    -1,     3,     4,    -1,    -1,
      -1,    62,     9,    10,    11,    12,    13,    -1,    -1,    -1,
      -1,    -1,    19,    -1,    -1,    -1,    -1,    24,    25,    -1,
      -1,    28,    29,    -1,    -1,    32,    -1,    -1,    35,    36,
      37,    -1,    -1,    40,    -1,    42,    43,    -1,    -1,    -1,
      47,    -1,    49,    50,    -1,    52,    -1,    -1,    -1,    -1,
       3,     4,    -1,    -1,    -1,    62,     9,    10,    11,    12,
      13,    -1,    -1,    -1,    -1,    -1,    19,    -1,    -1,    -1,
      -1,    24,    25,    -1,    -1,    28,    29,    -1,    -1,    32,
      -1,    -1,    35,    36,    37,    -1,    -1,    40,    -1,    42,
      43,    -1,    -1,    -1,    47,    -1,    49,    50,    -1,    52,
      -1,    -1,    -1,    -1,     3,     4,    -1,    -1,    -1,    62,
       9,    10,    11,    12,    13,    -1,    -1,    -1,    -1,    -1,
      19,    -1,    -1,    -1,    -1,    24,    25,    -1,    -1,    28,
      29,    -1,    -1,    32,    -1,    -1,    35,    36,    37,    -1,
      -1,    40,    -1,    42,    43,    -1,    -1,    -1,    47,    -1,
      49,    50,    -1,    52,    -1,    -1,    -1,    -1,     3,     4,
      -1,    -1,    -1,    62,     9,    10,    11,    12,    13,    14,
      -1,    -1,    -1,    -1,    19,    -1,    -1,    -1,    -1,    24,
      25,    -1,    -1,    28,    29,    -1,    -1,    32,    -1,    -1,
      35,    36,    37,    -1,    -1,    -1,    -1,    42,    43,    -1,
      -1,    -1,    47,    -1,    49,    -1,    -1,    52,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    62
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    64,    65,     0,     3,     4,     9,    10,    11,    12,
      13,    14,    19,    24,    25,    28,    29,    32,    35,    36,
      37,    42,    43,    47,    49,    52,    62,    66,    67,    73,
      76,    84,    85,    89,    90,    98,    99,   100,   101,   102,
     103,   104,   105,   106,   107,   108,   109,   110,   111,   115,
      13,   111,   111,    11,    47,    11,    11,    77,    78,   111,
     111,    47,    47,    47,    47,   111,    49,   110,    11,    91,
      92,    93,    94,   110,   112,   113,   114,   111,    46,    27,
      26,    56,    57,    58,     8,    23,    21,    22,    59,    60,
      30,    31,     3,     4,     5,     6,     7,    46,    61,    24,
      25,    47,    51,    52,    47,    11,    95,    96,    97,    49,
      20,    51,    83,   110,   110,    11,   110,    74,    48,    50,
      54,    55,    53,    54,   101,   111,   102,   103,   104,   105,
     106,   106,   107,   107,   107,   107,   108,   108,   109,   109,
     111,   111,   111,   110,   112,    11,   110,    95,    48,    54,
      87,    11,    78,    48,    46,    41,    48,    40,    50,    67,
      73,    75,    76,    81,   110,    93,   110,   114,    48,    53,
      48,    49,    97,    15,    16,    17,    50,    86,    88,    49,
     110,   110,    49,    82,   110,    44,    46,    46,    46,    49,
      79,    13,    84,    71,    46,    48,    71,    47,    79,    50,
      67,    73,    76,    80,    81,   110,    50,    67,    72,    73,
      76,    81,   110,   110,    49,    50,    11,    50,    46,    46,
      46,    33,    34,    68,    69,    70,    46,    46,    46,    48,
      71,    48,    47,    49,    69,    70,    49,    50,    49,   110,
      71,    71,    74,    48,    50,    50,    50,    49,    71,    50
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    63,    64,    65,    65,    66,    66,    66,    66,    66,
      66,    67,    67,    67,    67,    67,    67,    67,    68,    68,
      69,    70,    71,    71,    72,    72,    72,    72,    72,    73,
      74,    74,    75,    75,    75,    75,    75,    76,    77,    77,
      78,    79,    79,    80,    80,    80,    80,    80,    81,    82,
      82,    83,    83,    84,    85,    86,    86,    86,    87,    87,
      88,    89,    90,    90,    90,    90,    90,    90,    91,    91,
      92,    92,    93,    94,    95,    95,    96,    96,    97,    98,
      98,    98,    98,    98,    99,   100,   100,   101,   101,   102,
     102,   103,   103,   104,   104,   105,   105,   105,   106,   106,
     106,   106,   106,   107,   107,   107,   108,   108,   108,   109,
     109,   109,   109,   110,   110,   111,   111,   111,   111,   111,
     111,   111,   112,   112,   113,   113,   114,   115,   115,   115,
     115,   115,   115
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     2,     0,     2,     1,     2,     1,     1,
       1,     7,     8,     8,     9,     7,    11,     9,     2,     1,
       7,     4,     2,     0,     2,     2,     1,     1,     2,    11,
       2,     0,     2,     2,     1,     1,     2,     3,     1,     3,
       1,     2,     0,     2,     2,     1,     1,     2,     2,     1,
       0,     2,     0,     8,     7,     1,     1,     1,     2,     0,
       2,     5,     1,     1,     3,     3,     3,     1,     1,     0,
       1,     3,     3,     1,     1,     0,     1,     3,     1,     1,
       1,     1,     1,     1,     1,     1,     3,     1,     3,     1,
       3,     1,     3,     1,     3,     1,     3,     3,     1,     3,
       3,     3,     3,     1,     3,     3,     1,     3,     3,     1,
       3,     3,     3,     1,     3,     1,     2,     2,     2,     2,
       2,     2,     1,     0,     1,     3,     1,     1,     4,     4,
       3,     2,     2
};


#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)
#define YYEMPTY         (-2)
#define YYEOF           0

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                  \
do                                                              \
  if (yychar == YYEMPTY)                                        \
    {                                                           \
      yychar = (Token);                                         \
      yylval = (Value);                                         \
      YYPOPSTACK (yylen);                                       \
      yystate = *yyssp;                                         \
      goto yybackup;                                            \
    }                                                           \
  else                                                          \
    {                                                           \
      yyerror (&yylloc, pstate, scanner, YY_("syntax error: cannot back up")); \
      YYERROR;                                                  \
    }                                                           \
while (0)

/* Error token number */
#define YYTERROR        1
#define YYERRCODE       256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)                                \
    do                                                                  \
      if (N)                                                            \
        {                                                               \
          (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;        \
          (Current).first_column = YYRHSLOC (Rhs, 1).first_column;      \
          (Current).last_line    = YYRHSLOC (Rhs, N).last_line;         \
          (Current).last_column  = YYRHSLOC (Rhs, N).last_column;       \
        }                                                               \
      else                                                              \
        {                                                               \
          (Current).first_line   = (Current).last_line   =              \
            YYRHSLOC (Rhs, 0).last_line;                                \
          (Current).first_column = (Current).last_column =              \
            YYRHSLOC (Rhs, 0).last_column;                              \
        }                                                               \
    while (0)
#endif

#define YYRHSLOC(Rhs, K) ((Rhs)[K])


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL

/* Print *YYLOCP on YYO.  Private, do not rely on its existence. */

YY_ATTRIBUTE_UNUSED
static unsigned
yy_location_print_ (FILE *yyo, YYLTYPE const * const yylocp)
{
  unsigned res = 0;
  int end_col = 0 != yylocp->last_column ? yylocp->last_column - 1 : 0;
  if (0 <= yylocp->first_line)
    {
      res += YYFPRINTF (yyo, "%d", yylocp->first_line);
      if (0 <= yylocp->first_column)
        res += YYFPRINTF (yyo, ".%d", yylocp->first_column);
    }
  if (0 <= yylocp->last_line)
    {
      if (yylocp->first_line < yylocp->last_line)
        {
          res += YYFPRINTF (yyo, "-%d", yylocp->last_line);
          if (0 <= end_col)
            res += YYFPRINTF (yyo, ".%d", end_col);
        }
      else if (0 <= end_col && yylocp->first_column < end_col)
        res += YYFPRINTF (yyo, "-%d", end_col);
    }
  return res;
 }

#  define YY_LOCATION_PRINT(File, Loc)          \
  yy_location_print_ (File, &(Loc))

# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


# define YY_SYMBOL_PRINT(Title, Type, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Type, Value, Location, pstate, scanner); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*----------------------------------------.
| Print this symbol's value on YYOUTPUT.  |
`----------------------------------------*/

static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp, ana_parser_state *pstate, yyscan_t scanner)
{
  FILE *yyo = yyoutput;
  YYUSE (yyo);
  YYUSE (yylocationp);
  YYUSE (pstate);
  YYUSE (scanner);
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# endif
  YYUSE (yytype);
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp, ana_parser_state *pstate, yyscan_t scanner)
{
  YYFPRINTF (yyoutput, "%s %s (",
             yytype < YYNTOKENS ? "token" : "nterm", yytname[yytype]);

  YY_LOCATION_PRINT (yyoutput, *yylocationp);
  YYFPRINTF (yyoutput, ": ");
  yy_symbol_value_print (yyoutput, yytype, yyvaluep, yylocationp, pstate, scanner);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yytype_int16 *yyssp, YYSTYPE *yyvsp, YYLTYPE *yylsp, int yyrule, ana_parser_state *pstate, yyscan_t scanner)
{
  unsigned long int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       yystos[yyssp[yyi + 1 - yynrhs]],
                       &(yyvsp[(yyi + 1) - (yynrhs)])
                       , &(yylsp[(yyi + 1) - (yynrhs)])                       , pstate, scanner);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, yylsp, Rule, pstate, scanner); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif


#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
static YYSIZE_T
yystrlen (const char *yystr)
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
yystpcpy (char *yydest, const char *yysrc)
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
        switch (*++yyp)
          {
          case '\'':
          case ',':
            goto do_not_strip_quotes;

          case '\\':
            if (*++yyp != '\\')
              goto do_not_strip_quotes;
            /* Fall through.  */
          default:
            if (yyres)
              yyres[yyn] = *yyp;
            yyn++;
            break;

          case '"':
            if (yyres)
              yyres[yyn] = '\0';
            return yyn;
          }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return 1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return 2 if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYSIZE_T *yymsg_alloc, char **yymsg,
                yytype_int16 *yyssp, int yytoken)
{
  YYSIZE_T yysize0 = yytnamerr (YY_NULLPTR, yytname[yytoken]);
  YYSIZE_T yysize = yysize0;
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULLPTR;
  /* Arguments of yyformat. */
  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Number of reported tokens (one for the "unexpected", one per
     "expected"). */
  int yycount = 0;

  /* There are many possibilities here to consider:
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yytoken != YYEMPTY)
    {
      int yyn = yypact[*yyssp];
      yyarg[yycount++] = yytname[yytoken];
      if (!yypact_value_is_default (yyn))
        {
          /* Start YYX at -YYN if negative to avoid negative indexes in
             YYCHECK.  In other words, skip the first -YYN actions for
             this state because they are default actions.  */
          int yyxbegin = yyn < 0 ? -yyn : 0;
          /* Stay within bounds of both yycheck and yytname.  */
          int yychecklim = YYLAST - yyn + 1;
          int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
          int yyx;

          for (yyx = yyxbegin; yyx < yyxend; ++yyx)
            if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR
                && !yytable_value_is_error (yytable[yyx + yyn]))
              {
                if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                  {
                    yycount = 1;
                    yysize = yysize0;
                    break;
                  }
                yyarg[yycount++] = yytname[yyx];
                {
                  YYSIZE_T yysize1 = yysize + yytnamerr (YY_NULLPTR, yytname[yyx]);
                  if (! (yysize <= yysize1
                         && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
                    return 2;
                  yysize = yysize1;
                }
              }
        }
    }

  switch (yycount)
    {
# define YYCASE_(N, S)                      \
      case N:                               \
        yyformat = S;                       \
      break
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
# undef YYCASE_
    }

  {
    YYSIZE_T yysize1 = yysize + yystrlen (yyformat);
    if (! (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
      return 2;
    yysize = yysize1;
  }

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return 1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yyarg[yyi++]);
          yyformat += 2;
        }
      else
        {
          yyp++;
          yyformat++;
        }
  }
  return 0;
}
#endif /* YYERROR_VERBOSE */

/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep, YYLTYPE *yylocationp, ana_parser_state *pstate, yyscan_t scanner)
{
  YYUSE (yyvaluep);
  YYUSE (yylocationp);
  YYUSE (pstate);
  YYUSE (scanner);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE (yytype);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}




/*----------.
| yyparse.  |
`----------*/

int
yyparse (ana_parser_state *pstate, yyscan_t scanner)
{
/* The lookahead symbol.  */
int yychar;


/* The semantic value of the lookahead symbol.  */
/* Default value used for initialization, for pacifying older GCCs
   or non-GCC compilers.  */
YY_INITIAL_VALUE (static YYSTYPE yyval_default;)
YYSTYPE yylval YY_INITIAL_VALUE (= yyval_default);

/* Location data for the lookahead symbol.  */
static YYLTYPE yyloc_default
# if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL
  = { 1, 1, 1, 1 }
# endif
;
YYLTYPE yylloc = yyloc_default;

    /* Number of syntax errors so far.  */
    int yynerrs;

    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       'yyss': related to states.
       'yyvs': related to semantic values.
       'yyls': related to locations.

       Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    /* The location stack.  */
    YYLTYPE yylsa[YYINITDEPTH];
    YYLTYPE *yyls;
    YYLTYPE *yylsp;

    /* The locations where the error started and ended.  */
    YYLTYPE yyerror_range[3];

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken = 0;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;
  YYLTYPE yyloc;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N), yylsp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yyssp = yyss = yyssa;
  yyvsp = yyvs = yyvsa;
  yylsp = yyls = yylsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */

/* User initialization code.  */

{
  /* To get rid of $undefined */
  /* Do stuff here for things to do before the parsing starts */
}


  yylsp[0] = yylloc;
  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        YYSTYPE *yyvs1 = yyvs;
        yytype_int16 *yyss1 = yyss;
        YYLTYPE *yyls1 = yyls;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * sizeof (*yyssp),
                    &yyvs1, yysize * sizeof (*yyvsp),
                    &yyls1, yysize * sizeof (*yylsp),
                    &yystacksize);

        yyls = yyls1;
        yyss = yyss1;
        yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yytype_int16 *yyss1 = yyss;
        union yyalloc *yyptr =
          (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
        if (! yyptr)
          goto yyexhaustedlab;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
        YYSTACK_RELOCATE (yyls_alloc, yyls);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;
      yylsp = yyls + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
                  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = yylex (&yylval, &yylloc, scanner);
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token.  */
  yychar = YYEMPTY;

  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END
  *++yylsp = yylloc;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];

  /* Default location.  */
  YYLLOC_DEFAULT (yyloc, (yylsp - yylen), yylen);
  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 2:

    { pstate->ast = (yyvsp[0].ast); }

    break;

  case 3:

    { (yyval.ast) = add_child(pstate, (yyvsp[-1].ast), (yyvsp[0].ast)); }

    break;

  case 4:

    { (yyval.ast) = program_node(pstate);      }

    break;

  case 5:

    { (yyval.ast) = (yyvsp[-1].ast); }

    break;

  case 6:

    { (yyval.ast) = (yyvsp[0].ast); }

    break;

  case 7:

    { (yyval.ast) = (yyvsp[-1].ast); }

    break;

  case 8:

    { (yyval.ast) = (yyvsp[0].ast); }

    break;

  case 9:

    { (yyval.ast) = (yyvsp[0].ast); }

    break;

  case 10:

    { (yyval.ast) = (yyvsp[0].ast); }

    break;

  case 11:

    { 
    (yyval.ast) = list_node(pstate, COMO_AST_IF);
    add_child(pstate, (yyval.ast), (yyvsp[-4].ast));
    add_child(pstate, (yyval.ast), (yyvsp[-1].ast));
  }

    break;

  case 12:

    {
    (yyval.ast) = list_node(pstate, COMO_AST_IF);
    add_child(pstate, (yyval.ast), (yyvsp[-5].ast));
    add_child(pstate, (yyval.ast), (yyvsp[-2].ast));  
    add_child(pstate, (yyval.ast), (yyvsp[0].ast));
  }

    break;

  case 13:

    { 
    (yyval.ast) = list_node(pstate, COMO_AST_IF);
    add_child(pstate, (yyval.ast), (yyvsp[-5].ast));
    add_child(pstate, (yyval.ast), (yyvsp[-2].ast));
    add_child(pstate, (yyval.ast), (yyvsp[0].ast));
  }

    break;

  case 14:

    {
    (yyval.ast) = list_node(pstate, COMO_AST_IF);
    add_child(pstate, (yyval.ast), (yyvsp[-6].ast));
    add_child(pstate, (yyval.ast), (yyvsp[-3].ast));  
    add_child(pstate, (yyval.ast), (yyvsp[-1].ast));
    add_child(pstate, (yyval.ast), (yyvsp[0].ast));
  }

    break;

  case 15:

    {
    (yyval.ast) = list_node(pstate, COMO_AST_WHILE);
    add_child(pstate, (yyval.ast), (yyvsp[-4].ast));
    add_child(pstate, (yyval.ast), (yyvsp[-1].ast));     
  }

    break;

  case 16:

    {
    (yyval.ast) = list_node(pstate, COMO_AST_FOR);
    add_child(pstate, (yyval.ast), (yyvsp[-8].ast));
    add_child(pstate, (yyval.ast), (yyvsp[-6].ast));
    add_child(pstate, (yyval.ast), (yyvsp[-4].ast));     
    add_child(pstate, (yyval.ast), (yyvsp[-1].ast));
  }

    break;

  case 17:

    {
    (yyval.ast) = list_node(pstate, COMO_AST_FOREACH);
    add_child(pstate, (yyval.ast), id_node(pstate, (yyvsp[-6].id)));
    add_child(pstate, (yyval.ast), (yyvsp[-4].ast));
    add_child(pstate, (yyval.ast), (yyvsp[-1].ast));
  }

    break;

  case 18:

    { 
    (yyval.ast) = add_child(pstate, (yyvsp[-1].ast), (yyvsp[0].ast)); 
  }

    break;

  case 19:

    { 
    (yyval.ast) = list_node(pstate, COMO_AST_ELSE_IF); 
  }

    break;

  case 20:

    {
    (yyval.ast) = list_node(pstate, COMO_AST_ELSE_IF);
    add_child(pstate, (yyval.ast), (yyvsp[-4].ast));
    add_child(pstate, (yyval.ast), (yyvsp[-1].ast));
  }

    break;

  case 21:

    {
    (yyval.ast) = list_node(pstate, COMO_AST_ELSE);
    add_child(pstate, (yyval.ast), (yyvsp[-1].ast));
  }

    break;

  case 22:

    { (yyval.ast) = add_child(pstate, (yyvsp[-1].ast), (yyvsp[0].ast)); }

    break;

  case 23:

    { (yyval.ast) = list_node(pstate, COMO_AST_LIST);}

    break;

  case 24:

    { (yyval.ast) = (yyvsp[-1].ast); }

    break;

  case 25:

    { (yyval.ast) = (yyvsp[-1].ast); }

    break;

  case 26:

    { (yyval.ast) = (yyvsp[0].ast); }

    break;

  case 27:

    { (yyval.ast) = (yyvsp[0].ast); }

    break;

  case 28:

    { (yyval.ast) = (yyvsp[-1].ast); }

    break;

  case 29:

    {
    (yyval.ast) = list_node(pstate, COMO_AST_TRY);
    add_child(pstate, (yyval.ast), (yyvsp[-8].ast));
    add_child(pstate, (yyval.ast), id_node(pstate, (yyvsp[-4].id)));
    add_child(pstate, (yyval.ast), (yyvsp[-1].ast));
  }

    break;

  case 30:

    { (yyval.ast) = (yyvsp[-1].ast);
    add_child(pstate, (yyvsp[-1].ast), (yyvsp[0].ast));
  }

    break;

  case 31:

    { (yyval.ast) = list_node(pstate, COMO_AST_LIST); }

    break;

  case 32:

    { (yyval.ast) = (yyvsp[-1].ast); }

    break;

  case 33:

    { (yyval.ast) = (yyvsp[-1].ast); }

    break;

  case 34:

    { (yyval.ast) = (yyvsp[0].ast); }

    break;

  case 35:

    { (yyval.ast) = (yyvsp[0].ast); }

    break;

  case 36:

    { (yyval.ast) = (yyvsp[-1].ast); }

    break;

  case 37:

    {
    (yyval.ast) = import_node(pstate, (yyvsp[-1].ast));
    add_child(pstate, (yyval.ast), (yyvsp[0].ast));
  }

    break;

  case 38:

    { 
    (yyval.ast) = list_node(pstate, COMO_AST_LIST); 
    add_child(pstate, (yyval.ast), (yyvsp[0].ast));
  }

    break;

  case 39:

    { 
    (yyval.ast) = (yyvsp[-2].ast);
    add_child(pstate, (yyvsp[-2].ast), (yyvsp[0].ast));
  }

    break;

  case 40:

    { 
    (yyval.ast) = id_node(pstate, (yyvsp[0].id)); 
  }

    break;

  case 41:

    { 
    (yyval.ast) = (yyvsp[-1].ast); 
    add_child(pstate, (yyval.ast), (yyvsp[0].ast));
  }

    break;

  case 42:

    { 
    (yyval.ast) = list_node(pstate, COMO_AST_LIST); 
  }

    break;

  case 43:

    { (yyval.ast) = (yyvsp[-1].ast); }

    break;

  case 44:

    { (yyval.ast) = (yyvsp[-1].ast); }

    break;

  case 45:

    { (yyval.ast) = (yyvsp[0].ast); }

    break;

  case 46:

    { (yyval.ast) = (yyvsp[0].ast); }

    break;

  case 47:

    { (yyval.ast) = (yyvsp[-1].ast); }

    break;

  case 48:

    {
    (yyval.ast) = return_node(pstate, (yyvsp[0].ast));
  }

    break;

  case 49:

    { (yyval.ast) = (yyvsp[0].ast); }

    break;

  case 50:

    { (yyval.ast) = NULL; }

    break;

  case 51:

    {
    (yyval.ast) = id_node(pstate, (yyvsp[0].id));
  }

    break;

  case 52:

    { 
    (yyval.ast) = NULL; 
  }

    break;

  case 53:

    { ;
    (yyval.ast) = function_node(pstate, (yyvsp[-6].id), (yyvsp[-4].ast), (yyvsp[-1].ast)); 
  }

    break;

  case 54:

    { 
    (yyval.ast) = function_node(pstate, "<anonymous>", (yyvsp[-4].ast), (yyvsp[-1].ast));
  }

    break;

  case 55:

    {
    (yyval.token) = COMO_AST_PUBLIC;
  }

    break;

  case 56:

    {
    (yyval.token) = COMO_AST_PROTECTED;
  }

    break;

  case 57:

    {
    (yyval.token) = COMO_AST_PRIVATE;
  }

    break;

  case 58:

    { 
    (yyval.ast) = (yyvsp[-1].ast);  
    add_child(pstate, (yyval.ast), (yyvsp[0].ast)); 
  }

    break;

  case 59:

    { 
    (yyval.ast) = class_stmts_node(pstate); 
  }

    break;

  case 60:

    { 
    (yyval.ast) = class_method_node(pstate, (yyvsp[-1].token), (yyvsp[0].ast));
  }

    break;

  case 61:

    { 
    (yyval.ast) = class_node(pstate, (yyvsp[-3].id), (yyvsp[-1].ast)); 
  }

    break;

  case 62:

    {
    (yyval.ast) = id_node(pstate, (yyvsp[0].id)); 
  }

    break;

  case 63:

    { 
    (yyval.ast) = (yyvsp[0].ast);                                
  }

    break;

  case 64:

    { 
    (yyval.ast) = (yyvsp[-1].ast);                      
   }

    break;

  case 65:

    { 
    (yyval.ast) = object_node(pstate, (yyvsp[-1].ast));                  
  }

    break;

  case 66:

    { 
    (yyval.ast) = array_node(pstate, (yyvsp[-1].ast)); 
  }

    break;

  case 67:

    { (yyval.ast) = (yyvsp[0].ast); }

    break;

  case 68:

    { 
    (yyval.ast) = (yyvsp[0].ast); 
  }

    break;

  case 69:

    { 
    (yyval.ast) = list_node(pstate, COMO_AST_LIST); 
  }

    break;

  case 70:

    {     
    (yyval.ast) = list_node(pstate, COMO_AST_LIST);
    add_child(pstate, (yyval.ast), (yyvsp[0].ast));
  }

    break;

  case 71:

    { 
    add_child(pstate, (yyvsp[-2].ast), (yyvsp[0].ast));
    (yyval.ast) = (yyvsp[-2].ast); 
  }

    break;

  case 72:

    { 
    (yyval.ast) = list_node(pstate, COMO_AST_LIST);
    add_child(pstate, (yyval.ast), (yyvsp[-2].ast));
    add_child(pstate, (yyval.ast), (yyvsp[0].ast));
  }

    break;

  case 73:

    { (yyval.ast) = string_node(pstate, (yyvsp[0].id)); }

    break;

  case 74:

    { (yyval.ast) = (yyvsp[0].ast); }

    break;

  case 75:

    { (yyval.ast) = list_node(pstate, COMO_AST_LIST); }

    break;

  case 76:

    { 
    (yyval.ast) = list_node(pstate, COMO_AST_LIST);
    add_child(pstate, (yyval.ast), (yyvsp[0].ast));
  }

    break;

  case 77:

    { 
    add_child(pstate, (yyvsp[-2].ast), (yyvsp[0].ast));
    (yyval.ast) = (yyvsp[-2].ast);
 }

    break;

  case 78:

    { 
    (yyval.ast) = id_node(pstate, (yyvsp[0].id));
  }

    break;

  case 79:

    { (yyval.ast) = int_node(pstate, (yyvsp[0].lval));           }

    break;

  case 80:

    { (yyval.ast) = double_node(pstate, (yyvsp[0].dval));        }

    break;

  case 81:

    { (yyval.ast) = string_node(pstate, (yyvsp[0].stringliteral));        }

    break;

  case 82:

    { (yyval.ast) = bool_node(pstate, 1);           }

    break;

  case 83:

    { (yyval.ast) = bool_node(pstate, 0);           }

    break;

  case 84:

    { (yyval.ast) = (yyvsp[0].ast); }

    break;

  case 85:

    { (yyval.ast) = (yyvsp[0].ast); }

    break;

  case 86:

    { (yyval.ast) = (yyvsp[-2].ast); }

    break;

  case 87:

    { (yyval.ast) = (yyvsp[0].ast); }

    break;

  case 88:

    { (yyval.ast) = (yyvsp[-2].ast); }

    break;

  case 89:

    { (yyval.ast) = (yyvsp[0].ast); }

    break;

  case 90:

    { (yyval.ast) = (yyvsp[-2].ast); }

    break;

  case 91:

    { (yyval.ast) = (yyvsp[0].ast); }

    break;

  case 92:

    { (yyval.ast) = (yyvsp[-2].ast); }

    break;

  case 93:

    { (yyval.ast) = (yyvsp[0].ast); }

    break;

  case 94:

    { (yyval.ast) = (yyvsp[-2].ast); }

    break;

  case 95:

    { (yyval.ast) = (yyvsp[0].ast); }

    break;

  case 96:

    { 
    (yyval.ast) = binary_op(pstate, COMO_AST_EQUAL);
    add_child(pstate, (yyval.ast), (yyvsp[-2].ast));
    add_child(pstate, (yyval.ast), (yyvsp[0].ast));
  }

    break;

  case 97:

    {
    (yyval.ast) = binary_op(pstate, COMO_AST_NOT_EQUAL);
    add_child(pstate, (yyval.ast), (yyvsp[-2].ast));
    add_child(pstate, (yyval.ast), (yyvsp[0].ast));
  }

    break;

  case 98:

    { (yyval.ast) = (yyvsp[0].ast); }

    break;

  case 99:

    { (yyval.ast) = (yyvsp[-2].ast); }

    break;

  case 100:

    { (yyval.ast) = (yyvsp[-2].ast); }

    break;

  case 101:

    { 
    (yyval.ast) = binary_op(pstate, COMO_AST_LTE);
    add_child(pstate, (yyval.ast), (yyvsp[-2].ast));
    add_child(pstate, (yyval.ast), (yyvsp[0].ast));
  }

    break;

  case 102:

    { (yyval.ast) = (yyvsp[-2].ast); }

    break;

  case 103:

    { (yyval.ast) = (yyvsp[0].ast); }

    break;

  case 104:

    { (yyval.ast) = (yyvsp[-2].ast); }

    break;

  case 105:

    { (yyval.ast) = (yyvsp[-2].ast); }

    break;

  case 106:

    { (yyval.ast) = (yyvsp[0].ast); }

    break;

  case 107:

    { 
    (yyval.ast) = binary_op(pstate, COMO_AST_ADD);
    add_child(pstate, (yyval.ast), (yyvsp[-2].ast));
    add_child(pstate, (yyval.ast), (yyvsp[0].ast));
  }

    break;

  case 108:

    { 
    (yyval.ast) = binary_op(pstate, COMO_AST_SUB);
    add_child(pstate, (yyval.ast), (yyvsp[-2].ast));
    add_child(pstate, (yyval.ast), (yyvsp[0].ast)); 
  }

    break;

  case 109:

    { (yyval.ast) = (yyvsp[0].ast); }

    break;

  case 110:

    { 
    (yyval.ast) = binary_op(pstate, COMO_AST_MUL);
    add_child(pstate, (yyval.ast), (yyvsp[-2].ast));
    add_child(pstate, (yyval.ast), (yyvsp[0].ast));
  }

    break;

  case 111:

    { 
    (yyval.ast) = binary_op(pstate, COMO_AST_DIV);
    add_child(pstate, (yyval.ast), (yyvsp[-2].ast));
    add_child(pstate, (yyval.ast), (yyvsp[0].ast));
  }

    break;

  case 112:

    { 
    (yyval.ast) = binary_op(pstate, COMO_AST_REM);
    add_child(pstate, (yyval.ast), (yyvsp[-2].ast));
    add_child(pstate, (yyval.ast), (yyvsp[0].ast));
  }

    break;

  case 113:

    { (yyval.ast) = (yyvsp[0].ast); }

    break;

  case 114:

    { 
    (yyval.ast) = binary_op(pstate, COMO_AST_ASSIGN);
    add_child(pstate, (yyval.ast), (yyvsp[-2].ast));
    add_child(pstate, (yyval.ast), (yyvsp[0].ast));
  }

    break;

  case 115:

    { (yyval.ast) = (yyvsp[0].ast); }

    break;

  case 116:

    { (yyval.ast) = throw_node(pstate, (yyvsp[0].ast)); }

    break;

  case 117:

    { (yyval.ast) = (yyvsp[0].ast); }

    break;

  case 118:

    { (yyval.ast) = (yyvsp[0].ast); }

    break;

  case 119:

    { 
    (yyval.ast) = unary_node(pstate, (yyvsp[0].ast), COMO_AST_UNARY_MINUS);
  }

    break;

  case 120:

    { 
    (yyval.ast) = unary_node(pstate, (yyvsp[0].ast), COMO_AST_UNARY_PLUS); 
  }

    break;

  case 121:

    { 
    (yyval.ast) = unary_node(pstate, (yyvsp[0].ast), COMO_AST_UNARY_NOT); 
  }

    break;

  case 122:

    { (yyval.ast) = (yyvsp[0].ast);                               }

    break;

  case 123:

    { (yyval.ast) = list_node(pstate, COMO_AST_LIST); }

    break;

  case 124:

    { 
    (yyval.ast) = list_node(pstate, COMO_AST_LIST);
    add_child(pstate, (yyval.ast), (yyvsp[0].ast));
  }

    break;

  case 125:

    { 
    add_child(pstate, (yyvsp[-2].ast), (yyvsp[0].ast));
    (yyval.ast) = (yyvsp[-2].ast);
  }

    break;

  case 126:

    { (yyval.ast) = (yyvsp[0].ast); }

    break;

  case 127:

    { (yyval.ast) = (yyvsp[0].ast); }

    break;

  case 128:

    { 
    (yyval.ast) = index_node(pstate, (yyvsp[-3].ast), (yyvsp[-1].ast));
  }

    break;

  case 129:

    { 
    (yyval.ast) = call_node(pstate, (yyvsp[-3].ast), (yyvsp[-1].ast));
  }

    break;

  case 130:

    { 
    (yyval.ast) = property_node(pstate, (yyvsp[-2].ast), (yyvsp[0].id));
  }

    break;

  case 131:

    { (yyval.ast) = (yyvsp[-1].ast); }

    break;

  case 132:

    { (yyval.ast) = (yyvsp[-1].ast); }

    break;



      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;
  *++yylsp = yyloc;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYEMPTY : YYTRANSLATE (yychar);

  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (&yylloc, pstate, scanner, YY_("syntax error"));
#else
# define YYSYNTAX_ERROR yysyntax_error (&yymsg_alloc, &yymsg, \
                                        yyssp, yytoken)
      {
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = YYSYNTAX_ERROR;
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == 1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = (char *) YYSTACK_ALLOC (yymsg_alloc);
            if (!yymsg)
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = 2;
              }
            else
              {
                yysyntax_error_status = YYSYNTAX_ERROR;
                yymsgp = yymsg;
              }
          }
        yyerror (&yylloc, pstate, scanner, yymsgp);
        if (yysyntax_error_status == 2)
          goto yyexhaustedlab;
      }
# undef YYSYNTAX_ERROR
#endif
    }

  yyerror_range[1] = yylloc;

  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval, &yylloc, pstate, scanner);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  yyerror_range[1] = yylsp[1-yylen];
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYTERROR;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;

      yyerror_range[1] = *yylsp;
      yydestruct ("Error: popping",
                  yystos[yystate], yyvsp, yylsp, pstate, scanner);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  yyerror_range[2] = yylloc;
  /* Using YYLLOC is tempting, but would change the location of
     the lookahead.  YYLOC is available though.  */
  YYLLOC_DEFAULT (yyloc, yyerror_range, 2);
  *++yylsp = yyloc;

  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#if !defined yyoverflow || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (&yylloc, pstate, scanner, YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval, &yylloc, pstate, scanner);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  yystos[*yyssp], yyvsp, yylsp, pstate, scanner);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  return yyresult;
}



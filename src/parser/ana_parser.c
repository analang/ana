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
    T_NEW = 265,
    T_LTE = 266,
    T_GTE = 267,
    T_NEQ = 268,
    T_INC = 269,
    T_DEC = 270,
    T_LA = 271,
    T_LO = 272,
    T_BT = 273,
    T_BF = 274,
    T_LS = 275,
    T_RS = 276,
    T_VARARG = 277,
    T_PLUS_EQUAL = 278,
    T_IF = 279,
    T_ELSE_IF = 280,
    T_ELSE = 281,
    T_FOR = 282,
    T_FOREACH = 283,
    T_WHILE = 284,
    T_CONTINUE = 285,
    T_BREAK = 286,
    T_RETURN = 287,
    T_IN = 288,
    T_THROW = 289,
    T_TRY = 290,
    T_CATCH = 291,
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
#define T_LTE 266
#define T_GTE 267
#define T_NEQ 268
#define T_INC 269
#define T_DEC 270
#define T_LA 271
#define T_LO 272
#define T_BT 273
#define T_BF 274
#define T_LS 275
#define T_RS 276
#define T_VARARG 277
#define T_PLUS_EQUAL 278
#define T_IF 279
#define T_ELSE_IF 280
#define T_ELSE 281
#define T_FOR 282
#define T_FOREACH 283
#define T_WHILE 284
#define T_CONTINUE 285
#define T_BREAK 286
#define T_RETURN 287
#define T_IN 288
#define T_THROW 289
#define T_TRY 290
#define T_CATCH 291
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
#define YYLAST   742

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  60
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  50
/* YYNRULES -- Number of rules.  */
#define YYNRULES  131
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  249

/* YYTRANSLATE[YYX] -- Symbol number corresponding to YYX as returned
   by yylex, with out-of-bounds checking.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   291

#define YYTRANSLATE(YYX)                                                \
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, without out-of-bounds checking.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,    42,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    58,     2,     2,     2,     7,    54,     2,
      44,    45,     5,     3,    51,     4,    59,     6,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    48,    43,
      55,    57,    56,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    49,     2,    50,    53,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    46,    52,    47,     2,     2,     2,     2,
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
      40,    41
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   132,   132,   136,   137,   141,   142,   143,   144,   145,
     146,   150,   158,   162,   172,   190,   196,   204,   213,   217,
     224,   234,   241,   244,   249,   250,   251,   252,   253,   257,
     263,   272,   275,   278,   279,   280,   281,   282,   287,   291,
     297,   298,   299,   300,   301,   305,   308,   311,   317,   318,
     322,   325,   330,   336,   344,   348,   353,   359,   365,   368,
     371,   374,   377,   380,   384,   387,   393,   397,   404,   412,
     413,   417,   418,   422,   426,   433,   436,   442,   443,   444,
     445,   446,   450,   454,   455,   463,   464,   472,   473,   481,
     482,   490,   491,   499,   500,   505,   510,   518,   519,   524,
     529,   534,   542,   543,   548,   556,   557,   562,   570,   571,
     576,   581,   589,   590,   595,   603,   607,   610,   613,   616,
     619,   625,   626,   630,   634,   641,   645,   648,   651,   654,
     657,   660
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
  "\"keyword new\"", "\"<=\"", "\">=\"", "\"!=\"", "\"++\"", "\"--\"",
  "\"&&\"", "\"||\"", "\"true literal\"", "\"false literal\"", "\"<<\"",
  "\">>\"", "\"...\"", "\"+=\"", "\"keyword if\"", "\"keyword else if\"",
  "\"keyword else\"", "\"keyword for\"", "\"keyword foreach\"",
  "\"keyword while\"", "\"keyword continue\"", "\"keyword break\"",
  "\"keyword return\"", "\"keyword in\"", "\"keyword throw\"",
  "\"keyword try\"", "\"keyword catch\"", "\"character\"", "';'", "'('",
  "')'", "'{'", "'}'", "':'", "'['", "']'", "','", "'|'", "'^'", "'&'",
  "'<'", "'>'", "'='", "'!'", "'.'", "$accept", "program", "statements",
  "statement", "selection_stmt", "else_if_stmts", "elseif", "else_stmt",
  "if_statements", "if_statement", "throw_stmt", "trycatch_stmt",
  "stmt_group", "stmt", "function_statements", "function_statement",
  "jump_statement", "optional_assignment_expression", "optional_extends",
  "function_def", "function_expression", "class_statements",
  "class_statement", "class_def", "primary_expression", "opt_key_vpairs",
  "key_vpairs", "keyvpair", "key", "optional_parameter_list",
  "function_params", "function_param", "constant", "basic_expression",
  "logical_or_expression", "logical_and_expression",
  "inclusive_or_expression", "exclusive_or_expression", "and_expression",
  "equality_expression", "relational_expression", "shift_expression",
  "additive_expression", "multiplicative_expression",
  "assignment_expression", "unary_expression", "optional_argument_list",
  "argument_list", "argument", "postfix_expression", YY_NULLPTR
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
     290,   291,     2,    59,    40,    41,   123,   125,    58,    91,
      93,    44,   124,    94,    38,    60,    62,    61,    33,    46
};
# endif

#define YYPACT_NINF -172

#define yypact_value_is_default(Yystate) \
  (!!((Yystate) == (-172)))

#define YYTABLE_NINF -1

#define yytable_value_is_error(Yytable_value) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
    -172,    19,   684,  -172,    25,    25,  -172,  -172,  -172,  -172,
      -4,    10,    25,    25,  -172,  -172,   -38,   -13,    -3,    26,
      25,    17,    25,     4,    25,    25,  -172,  -172,    30,  -172,
    -172,  -172,  -172,  -172,  -172,  -172,    68,    56,    39,    40,
      38,     5,    -5,   -16,    62,    47,    53,   -11,    13,    57,
    -172,  -172,    58,     3,    49,  -172,  -172,    25,    25,    36,
      25,  -172,  -172,    59,  -172,  -172,    60,    52,  -172,    61,
    -172,    50,    54,  -172,  -172,  -172,    25,    25,    25,    25,
      25,    25,    25,    25,    25,    25,    25,    25,    25,    25,
      25,    25,    25,    25,    25,  -172,    25,    25,  -172,  -172,
      25,    25,    95,     3,  -172,    97,    65,    67,  -172,   100,
      66,    75,    78,    84,    79,   170,  -172,  -172,     4,    25,
    -172,    25,    56,  -172,    39,    40,    38,     5,    -5,    -5,
      -5,   -16,   -16,   -16,   -16,    62,    62,    47,    47,  -172,
    -172,  -172,  -172,  -172,    80,    73,  -172,    81,  -172,    82,
       3,  -172,  -172,    83,    25,    25,   104,  -172,  -172,    25,
      86,  -172,   108,  -172,  -172,   111,   112,  -172,  -172,  -172,
    -172,  -172,   110,  -172,  -172,    -8,  -172,   115,   114,  -172,
    -172,  -172,   116,  -172,  -172,  -172,  -172,   234,   150,  -172,
    -172,  -172,   284,    25,   117,   334,   151,   384,  -172,  -172,
     121,  -172,  -172,   122,   123,    48,  -172,  -172,   124,  -172,
     125,   126,   127,  -172,  -172,   130,  -172,  -172,  -172,  -172,
     132,   131,    48,  -172,  -172,  -172,  -172,  -172,   138,   434,
     146,    25,  -172,  -172,  -172,  -172,  -172,  -172,   133,   484,
     534,   584,   154,  -172,  -172,  -172,  -172,   634,  -172
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       4,     0,     2,     1,     0,     0,    77,    78,    58,    79,
       0,     0,     0,     0,    80,    81,     0,     0,     0,     0,
       0,     0,     0,    65,   122,     0,     3,     9,     0,     8,
       6,    63,     7,   126,    59,   112,    82,    83,    85,    87,
      89,    91,    93,    97,   102,   105,     0,   108,   115,     0,
     119,   118,     0,    72,    51,   117,   116,     0,     0,     0,
       0,    29,    32,     0,    69,    70,     0,    64,    66,     0,
     125,     0,   121,   123,   120,    10,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     5,     0,     0,   130,   131,
     122,     0,     0,    72,    75,     0,     0,    71,    73,     0,
       0,     0,     0,     0,     0,     0,    60,    61,     0,     0,
      62,     0,    84,   108,    86,    88,    90,    92,    94,    95,
      96,   100,   101,    98,    99,   103,   104,   106,   107,   109,
     110,   111,   114,   113,     0,     0,   129,     0,    76,     0,
       0,    50,    55,     0,     0,     0,     0,    47,    46,    49,
       0,    35,     0,    34,    31,     0,     0,    67,    68,   124,
     128,   127,     0,    39,    74,     0,    23,     0,     0,    23,
      45,    48,     0,    37,    36,    33,    39,     0,     0,    57,
      56,    54,     0,     0,     0,     0,     0,     0,    53,    42,
       0,    41,    38,     0,     0,    11,    26,    22,     0,    25,
       0,     0,     0,    23,    15,     0,    52,    44,    43,    40,
       0,     0,    12,    19,    13,    28,    27,    24,     0,     0,
       0,     0,    23,    18,    14,    23,    17,    32,     0,     0,
       0,     0,     0,    21,    16,    30,    23,     0,    20
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -172,  -172,  -172,  -172,    -1,  -172,   -52,   -51,  -171,  -172,
       0,     1,   -36,  -172,    22,  -172,  -111,  -172,  -172,    37,
    -172,  -172,  -172,  -172,  -172,  -172,  -172,    93,  -172,   118,
    -172,    63,  -172,  -172,  -172,   139,   141,   142,   143,   144,
       6,   -60,    -7,    -6,    -2,    55,   134,  -172,   102,  -172
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     1,     2,    26,   206,   222,   223,   224,   192,   207,
     208,   209,   115,   164,   187,   202,   210,   180,   110,    30,
      31,   175,   191,    32,    33,    66,    67,    68,    69,   106,
     107,   108,    34,    35,    36,    37,    38,    39,    40,    41,
      42,    43,    44,    45,   211,    47,    71,    72,    73,    48
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_uint8 yytable[] =
{
      46,    27,    28,    29,   165,   188,    57,    52,   195,    88,
      89,    84,    85,    81,   104,    64,    65,    96,    61,     3,
      63,    54,    70,    82,   131,   132,   133,   134,     4,     5,
     105,    58,    98,    99,     6,     7,     8,     9,    49,   189,
      53,    59,   229,    83,    12,    13,    97,   113,    14,    15,
      86,    87,    92,    93,    94,   111,   112,   100,   114,    50,
      51,   239,   101,    62,   240,    90,    91,    55,    56,    22,
      60,    23,   102,    75,    24,   247,   203,    77,   220,   221,
      74,   135,   136,    25,   137,   138,   203,   128,   129,   130,
      76,    78,    80,    79,   142,   143,    95,   109,    70,   145,
     120,    53,   103,   118,   116,   121,   146,   117,   148,   119,
     149,   151,   152,   166,   161,   162,   163,   168,   150,    70,
     153,   154,   155,   171,   156,   170,   172,   182,   173,   176,
     165,   123,   123,   123,   123,   123,   123,   123,   123,   123,
     123,   123,   123,   123,   123,   123,   123,   139,   140,   141,
     179,   183,   177,   178,   184,   185,   186,   181,   193,   194,
     196,    52,   215,   213,   217,   218,   219,   225,   226,   227,
     233,   234,   228,     4,     5,   230,   231,   232,   242,     6,
       7,     8,     9,    49,   235,   204,   199,   200,   201,    12,
      13,   212,   237,    14,    15,   204,   199,   200,   201,    16,
     246,   241,    17,    18,    19,   157,   158,   159,   197,    20,
      21,   167,   190,   174,    22,   122,    23,   160,   124,    24,
     125,   147,   126,   169,   127,     0,     0,     0,    25,   238,
       0,     0,     0,     0,   144,     0,     0,     4,     5,   166,
     161,   162,   163,     6,     7,     8,     9,    49,     0,     0,
       0,     0,     0,    12,    13,     0,     0,    14,    15,     0,
       0,     0,     0,    16,     0,     0,    17,    18,    19,   157,
     158,   159,     0,    20,    21,     0,     0,     0,    22,     0,
      23,   198,     0,    24,     0,     0,     0,     4,     5,     0,
       0,     0,    25,     6,     7,     8,     9,    49,     0,     0,
       0,     0,     0,    12,    13,     0,     0,    14,    15,     0,
       0,     0,     0,    16,     0,     0,    17,    18,    19,   157,
     158,   159,     0,    20,    21,     0,     0,     0,    22,     0,
      23,   205,     0,    24,     0,     0,     0,     4,     5,     0,
       0,     0,    25,     6,     7,     8,     9,    49,     0,     0,
       0,     0,     0,    12,    13,     0,     0,    14,    15,     0,
       0,     0,     0,    16,     0,     0,    17,    18,    19,   157,
     158,   159,     0,    20,    21,     0,     0,     0,    22,     0,
      23,   214,     0,    24,     0,     0,     0,     4,     5,     0,
       0,     0,    25,     6,     7,     8,     9,    49,     0,     0,
       0,     0,     0,    12,    13,     0,     0,    14,    15,     0,
       0,     0,     0,    16,     0,     0,    17,    18,    19,   157,
     158,   159,     0,    20,    21,     0,     0,     0,    22,     0,
      23,   216,     0,    24,     0,     0,     0,     4,     5,     0,
       0,     0,    25,     6,     7,     8,     9,    49,     0,     0,
       0,     0,     0,    12,    13,     0,     0,    14,    15,     0,
       0,     0,     0,    16,     0,     0,    17,    18,    19,   157,
     158,   159,     0,    20,    21,     0,     0,     0,    22,     0,
      23,   236,     0,    24,     0,     0,     0,     4,     5,     0,
       0,     0,    25,     6,     7,     8,     9,    49,     0,     0,
       0,     0,     0,    12,    13,     0,     0,    14,    15,     0,
       0,     0,     0,    16,     0,     0,    17,    18,    19,   157,
     158,   159,     0,    20,    21,     0,     0,     0,    22,     0,
      23,   243,     0,    24,     0,     0,     0,     4,     5,     0,
       0,     0,    25,     6,     7,     8,     9,    49,     0,     0,
       0,     0,     0,    12,    13,     0,     0,    14,    15,     0,
       0,     0,     0,    16,     0,     0,    17,    18,    19,   157,
     158,   159,     0,    20,    21,     0,     0,     0,    22,     0,
      23,   244,     0,    24,     0,     0,     0,     4,     5,     0,
       0,     0,    25,     6,     7,     8,     9,    49,     0,     0,
       0,     0,     0,    12,    13,     0,     0,    14,    15,     0,
       0,     0,     0,    16,     0,     0,    17,    18,    19,   157,
     158,   159,     0,    20,    21,     0,     0,     0,    22,     0,
      23,   245,     0,    24,     0,     0,     0,     4,     5,     0,
       0,     0,    25,     6,     7,     8,     9,    49,     0,     0,
       0,     0,     0,    12,    13,     0,     0,    14,    15,     0,
       0,     0,     0,    16,     0,     0,    17,    18,    19,   157,
     158,   159,     0,    20,    21,     0,     0,     0,    22,     0,
      23,   248,     0,    24,     0,     0,     0,     4,     5,     0,
       0,     0,    25,     6,     7,     8,     9,    10,    11,     0,
       0,     0,     0,    12,    13,     0,     0,    14,    15,     0,
       0,     0,     0,    16,     0,     0,    17,    18,    19,     0,
       0,     0,     0,    20,    21,     0,     0,     0,    22,     0,
      23,     0,     0,    24,     0,     0,     0,     0,     0,     0,
       0,     0,    25
};

static const yytype_int16 yycheck[] =
{
       2,     2,     2,     2,   115,    13,    44,    11,   179,    25,
      26,    16,    17,     8,    11,    11,    12,    28,    20,     0,
      22,    11,    24,    18,    84,    85,    86,    87,     3,     4,
      27,    44,    19,    20,     9,    10,    11,    12,    13,    47,
      44,    44,   213,    38,    19,    20,    57,    11,    23,    24,
      55,    56,     5,     6,     7,    57,    58,    44,    60,     4,
       5,   232,    49,    46,   235,     3,     4,    12,    13,    44,
      44,    46,    59,    43,    49,   246,   187,    21,    30,    31,
      25,    88,    89,    58,    90,    91,   197,    81,    82,    83,
      22,    52,    54,    53,    96,    97,    43,    48,   100,   101,
      50,    44,    44,    51,    45,    51,    11,    47,    11,    48,
      45,    11,    46,   115,   115,   115,   115,   119,    51,   121,
      45,    43,    38,    50,    45,    45,    45,    41,    46,    46,
     241,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      46,    43,   154,   155,    43,    43,    46,   159,    43,    45,
      44,    11,    11,    46,    43,    43,    43,    43,    43,    43,
     222,   222,    45,     3,     4,    45,    44,    46,    45,     9,
      10,    11,    12,    13,    46,   187,   187,   187,   187,    19,
      20,   193,    46,    23,    24,   197,   197,   197,   197,    29,
      46,   237,    32,    33,    34,    35,    36,    37,   186,    39,
      40,   118,   175,   150,    44,    76,    46,    47,    77,    49,
      78,   103,    79,   121,    80,    -1,    -1,    -1,    58,   231,
      -1,    -1,    -1,    -1,   100,    -1,    -1,     3,     4,   241,
     241,   241,   241,     9,    10,    11,    12,    13,    -1,    -1,
      -1,    -1,    -1,    19,    20,    -1,    -1,    23,    24,    -1,
      -1,    -1,    -1,    29,    -1,    -1,    32,    33,    34,    35,
      36,    37,    -1,    39,    40,    -1,    -1,    -1,    44,    -1,
      46,    47,    -1,    49,    -1,    -1,    -1,     3,     4,    -1,
      -1,    -1,    58,     9,    10,    11,    12,    13,    -1,    -1,
      -1,    -1,    -1,    19,    20,    -1,    -1,    23,    24,    -1,
      -1,    -1,    -1,    29,    -1,    -1,    32,    33,    34,    35,
      36,    37,    -1,    39,    40,    -1,    -1,    -1,    44,    -1,
      46,    47,    -1,    49,    -1,    -1,    -1,     3,     4,    -1,
      -1,    -1,    58,     9,    10,    11,    12,    13,    -1,    -1,
      -1,    -1,    -1,    19,    20,    -1,    -1,    23,    24,    -1,
      -1,    -1,    -1,    29,    -1,    -1,    32,    33,    34,    35,
      36,    37,    -1,    39,    40,    -1,    -1,    -1,    44,    -1,
      46,    47,    -1,    49,    -1,    -1,    -1,     3,     4,    -1,
      -1,    -1,    58,     9,    10,    11,    12,    13,    -1,    -1,
      -1,    -1,    -1,    19,    20,    -1,    -1,    23,    24,    -1,
      -1,    -1,    -1,    29,    -1,    -1,    32,    33,    34,    35,
      36,    37,    -1,    39,    40,    -1,    -1,    -1,    44,    -1,
      46,    47,    -1,    49,    -1,    -1,    -1,     3,     4,    -1,
      -1,    -1,    58,     9,    10,    11,    12,    13,    -1,    -1,
      -1,    -1,    -1,    19,    20,    -1,    -1,    23,    24,    -1,
      -1,    -1,    -1,    29,    -1,    -1,    32,    33,    34,    35,
      36,    37,    -1,    39,    40,    -1,    -1,    -1,    44,    -1,
      46,    47,    -1,    49,    -1,    -1,    -1,     3,     4,    -1,
      -1,    -1,    58,     9,    10,    11,    12,    13,    -1,    -1,
      -1,    -1,    -1,    19,    20,    -1,    -1,    23,    24,    -1,
      -1,    -1,    -1,    29,    -1,    -1,    32,    33,    34,    35,
      36,    37,    -1,    39,    40,    -1,    -1,    -1,    44,    -1,
      46,    47,    -1,    49,    -1,    -1,    -1,     3,     4,    -1,
      -1,    -1,    58,     9,    10,    11,    12,    13,    -1,    -1,
      -1,    -1,    -1,    19,    20,    -1,    -1,    23,    24,    -1,
      -1,    -1,    -1,    29,    -1,    -1,    32,    33,    34,    35,
      36,    37,    -1,    39,    40,    -1,    -1,    -1,    44,    -1,
      46,    47,    -1,    49,    -1,    -1,    -1,     3,     4,    -1,
      -1,    -1,    58,     9,    10,    11,    12,    13,    -1,    -1,
      -1,    -1,    -1,    19,    20,    -1,    -1,    23,    24,    -1,
      -1,    -1,    -1,    29,    -1,    -1,    32,    33,    34,    35,
      36,    37,    -1,    39,    40,    -1,    -1,    -1,    44,    -1,
      46,    47,    -1,    49,    -1,    -1,    -1,     3,     4,    -1,
      -1,    -1,    58,     9,    10,    11,    12,    13,    -1,    -1,
      -1,    -1,    -1,    19,    20,    -1,    -1,    23,    24,    -1,
      -1,    -1,    -1,    29,    -1,    -1,    32,    33,    34,    35,
      36,    37,    -1,    39,    40,    -1,    -1,    -1,    44,    -1,
      46,    47,    -1,    49,    -1,    -1,    -1,     3,     4,    -1,
      -1,    -1,    58,     9,    10,    11,    12,    13,    14,    -1,
      -1,    -1,    -1,    19,    20,    -1,    -1,    23,    24,    -1,
      -1,    -1,    -1,    29,    -1,    -1,    32,    33,    34,    -1,
      -1,    -1,    -1,    39,    40,    -1,    -1,    -1,    44,    -1,
      46,    -1,    -1,    49,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    58
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    61,    62,     0,     3,     4,     9,    10,    11,    12,
      13,    14,    19,    20,    23,    24,    29,    32,    33,    34,
      39,    40,    44,    46,    49,    58,    63,    64,    70,    71,
      79,    80,    83,    84,    92,    93,    94,    95,    96,    97,
      98,    99,   100,   101,   102,   103,   104,   105,   109,    13,
     105,   105,    11,    44,    11,   105,   105,    44,    44,    44,
      44,   104,    46,   104,    11,    12,    85,    86,    87,    88,
     104,   106,   107,   108,   105,    43,    22,    21,    52,    53,
      54,     8,    18,    38,    16,    17,    55,    56,    25,    26,
       3,     4,     5,     6,     7,    43,    28,    57,    19,    20,
      44,    49,    59,    44,    11,    27,    89,    90,    91,    48,
      78,   104,   104,    11,   104,    72,    45,    47,    51,    48,
      50,    51,    95,   105,    96,    97,    98,    99,   100,   100,
     100,   101,   101,   101,   101,   102,   102,   103,   103,   105,
     105,   105,   104,   104,   106,   104,    11,    89,    11,    45,
      51,    11,    46,    45,    43,    38,    45,    35,    36,    37,
      47,    64,    70,    71,    73,    76,   104,    87,   104,   108,
      45,    50,    45,    46,    91,    81,    46,   104,   104,    46,
      77,   104,    41,    43,    43,    43,    46,    74,    13,    47,
      79,    82,    68,    43,    45,    68,    44,    74,    47,    64,
      70,    71,    75,    76,   104,    47,    64,    69,    70,    71,
      76,   104,   104,    46,    47,    11,    47,    43,    43,    43,
      30,    31,    65,    66,    67,    43,    43,    43,    45,    68,
      45,    44,    46,    66,    67,    46,    47,    46,   104,    68,
      68,    72,    45,    47,    47,    47,    46,    68,    47
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    60,    61,    62,    62,    63,    63,    63,    63,    63,
      63,    64,    64,    64,    64,    64,    64,    64,    65,    65,
      66,    67,    68,    68,    69,    69,    69,    69,    69,    70,
      71,    72,    72,    73,    73,    73,    73,    73,    74,    74,
      75,    75,    75,    75,    75,    76,    76,    76,    77,    77,
      78,    78,    79,    80,    81,    81,    82,    83,    84,    84,
      84,    84,    84,    84,    85,    85,    86,    86,    87,    88,
      88,    89,    89,    90,    90,    91,    91,    92,    92,    92,
      92,    92,    93,    94,    94,    95,    95,    96,    96,    97,
      97,    98,    98,    99,    99,    99,    99,   100,   100,   100,
     100,   100,   101,   101,   101,   102,   102,   102,   103,   103,
     103,   103,   104,   104,   104,   105,   105,   105,   105,   105,
     105,   106,   106,   107,   107,   108,   109,   109,   109,   109,
     109,   109
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     2,     0,     2,     1,     1,     1,     1,
       2,     7,     8,     8,     9,     7,    11,     9,     2,     1,
       7,     4,     2,     0,     2,     1,     1,     2,     2,     2,
      11,     2,     0,     2,     1,     1,     2,     2,     2,     0,
       2,     1,     1,     2,     2,     2,     1,     1,     1,     0,
       2,     0,     8,     7,     2,     0,     1,     6,     1,     1,
       3,     3,     3,     1,     1,     0,     1,     3,     3,     1,
       1,     1,     0,     1,     3,     1,     2,     1,     1,     1,
       1,     1,     1,     1,     3,     1,     3,     1,     3,     1,
       3,     1,     3,     1,     3,     3,     3,     1,     3,     3,
       3,     3,     1,     3,     3,     1,     3,     3,     1,     3,
       3,     3,     1,     3,     3,     1,     2,     2,     2,     2,
       2,     1,     0,     1,     3,     1,     1,     4,     4,     3,
       2,     2
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

    { (yyval.ast) = (yyvsp[0].ast); }

    break;

  case 8:

    { (yyval.ast) = (yyvsp[0].ast); }

    break;

  case 9:

    { (yyval.ast) = (yyvsp[0].ast); }

    break;

  case 10:

    { (yyval.ast) = (yyvsp[-1].ast); }

    break;

  case 11:

    {
    (yyval.ast) = list_node(pstate, COMO_AST_IF);
    // assignment_expression
    add_child(pstate, (yyval.ast), (yyvsp[-4].ast));
    // if_statement suite
    add_child(pstate, (yyval.ast), (yyvsp[-1].ast));
  }

    break;

  case 12:

    {
    (yyval.ast) = compound_if_node(pstate, (yyvsp[-5].ast), (yyvsp[-2].ast), (yyvsp[0].ast), NULL);
  }

    break;

  case 13:

    { 
    (yyval.ast) = list_node(pstate, COMO_AST_IF);
    // assignment_expression
    add_child(pstate, (yyval.ast), (yyvsp[-5].ast));
    // statement_suite
    add_child(pstate, (yyval.ast), (yyvsp[-2].ast));
    // optional else_stmt
    add_child(pstate, (yyval.ast), (yyvsp[0].ast));
  }

    break;

  case 14:

    {
    //$$ = list_node(pstate, COMO_AST_COMPOUND_IF_STATEMENT);
    
    (yyval.ast) = compound_if_node(pstate, (yyvsp[-6].ast), (yyvsp[-3].ast), (yyvsp[-1].ast), (yyvsp[0].ast));

    // assignment_expression
    //add_child(pstate, $$, $3);

    //if_statements
    //add_child(pstate, $$, $6);  
  
    // else_if_stmts
    //add_child(pstate, $$, $8);

    // else_stmt
    //add_child(pstate, $$, $9);
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
    (yyval.ast) = (yyvsp[-1].ast);
    add_child(pstate, (yyval.ast), (yyvsp[0].ast)); 
  }

    break;

  case 19:

    { 
    (yyval.ast) = list_node(pstate, COMO_AST_ELSE_IF_SUITE); 
    add_child(pstate, (yyval.ast), (yyvsp[0].ast));
  }

    break;

  case 20:

    {
    (yyval.ast) = list_node(pstate, COMO_AST_ELSE_IF);
    // assignment expression
    add_child(pstate, (yyval.ast), (yyvsp[-4].ast));
    // statements
    add_child(pstate, (yyval.ast), (yyvsp[-1].ast));
  }

    break;

  case 21:

    {
    (yyval.ast) = list_node(pstate, COMO_AST_ELSE_SUITE);
    add_child(pstate, (yyval.ast), (yyvsp[-1].ast));
  }

    break;

  case 22:

    {
    (yyval.ast) = add_child(pstate, (yyvsp[-1].ast), (yyvsp[0].ast)); 
  }

    break;

  case 23:

    { 
    (yyval.ast) = list_node(pstate, COMO_AST_LIST);
  }

    break;

  case 24:

    { (yyval.ast) = (yyvsp[-1].ast); }

    break;

  case 25:

    { (yyval.ast) = (yyvsp[0].ast); }

    break;

  case 26:

    { (yyval.ast) = (yyvsp[0].ast); }

    break;

  case 27:

    { (yyval.ast) = (yyvsp[-1].ast); }

    break;

  case 28:

    { (yyval.ast) = (yyvsp[-1].ast); }

    break;

  case 29:

    { 
    (yyval.ast) = throw_node(pstate, (yyvsp[0].ast)); 
  }

    break;

  case 30:

    {
    (yyval.ast) = list_node(pstate, COMO_AST_TRY);
    add_child(pstate, (yyval.ast), (yyvsp[-8].ast));
    add_child(pstate, (yyval.ast), id_node(pstate, (yyvsp[-4].id)));
    add_child(pstate, (yyval.ast), (yyvsp[-1].ast));
  }

    break;

  case 31:

    { (yyval.ast) = (yyvsp[-1].ast);
    add_child(pstate, (yyvsp[-1].ast), (yyvsp[0].ast));
  }

    break;

  case 32:

    { (yyval.ast) = list_node(pstate, COMO_AST_LIST); }

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

    { (yyval.ast) = (yyvsp[-1].ast); }

    break;

  case 38:

    { 
    (yyval.ast) = (yyvsp[-1].ast); 
    add_child(pstate, (yyval.ast), (yyvsp[0].ast));
  }

    break;

  case 39:

    { 
    (yyval.ast) = list_node(pstate, COMO_AST_LIST); 
  }

    break;

  case 40:

    { (yyval.ast) = (yyvsp[-1].ast); }

    break;

  case 41:

    { (yyval.ast) = (yyvsp[0].ast); }

    break;

  case 42:

    { (yyval.ast) = (yyvsp[0].ast); }

    break;

  case 43:

    { (yyval.ast) = (yyvsp[-1].ast); }

    break;

  case 44:

    { (yyval.ast) = (yyvsp[-1].ast); }

    break;

  case 45:

    {
    (yyval.ast) = return_node(pstate, (yyvsp[0].ast));
  }

    break;

  case 46:

    {
    (yyval.ast) = break_node(pstate);
  }

    break;

  case 47:

    {
    (yyval.ast) = continue_node(pstate);
}

    break;

  case 48:

    { (yyval.ast) = (yyvsp[0].ast); }

    break;

  case 49:

    { (yyval.ast) = NULL; }

    break;

  case 50:

    {
    (yyval.ast) = id_node(pstate, (yyvsp[0].id));
  }

    break;

  case 51:

    {
  (yyval.ast) = NULL;
}

    break;

  case 52:

    { ;
    (yyval.ast) = function_node(pstate, (yyvsp[-6].id), (yyvsp[-4].ast), (yyvsp[-1].ast)); 
  }

    break;

  case 53:

    { 
    printf("function expression are not implemented yet\n");
    exit(1);
    (yyval.ast) = function_node(pstate, "<anonymous>", (yyvsp[-4].ast), (yyvsp[-1].ast));
  }

    break;

  case 54:

    { 
    (yyval.ast) = (yyvsp[-1].ast);  
    add_child(pstate, (yyval.ast), (yyvsp[0].ast)); 
  }

    break;

  case 55:

    { 
    (yyval.ast) = class_stmts_node(pstate); 
  }

    break;

  case 56:

    { 
    (yyval.ast) = class_method_node(pstate, 0, (yyvsp[0].ast));
  }

    break;

  case 57:

    { 
    (yyval.ast) = class_node(pstate, (yyvsp[-4].id), (yyvsp[-3].ast), (yyvsp[-1].ast)); 
  }

    break;

  case 58:

    {
    (yyval.ast) = id_node(pstate, (yyvsp[0].id)); 
  }

    break;

  case 59:

    { 
    (yyval.ast) = (yyvsp[0].ast);                                
  }

    break;

  case 60:

    {
    (yyval.ast) = (yyvsp[-1].ast);                      
   }

    break;

  case 61:

    { 
    (yyval.ast) = object_node(pstate, (yyvsp[-1].ast));                  
  }

    break;

  case 62:

    { 
    (yyval.ast) = array_node(pstate, (yyvsp[-1].ast)); 
  }

    break;

  case 63:

    { (yyval.ast) = (yyvsp[0].ast); }

    break;

  case 64:

    { 
    (yyval.ast) = (yyvsp[0].ast); 
  }

    break;

  case 65:

    { 
    (yyval.ast) = list_node(pstate, COMO_AST_LIST); 
  }

    break;

  case 66:

    {     
    (yyval.ast) = list_node(pstate, COMO_AST_LIST);
    add_child(pstate, (yyval.ast), (yyvsp[0].ast));
  }

    break;

  case 67:

    { 
    add_child(pstate, (yyvsp[-2].ast), (yyvsp[0].ast));
    (yyval.ast) = (yyvsp[-2].ast); 
  }

    break;

  case 68:

    { 
    (yyval.ast) = list_node(pstate, COMO_AST_LIST);
    add_child(pstate, (yyval.ast), (yyvsp[-2].ast));
    add_child(pstate, (yyval.ast), (yyvsp[0].ast));
  }

    break;

  case 69:

    { (yyval.ast) = string_node(pstate, (yyvsp[0].id)); }

    break;

  case 70:

    { (yyval.ast) = string_node(pstate, (yyvsp[0].stringliteral)); }

    break;

  case 71:

    { (yyval.ast) = (yyvsp[0].ast); }

    break;

  case 72:

    { (yyval.ast) = list_node(pstate, COMO_AST_LIST); }

    break;

  case 73:

    { 
    (yyval.ast) = list_node(pstate, COMO_AST_LIST);
    add_child(pstate, (yyval.ast), (yyvsp[0].ast));
  }

    break;

  case 74:

    { 
    add_child(pstate, (yyvsp[-2].ast), (yyvsp[0].ast));
    (yyval.ast) = (yyvsp[-2].ast);
 }

    break;

  case 75:

    { 
    (yyval.ast) = id_node(pstate, (yyvsp[0].id));
  }

    break;

  case 76:

    {
    (yyval.ast) = vararg_node(pstate, (yyvsp[0].id));
  }

    break;

  case 77:

    { (yyval.ast) = int_node(pstate, (yyvsp[0].lval));           }

    break;

  case 78:

    { (yyval.ast) = double_node(pstate, (yyvsp[0].dval));        }

    break;

  case 79:

    { (yyval.ast) = string_node(pstate, (yyvsp[0].stringliteral));        }

    break;

  case 80:

    { (yyval.ast) = bool_node(pstate, 1);           }

    break;

  case 81:

    { (yyval.ast) = bool_node(pstate, 0);           }

    break;

  case 82:

    { (yyval.ast) = (yyvsp[0].ast); }

    break;

  case 83:

    { (yyval.ast) = (yyvsp[0].ast); }

    break;

  case 84:

    { 
    (yyval.ast) = binary_op(pstate, COMO_AST_OR);
    add_child(pstate, (yyval.ast), (yyvsp[-2].ast));
    add_child(pstate, (yyval.ast), (yyvsp[0].ast));
  }

    break;

  case 85:

    { (yyval.ast) = (yyvsp[0].ast); }

    break;

  case 86:

    { 
    (yyval.ast) = binary_op(pstate, COMO_AST_AND);
    add_child(pstate, (yyval.ast), (yyvsp[-2].ast));
    add_child(pstate, (yyval.ast), (yyvsp[0].ast));
  }

    break;

  case 87:

    { (yyval.ast) = (yyvsp[0].ast); }

    break;

  case 88:

    { 
    printf("| operator is not implemented yet\n");
    exit(1);
    (yyval.ast) = (yyvsp[-2].ast); 
  }

    break;

  case 89:

    { (yyval.ast) = (yyvsp[0].ast); }

    break;

  case 90:

    { 
    printf("binary ^ operator is not implemented yet\n");
    exit(1);
    (yyval.ast) = (yyvsp[-2].ast); 
  }

    break;

  case 91:

    { (yyval.ast) = (yyvsp[0].ast); }

    break;

  case 92:

    { 
    printf("binary & operator is not implemented yet\n");
    exit(1);
    (yyval.ast) = (yyvsp[-2].ast); 
  }

    break;

  case 93:

    { (yyval.ast) = (yyvsp[0].ast); }

    break;

  case 94:

    { 
    (yyval.ast) = binary_op(pstate, COMO_AST_EQUAL);
    add_child(pstate, (yyval.ast), (yyvsp[-2].ast));
    add_child(pstate, (yyval.ast), (yyvsp[0].ast));
  }

    break;

  case 95:

    {
    (yyval.ast) = binary_op(pstate, COMO_AST_NOT_EQUAL);
    add_child(pstate, (yyval.ast), (yyvsp[-2].ast));
    add_child(pstate, (yyval.ast), (yyvsp[0].ast));
  }

    break;

  case 96:

    {
    (yyval.ast) = binary_op(pstate, COMO_AST_IN);
    add_child(pstate, (yyval.ast), (yyvsp[-2].ast));
    add_child(pstate, (yyval.ast), (yyvsp[0].ast));   
  }

    break;

  case 97:

    { (yyval.ast) = (yyvsp[0].ast); }

    break;

  case 98:

    { 
    (yyval.ast) = binary_op(pstate, COMO_AST_LT);
    add_child(pstate, (yyval.ast), (yyvsp[-2].ast));
    add_child(pstate, (yyval.ast), (yyvsp[0].ast));
  }

    break;

  case 99:

    { 
    (yyval.ast) = binary_op(pstate, COMO_AST_GT);
    add_child(pstate, (yyval.ast), (yyvsp[-2].ast));
    add_child(pstate, (yyval.ast), (yyvsp[0].ast));
  }

    break;

  case 100:

    { 
    (yyval.ast) = binary_op(pstate, COMO_AST_LTE);
    add_child(pstate, (yyval.ast), (yyvsp[-2].ast));
    add_child(pstate, (yyval.ast), (yyvsp[0].ast));
  }

    break;

  case 101:

    { 
    (yyval.ast) = binary_op(pstate, COMO_AST_GTE);
    add_child(pstate, (yyval.ast), (yyvsp[-2].ast));
    add_child(pstate, (yyval.ast), (yyvsp[0].ast));
  }

    break;

  case 102:

    { (yyval.ast) = (yyvsp[0].ast); }

    break;

  case 103:

    { 
    (yyval.ast) = binary_op(pstate, COMO_AST_LEFT_SHIFT);
    add_child(pstate, (yyval.ast), (yyvsp[-2].ast));
    add_child(pstate, (yyval.ast), (yyvsp[0].ast)); 
  }

    break;

  case 104:

    { 
    (yyval.ast) = binary_op(pstate, COMO_AST_RIGHT_SHIFT);
    add_child(pstate, (yyval.ast), (yyvsp[-2].ast));
    add_child(pstate, (yyval.ast), (yyvsp[0].ast)); 
  }

    break;

  case 105:

    { (yyval.ast) = (yyvsp[0].ast); }

    break;

  case 106:

    { 
    (yyval.ast) = binary_op(pstate, COMO_AST_ADD);
    add_child(pstate, (yyval.ast), (yyvsp[-2].ast));
    add_child(pstate, (yyval.ast), (yyvsp[0].ast));
  }

    break;

  case 107:

    { 
    (yyval.ast) = binary_op(pstate, COMO_AST_SUB);
    add_child(pstate, (yyval.ast), (yyvsp[-2].ast));
    add_child(pstate, (yyval.ast), (yyvsp[0].ast)); 
  }

    break;

  case 108:

    { (yyval.ast) = (yyvsp[0].ast); }

    break;

  case 109:

    { 
    (yyval.ast) = binary_op(pstate, COMO_AST_MUL);
    add_child(pstate, (yyval.ast), (yyvsp[-2].ast));
    add_child(pstate, (yyval.ast), (yyvsp[0].ast));
  }

    break;

  case 110:

    { 
    (yyval.ast) = binary_op(pstate, COMO_AST_DIV);
    add_child(pstate, (yyval.ast), (yyvsp[-2].ast));
    add_child(pstate, (yyval.ast), (yyvsp[0].ast));
  }

    break;

  case 111:

    { 
    (yyval.ast) = binary_op(pstate, COMO_AST_REM);
    add_child(pstate, (yyval.ast), (yyvsp[-2].ast));
    add_child(pstate, (yyval.ast), (yyvsp[0].ast));
  }

    break;

  case 112:

    { (yyval.ast) = (yyvsp[0].ast); }

    break;

  case 113:

    { 
    (yyval.ast) = binary_op(pstate, COMO_AST_ASSIGN);
    add_child(pstate, (yyval.ast), (yyvsp[-2].ast));
    add_child(pstate, (yyval.ast), (yyvsp[0].ast));
  }

    break;

  case 114:

    {
    (yyval.ast) = binary_op(pstate, COMO_AST_PLUS_ASSIGN);
    add_child(pstate, (yyval.ast), (yyvsp[-2].ast));
    add_child(pstate, (yyval.ast), (yyvsp[0].ast));
  }

    break;

  case 115:

    { 
    (yyval.ast) = (yyvsp[0].ast); 
  }

    break;

  case 116:

    { 
    (yyval.ast) = unary_node(pstate, (yyvsp[0].ast), COMO_AST_PREFIXDEC);
  }

    break;

  case 117:

    { 
    (yyval.ast) = unary_node(pstate, (yyvsp[0].ast), COMO_AST_PREFIXINC);
  }

    break;

  case 118:

    { 
    (yyval.ast) = unary_node(pstate, (yyvsp[0].ast), COMO_AST_UNARY_MINUS);
  }

    break;

  case 119:

    { 
    (yyval.ast) = unary_node(pstate, (yyvsp[0].ast), COMO_AST_UNARY_PLUS); 
  }

    break;

  case 120:

    { 
    (yyval.ast) = unary_node(pstate, (yyvsp[0].ast), COMO_AST_UNARY_NOT); 
  }

    break;

  case 121:

    { (yyval.ast) = (yyvsp[0].ast);                               }

    break;

  case 122:

    { (yyval.ast) = list_node(pstate, COMO_AST_LIST); }

    break;

  case 123:

    { 
    (yyval.ast) = list_node(pstate, COMO_AST_LIST);
    add_child(pstate, (yyval.ast), (yyvsp[0].ast));
  }

    break;

  case 124:

    { 
    add_child(pstate, (yyvsp[-2].ast), (yyvsp[0].ast));
    (yyval.ast) = (yyvsp[-2].ast);
  }

    break;

  case 125:

    { (yyval.ast) = (yyvsp[0].ast); }

    break;

  case 126:

    { 
    (yyval.ast) = (yyvsp[0].ast); 
  }

    break;

  case 127:

    { 
    (yyval.ast) = index_node(pstate, (yyvsp[-3].ast), (yyvsp[-1].ast));
  }

    break;

  case 128:

    { 
    (yyval.ast) = call_node(pstate, (yyvsp[-3].ast), (yyvsp[-1].ast));
  }

    break;

  case 129:

    { 
    (yyval.ast) = property_node(pstate, (yyvsp[-2].ast), (yyvsp[0].id));
  }

    break;

  case 130:

    { 
    (yyval.ast) = postfix_node(pstate, (yyvsp[-1].ast), COMO_AST_POSTFIXINC); 
  }

    break;

  case 131:

    { 
    (yyval.ast) = postfix_node(pstate, (yyvsp[-1].ast), COMO_AST_POSTFIXDEC); 
  }

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



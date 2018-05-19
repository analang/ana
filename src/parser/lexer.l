%{
#include <stdio.h>
#include <string.h>
#include "ana_parser.h"
#include "ana_ast.h"
#define YY_EXTRA_TYPE ana_parser_state *
/*
 *https://westes.github.io/flex/manual/Scanner-Options.html#Scanner-Options
 *
 * https://westes.github.io/flex/manual/Overriding-The-Default-Memory-Management.html#Overriding-The-Default-Memory-Management
 * regex:
 * https://westes.github.io/flex/manual/Numbers.html#Numbers
 * identifiers:
 * https://westes.github.io/flex/manual/Identifiers.html#Identifiers
 * quoted literals:
 * https://westes.github.io/flex/manual/Quoted-Constructs.html#Quoted-Constructs
 *http://archive.oreilly.com/pub/a/linux/excerpts/9780596155971/error-reporting-recovery.html
 * https://stackoverflow.com/questions/20458469/flex-bison-eof-propagation-from-stdin-vs-a-file
 http://dinosaur.compilertools.net/flex/manpage.html
 */
/* https://stackoverflow.com/questions/656703/how-does-flex-support-bison-location-exactly */
 
/* fileno is not C99, so do this workaround, since we'll only use it known FILE*/

#ifndef fileno
#define fileno(fp) (fp == stdin ? STDIN_FILENO : -1)
#endif

#define YY_USER_INIT \
  yy_set_interactive(yyin == stdin); \
  yyextra->first_line = 1; \
  yyextra->first_column = 1; \
  yyextra->line = 0; \
  yyextra->last_column = 1; \
  yyextra->last_line = 1; \

#define YY_USER_ACTION \
  yyextra->first_line = yyextra->last_line; \
  yyextra->first_column = yyextra->last_column; \
  for(int i = 0; yytext[i] != '\0'; i++) { \
      if(yytext[i] == '\n') { \
          yyextra->last_line++; \
          yyextra->last_column = 1; \
      } \
      else { \
          yyextra->last_column++; \
      } \
  }
%}

%option warn nodefault stack
%option reentrant never-interactive
%option bison-bridge bison-locations yylineno
%option nounput noinput noyywrap unistd
%option noyy_top_state

 
WHITE_SPACE [ \r\n\t]*
L   [a-zA-Z_]
A   [a-zA-Z_0-9]
D   [0-9]

%x SINGLE_LINE_COMMENT

%% 

"//"                                yy_push_state(SINGLE_LINE_COMMENT, yyscanner);
<SINGLE_LINE_COMMENT>\n             yy_pop_state(yyscanner);
<SINGLE_LINE_COMMENT>[^\n]+


"as"        { return T_AS;        }
"function"  { return T_FUNCTION;  }
"func"      { return T_FUNCTION;  }
"class"     { return T_CLASS;     }
"public"    { return T_PUBLIC;    }
"private"   { return T_PRIVATE;   }
"protected" { return T_PROTECTED; }
"import"    { return T_IMPORT;    }
"throw"     { return T_THROW;     }
"new"       { return T_NEW;       }
"<="        { return T_LTE;       }
">="        { return T_GTE;       }
"++"        { return T_INC;       }
"--"        { return T_DEC;       }
"=="        { return T_EQ;        }
"!="        { return T_NEQ;       }
"&&"        { return T_LA;        }
"||"        { return T_LO;        }
"true"      { return T_BT;        }
"false"     { return T_BF;        }
"try"       { return T_TRY;       }
"catch"     { return T_CATCH;     }
"if"        { return T_IF;        }
"else if"   { return T_ELSE_IF;   }
"else"      { return T_ELSE;      }
"for"       { return T_FOR;       }
"foreach"   { return T_FOREACH;   }
"while"     { return T_WHILE;     }
"continue"  { return T_CONTINUE;  }
"break"     { return T_BREAK;     }
"return"    { return T_RETURN;    }
"in"        { return T_IN;        }
"<<"        { return T_LS;        }
">>"        { return T_RS;        }

{WHITE_SPACE} { /* Skipping Blanks Today */ }
{L}{A}*         {
  size_t len = strlen(yytext);
  yylval->id = ana_arena_alloc(yyextra->arena, len + 1);
  memcpy(yylval->id, yytext, len + 1);
  return T_ID;
}

{D}+  { 
  yylval->lval = strtol(yytext, NULL, 10); 
  return T_INT; 
}

{D}+"."{D}+ {
  yylval->dval = atof(yytext);
  return T_DOUBLE;
}

L?\"(\\.|[^\\"])*\" { 
/* TODO, make this a yy_push_state string literal scanner */
  size_t len = strlen(yytext);
  if(len > 2U) {
    yylval->stringliteral = ana_arena_alloc(yyextra->arena, (len -2) + 1);
    memcpy(yylval->stringliteral, yytext + 1, len - 2);
    yylval->stringliteral[len - 2] = '\0';
    return T_STR_LIT;
  } else {
    yylval->stringliteral = ana_arena_alloc(yyextra->arena, 1);
    yylval->stringliteral[0] = '\0';
    return T_STR_LIT;
  }
}

L?\'(\\.|[^\\'])*\' { 
  size_t len = strlen(yytext);
  if(len > 2U) {
    yylval->stringliteral = ana_arena_alloc(yyextra->arena, (len -2) + 1);
    memcpy(yylval->stringliteral, yytext + 1, len - 2);
    yylval->stringliteral[len - 2] = '\0';
    return T_STR_LIT;
  } else {
    yylval->stringliteral = ana_arena_alloc(yyextra->arena, 1);
    yylval->stringliteral[0] = '\0';
    return T_STR_LIT;
  }
}

<<EOF>> {
  return 0;
}

.   { return yytext[0];            }

%%

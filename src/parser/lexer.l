%{
#include <stdio.h>
#include <string.h>
#include <assert.h>
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

typedef struct ana_byte_buffer {
  size_t capacity;
  size_t position;
  char *buffer;
} ana_byte_buffer;

static struct ana_byte_buffer string_literal;

static inline int ana_byte_buffer_init(ana_byte_buffer *buffer)
{
  buffer->capacity = 8;
  buffer->position = 0;
  buffer->buffer = malloc(8);

  if(!buffer->buffer)
  {
    return 1;
  }

  return 0;
}

static inline int ana_byte_buffer_finalize(ana_byte_buffer *buffer)
{
  buffer->capacity = 0;
  buffer->position = 0;
  free(buffer->buffer);
  buffer->buffer = NULL; 

  return 0;
}

static inline int ana_byte_buffer_appendc(ana_byte_buffer *buffer,
  int c)
{
  if(buffer->position >= buffer->capacity)
  {
    buffer->capacity *= 2;
    buffer->buffer = realloc(buffer->buffer, buffer->capacity);

    if(!buffer->buffer)
    {
      return 1;
    }
  }

  buffer->buffer[buffer->position++] = c;

  return 0;
}

#define ANA_BYTE_BUFFER_APPENDC_SAFE(buffer, c) do { \
  if(ana_byte_buffer_appendc(&string_literal, c)) \
  { \
    fprintf(stdout, "memory error for ana_byte_buffer_put_appendc"); \
    exit(1); \
  } \
} while(0)


%}

%option warn stack
%option reentrant never-interactive
%option bison-bridge bison-locations yylineno
%option nounput noinput noyywrap unistd
%option noyy_top_state

WHITE_SPACE [ \r\n\t]*
L   [a-zA-Z_]
A   [a-zA-Z_0-9]
D   [0-9]

%x SINGLE_LINE_COMMENT
%x DOUBLE_QUOTED_STRING_LITERAL
%x SINGLE_QUOTED_STRING_LITERAL

%% 


"//"                                yy_push_state(SINGLE_LINE_COMMENT, yyscanner);
<SINGLE_LINE_COMMENT>\n             yy_pop_state(yyscanner);
<SINGLE_LINE_COMMENT>[^\n]+

[ \r\n\t]* { /* Skipping Blanks Today */ }

"function"  { return T_FUNCTION;  }
"func"      { return T_FUNCTION;  }
"class"     { return T_CLASS;     }
"throw"     { return T_THROW;     }
"new"       { return T_NEW;       }
"<="        { return T_LTE;       }
">="        { return T_GTE;       }
"++"        { return T_INC;       }
"+="        { return T_PLUS_EQUAL;}
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
"..."       { return T_VARARG;    }

\" {
  if(ana_byte_buffer_init(&string_literal))
  {
    fprintf(stdout, "malloc failed, for string literal allocatoion");
    exit(1);
  }

  BEGIN(DOUBLE_QUOTED_STRING_LITERAL);
}

\' {
  if(ana_byte_buffer_init(&string_literal))
  {
    fprintf(stdout, "malloc failed, for string literal allocatoion");
    exit(1);
  }
  
  BEGIN(SINGLE_QUOTED_STRING_LITERAL);
}

<DOUBLE_QUOTED_STRING_LITERAL>\" {
  BEGIN(INITIAL);

  if(ana_byte_buffer_appendc(&string_literal, '\0'))
  {
    fprintf(stdout, "memory error for ana_byte_buffer_put_appendc");
    exit(1);    
  }

  size_t len = string_literal.position;
  string_literal.position = 0;

  if(len > 0) {
    yylval->stringliteral = ana_arena_alloc(yyextra->arena, len);
    memcpy(yylval->stringliteral, string_literal.buffer, len);
  } else {
    yylval->stringliteral = ana_arena_alloc(yyextra->arena, 1);
    yylval->stringliteral[0] = '\0';
  }

  ana_byte_buffer_finalize(&string_literal);

  return T_STR_LIT;
}

<SINGLE_QUOTED_STRING_LITERAL>\' {
  BEGIN(INITIAL);

  if(ana_byte_buffer_appendc(&string_literal, '\0'))
  {
    fprintf(stdout, "memory error for ana_byte_buffer_put_appendc");
    exit(1);    
  }

  size_t len = string_literal.position;
  string_literal.position = 0;
  if(len > 0) {
    yylval->stringliteral = ana_arena_alloc(yyextra->arena, len);
    memcpy(yylval->stringliteral, string_literal.buffer, len);
  } else {
    yylval->stringliteral = ana_arena_alloc(yyextra->arena, 1);
    yylval->stringliteral[0] = '\0';
  }  

  ana_byte_buffer_finalize(&string_literal);

  return T_STR_LIT;
}

<DOUBLE_QUOTED_STRING_LITERAL,SINGLE_QUOTED_STRING_LITERAL>\n {
  printf("ana: unterminated string literal on line %d\n",
      yyextra->first_line);
  exit(1);
}
<DOUBLE_QUOTED_STRING_LITERAL,SINGLE_QUOTED_STRING_LITERAL>\\n {
  ANA_BYTE_BUFFER_APPENDC_SAFE(&string_literal, '\n');
}
<DOUBLE_QUOTED_STRING_LITERAL,SINGLE_QUOTED_STRING_LITERAL>\\a {
  ANA_BYTE_BUFFER_APPENDC_SAFE(&string_literal, '\a');
}
<DOUBLE_QUOTED_STRING_LITERAL,SINGLE_QUOTED_STRING_LITERAL>\\b {
  ANA_BYTE_BUFFER_APPENDC_SAFE(&string_literal, '\b');
}
<DOUBLE_QUOTED_STRING_LITERAL,SINGLE_QUOTED_STRING_LITERAL>\\f {
  ANA_BYTE_BUFFER_APPENDC_SAFE(&string_literal, '\f');
}
<DOUBLE_QUOTED_STRING_LITERAL,SINGLE_QUOTED_STRING_LITERAL>\\r {
  ANA_BYTE_BUFFER_APPENDC_SAFE(&string_literal, '\r');
}
<DOUBLE_QUOTED_STRING_LITERAL,SINGLE_QUOTED_STRING_LITERAL>\\t {
  ANA_BYTE_BUFFER_APPENDC_SAFE(&string_literal, '\t');
}
<DOUBLE_QUOTED_STRING_LITERAL,SINGLE_QUOTED_STRING_LITERAL>\\v {
  ANA_BYTE_BUFFER_APPENDC_SAFE(&string_literal, '\v');
}

<DOUBLE_QUOTED_STRING_LITERAL,SINGLE_QUOTED_STRING_LITERAL>\\(.|\n) {
  ANA_BYTE_BUFFER_APPENDC_SAFE(&string_literal, yytext[1]);
}

<DOUBLE_QUOTED_STRING_LITERAL>[^\\\n\"]+ {
  size_t i;

  for(i = 0; i < yyleng; i++)
  {
    ANA_BYTE_BUFFER_APPENDC_SAFE(&string_literal, yytext[i]);   
  }
}

<SINGLE_QUOTED_STRING_LITERAL>[^\\\n\']+ {
  size_t i;

  for(i = 0; i < yyleng; i++)
  {
    ANA_BYTE_BUFFER_APPENDC_SAFE(&string_literal, yytext[i]);   
  }
}


{L}{A}*  {
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

<<EOF>> {
  return 0;
}

.   { return yytext[0];            }

%%

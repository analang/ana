#define _GNU_SOURCE 1 /* For realpath */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
#include <getopt.h>
#include <ana.h>
#include <errno.h>
#include <limits.h>
#include <sys/stat.h>

#include "arena.h"
#include "ana_ast.h"
#include "compiler.h"
#include "utils.h"
#include "vm.h"
#include "anaio.h"

#define PROGRAM_NAME "ana"

typedef struct ana_options {
  int debug;
  int ast;
  int opcodes;
  int interactive;
  int command;
  char *cmdstr;
  char *function;
  int version;
  int help;
  char **argv;
  int argc;
  int error;
  int livetracing;
} ana_options;

const char *shorthelpstr = "\
Usage: " PROGRAM_NAME " [option] ... [-c <command> | <file> | -] [args]";

const char *helpstr = "\
options and arguments:\n\
  -a, --ast                 print the abstract syntax tree\n\
  -o, --opcodes             print the program bytecode representation\n\
  -i, --interactive         run a REPL\n\
  -f, --function            function to output bytecode for\n\
  -l, --live-tracing        execute the program with each opcode printed\n\
  -c, --command  <command>  execute the ana command\n\
  -v, --version             print the ana version\n\
  -h, --help                display this menu\n\
  -d, --debug               print C debugging messages to stderr\n\
  command                   a string of ana source code\n\
  file                      path to the file to be executed\n\
  args                      arguments passed to the ana program\n\
  -                         read from stdin and execute the text on EOF\n\
";

static int usage(int status)
{
  if(status != 0) 
  {
    fprintf(stdout, "%s\n", shorthelpstr);
    fprintf(stdout, "Try " PROGRAM_NAME " --help' for more information\n");
  } 
  else { 
    fprintf(stdout, "%s\n", shorthelpstr);
    fprintf(stdout, "%s", helpstr);
  }
  
  exit(status);
}

static struct option long_options[] = {
  { "debug",        no_argument,       NULL, 'd'},
  { "ast",          no_argument,       NULL, 'a'},
  { "opcodes",      no_argument,       NULL, 'o'},
  { "interactive",  no_argument,       NULL, 'i'},
  { "function",     required_argument, NULL, 'f'},
  { "command",      required_argument, NULL, 'c'},
  { "version",      no_argument,       NULL, 'v'},
  { "live-tracing", no_argument,       NULL, 'l'},
  { "help",         no_argument,       NULL, 'h'},
  {0, 0, 0, 0}
};

static ana_options parse_argv(int argc, char **argv)
{
  ana_options ret;
  int c;
  int option_index = 0;

  memset(&ret, 0, sizeof(ana_options));

  for(;;)
  {
    opterr = 1;

    c = getopt_long(argc, argv, "daoic:f:vhl", long_options, &option_index);

    if(c == -1)
      break;

    switch(c)
    { 
      case 0:
        break;
      case 'l':
        ret.livetracing = 1;
        break;
      case 'f':
        ret.function = ana_strdup(optarg);
        break;
      case 'd':
        ret.debug = 1;
        break;
      case 'a':
        ret.ast = 1;
        break;
      case 'o':
        ret.opcodes = 1;
        break;
      case 'i':
        ret.interactive = 1;
        break;
      case 'c':
        ret.command = 1;
        ret.cmdstr = optarg;
        break;
      case 'v':
        ret.version = 1;
        break;
      case 'h':
        ret.help = 1;
        break;
      case '?':
        ret.error = 1;
        goto exit;
      default:
        usage(1);
    }
  }

exit:
  if(optind < argc)
  {    
    ret.argc = argc - optind;
    ret.argv = argv + optind;        
  }

  return ret;
}

static void ana_free_opts(ana_options *opts)
{
  if(opts->function)
    free(opts->function);
}

static int run_file(ana_options *opts)
{
  /* TODO, resolve full file path */ 
  const char *filename = opts->argv[0];
  int retval;
  char *resolved_path = NULL;
  ana_parser_state state;
  FILE *fp = NULL;

  if(*filename == '-')
  {
    fp = stdin;
    filename = "<stdin>";
  }

  if(!fp)
  {
    fp = ana_open_file_for_parsing(filename);
    
    if(!fp)
    {
      retval = 1;
      fprintf(stderr, PROGRAM_NAME ": Can't open file \"%s\": %s\n", filename, 
        strerror(errno));
      goto exit;
    }

    resolved_path = realpath(filename, NULL);

    if(!resolved_path)
    {
      fprintf(stderr, PROGRAM_NAME ": Can't resolve file \"%s\" (%d)\n", 
        filename, errno);

      goto exit;
    }

    filename = resolved_path;
  }

  state.debug = opts->debug;
  state.arena = ana_arena_new();

  if(!state.arena)
  {
    retval = 1;
    fprintf(stderr, PROGRAM_NAME " : failed to acquire arena "
      "(ana_arena_new)\n");
    goto exit;
  }


  retval = ana_astfromfile(
    fp, filename, opts->argc, opts->argv, &state);

  if(retval != 0)
  {
    /* In the import statement, the runtime will actually, 
       throw a real SyntaxError exception and will be catchable by
       the calling module
     */
    fprintf(stdout, "SyntaxError: %s\n", state.error);
    fprintf(stdout, "    at %s:%d:%d \n", filename, state.first_line,
      state.first_column);

    /* reset file pointer */
    if(fseek(fp, 0, SEEK_SET)) 
    {
      if(opts->debug) 
      {
        fprintf(stderr, "warning: failed to set current position to start of "
          "file\n");
      }
    }
    else 
    {
      char c;
      char line[COMO_LINE_MAX];
      int i = 0;
      int curline = 0;
      while((c = fgetc(fp)) != EOF)
      {
        if(c == '\n')
        {
          curline++;
          if(curline == state.first_line) 
          {
            line[i] = '\0';
            
            char *start = line;
            char *linep = line;

            while(*linep == ' ')
              linep++;

            fprintf(stdout, "%s\n", linep);

            state.first_column = state.first_column - (linep - start);

            for(i = 1; i <= state.first_column; i++)
            {
              if(i == state.first_column) 
              {
                fprintf(stdout, "^\n");

                break;
              }
              else
                fputc(' ', stdout);
            }
            break;
          }
          i= 0;
        } 
        else
        {
          if(!(i < COMO_LINE_MAX - 1))
            line[i++] = '\0';
          else
            line[i++] = c;
        }
      }
    }
    
    fflush(stdout);
    fclose(fp);

    ana_arena_free(state.arena);
  } 
  else 
  {
    fclose(fp);
    ComoVM *vm = ana_vm_new();

    if(opts->livetracing)
      vm->flags |= COMO_VM_LIVE_TRACING;
    else if(opts->opcodes)
      vm->flags |= COMO_VM_TRACING;

    if(opts->ast)
    {
      visit(state.ast, 0);
    }
    else 
    {
      ana_object *code = ana_compileast((char *)filename, vm, state.ast);

      if(!code) 
        fprintf(stderr, PROGRAM_NAME ": compile error\n");
      else
        ana_eval_ex(vm, code, 0); 

      ana_vm_finalize(vm);
    }

    if(opts->debug)
      ana_arena_stats(stderr, state.arena);
    
    ana_arena_free(state.arena);
  }

exit:
  if(resolved_path)
    free(resolved_path);

  ana_free_opts(opts);
  return retval;
}

static int dump_ast(const char *src, int argc, char **argv)
{  
  assert(src);
  (void)argv;
  (void)argc;

  fprintf(stdout, "%s\n", __func__);
  return 0;
}

static int dump_opcodes(const char *src, int argc, char **argv)
{
  assert(src);

  (void)argc; (void)argv;

  fprintf(stdout, "%s\n", __func__);
  return 0;
}

static int run_string(const char *src, int argc, char **argv)
{
  assert(src);

  (void)argc; (void)argv;

  fprintf(stderr, "%s\n", __func__);
  return 0;
}

static int run_command(ana_options *opts)
{
  const char *cmd = opts->cmdstr;

  if(opts->ast)
    return dump_ast(cmd, opts->argc, opts->argv);
  else if(opts->opcodes)
    return dump_opcodes(cmd, opts->argc, opts->argv); 
  else
    return run_string(cmd, opts->argc, opts->argv);
}


int main(int argc, char **argv)
{
  ana_options opts = parse_argv(argc, argv);

  if(opts.error) 
    return usage(1);
  else if(opts.interactive)
    return printf("running an eval loop\n");
  else if(opts.command)
    return run_command(&opts);
  else if(opts.version)  
    return printf("version\n");
  else if(opts.help)   
    return usage(0);
  else if(opts.argc == 0)   
    return usage(1);
  else
    return run_file(&opts);

  return 0;
}

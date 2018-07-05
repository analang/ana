#include <ana.h>
#include <errno.h>
#include <assert.h>
#include <unistd.h>
#include <stdio.h>

ana_object *ana__builtin_readline(ana_object *args)
{
   COMO_UNUSED(args);

    char *buffer = malloc(8);
    size_t buffersize = 8;
    size_t i = 0;
    int c;
    ana_object *retval;

    while(read(STDIN_FILENO, &c, 1) != 0) 
    {
      if(i == buffersize) {
        buffer = realloc(buffer, buffersize * 2);
        buffersize = buffersize * 2;
      }

      if(c == '\n')
        break; 

      buffer[i++] = c;
    }

    if(errno == EINTR) {
  #if 0   
      fprintf(stdout, "read was interuptted by EINTR\n");
      errno = 0;
      goto tryagain;
  #endif
    }

    buffer[i] = '\0';

    retval =  ana_stringfromstring(buffer);

    free(buffer);

    return retval;
}

ana_object *ana__builtin_print(ana_object *args)
{
  ana_size_t i;

  for(i = 0; i < ana_array_size(args); i++)
  {
    ana_object *value = ana_array_get(args, i); 
   
    assert(value->type->obj_print != NULL);

    ana_object_print(value);

    fprintf(stdout, "\n");
  }

  return NULL;
}
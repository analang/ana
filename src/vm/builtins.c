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
    int c = (int)'\0';
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

ana_object *ana__builtin_int(ana_object *args)
{
  if(ana_array_size(args) != 1)
  {
    Ana_SetError("ArgumentError", "int() expects exactly 1 argument");

    return NULL;
  }

  ana_object *val = ana_get_array(args)->items[0];

  if(ana_type_is(val, ana_string_type))
  {
    ana_string *str = ana_get_string(val);

    if(str->len == 1)
    {
      return ana_longfromlong((long)((int)str->value[0]));
    }
    else if(str->len == 0)
    {
      Ana_SetError("ArgumentError", "Invalid argument");
    }
    else
    {
      long lval = 0;
      ana_size_t i;

      for(i = 0; i < str->len; i++)
      {
        if(str->value[i] >= '0' && str->value[i] <= '9')
        {
          lval = lval * 10 + (str->value[i] - '0');
        }
        else
        {
          Ana_SetError("ArgumentError", "Cannot convert string to integer");

          return NULL;
        }
      }

      return ana_longfromlong(lval);
    }
  }
  else if(ana_type_is(val, ana_long_type))
  {
    return val;
  }
  else
  {
    Ana_SetError("ArgumentError", "argument must be a string or an integer");
  }

  return NULL;
} 
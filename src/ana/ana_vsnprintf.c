#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

/*
 * int vsnprintf(char *str, size_t size, const char *format, va_list ap);
 *
 *
 * write at most size bytes (including the terminating null byte ('\0')) to str. 
 *
 * RETURNS the number of characters printed, excluding the null byte
 * If an output error is encountered, a negative value is returned. 
 *
 *
 * If the output was truncated due to this limit (size ) then the return value is the 
 * number of characters (excluding the terminating null byte) which would have 
 * been written to the final string if enough space had been available. Thus, 
 * a return value of size or more means that the output was truncated.
 *
 * 
 * for success:
 * ret should be less than size, because ret will reprsent total characters minus null byte=
 */

static char *build_str(const char *fmt, ...)
{
  int i = 0;
  char *buffer = malloc(10);
  size_t size = 10;
  va_list ap;

  start:
    va_start(ap, fmt);
    int ret = vsnprintf(buffer, size, fmt, ap);
    va_end(ap);

  if(ret < 0) {
    fprintf(stderr, "vsnprintf: error\n");
    return NULL;
  }
  else if((size_t)ret < size)
  {
    goto done;
  }
  if((size_t)ret >= size)
  {
    size_t needed = ((size_t)ret - size) + 1;
    size = size + ((size_t)ret - size ) + 1;
    fprintf(stdout, "build_str: need %zu more bytes\n", needed);
    buffer = realloc(buffer, size);
    goto start;
  }

done:
  printf("size=%zu, ret=%d\n", size, ret);

  return buffer;
}
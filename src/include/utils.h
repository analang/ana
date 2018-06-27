#ifndef COMO_UTILS_H
#define COMO_UTILS_H

#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>
#include <errno.h>

static inline char *ana_strdup(char *src)
{
  if(src == NULL || *src == '\0')
    return src;

  size_t len = strlen(src);
  char *buf;

  buf = malloc(len + 1);

  memcpy(buf, src, len);

  buf[len] = '\0';

  return buf;
}


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

static inline char *ana_build_str(const char *fmt, ...)
{
  //int i = 0;
  char *buffer = malloc(32);
  size_t size = 32;
  va_list ap;

  start:
    va_start(ap, fmt);
    int ret = vsnprintf(buffer, size, fmt, ap);
    va_end(ap);

  if(ret < 0) {
    return NULL;
  }
  else if((size_t)ret < size)
  {
    goto done;
  }
  if((size_t)ret >= size)
  {
   // size_t needed = ((size_t)ret - size) + 1;
    size = size + ((size_t)ret - size ) + 1;
    buffer = realloc(buffer, size);
    goto start;
  }

done:
  return buffer;
}

static inline char *ana_build_str_l(size_t length, char *buffer, const char *fmt, ...)
{
  va_list ap;

  va_start(ap, fmt);
  int ret = vsnprintf(buffer, length, fmt, ap);
  va_end(ap);

  if(ret < 0) 
  {
    return NULL;
  }
  else if((size_t)ret < length)
  {
    goto done;
  }
  if((size_t)ret >= length)
  {
    buffer[length - 1] = '\0';
  }

done:
  return buffer;
}



#define ANA_AUTO_RELEASE(val, block) do { \
  char *value = val; \
  assert(value); \
  block \
  free(value); \
} while(0)

#endif
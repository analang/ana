#include <anaio.h>
#include <stdio.h>
#include <sys/stat.h>
#include <errno.h>

FILE *ana_open_file_for_parsing(const char *filename)
{
  FILE *fp;
  struct stat st;

  /* First check if the file can be opened */
  if((fp = fopen(filename, "r")) == NULL)
  {
    return fp;
  }

  if(stat(filename, &st) == 0 && !S_ISREG(st.st_mode))
  {
    fclose(fp);
    errno = ENOENT;
    return NULL;
  } 

  return fp;
}
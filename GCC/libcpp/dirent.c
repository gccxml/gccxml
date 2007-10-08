#include <dirent.h>
#include <io.h>
typedef struct DIR_s
{
  int first;
  struct dirent dir;
  long handle;
} DIR;

DIR* opendir(const char* dirname)
{
  struct _finddata_t data;
  long handle;
  char buf[1024];
  strcpy(buf, dirname);
  if(buf[strlen(buf)-1] != '\\')
    {
    strcat(buf, "\\");
    }
  strcat(buf, "*.*");
  if((handle = _findfirst(buf, &data)) >= 0)
    {
    DIR* d = (DIR*)malloc(sizeof(DIR));
    d->handle = handle;
    d->first = 1;
    strcpy(d->dir.d_name, data.name);
    return d;
    }
  return 0;
}

struct dirent* readdir(DIR* d)
{
  if(d)
    {
    struct _finddata_t data;
    if(d->first)
      {
      d->first = 0;
      return &d->dir;
      }
    else if(_findnext(d->handle, &data) == 0)
      {
      strcpy(d->dir.d_name, data.name);
      return &d->dir;
      }
    }
  return 0;
}

int closedir(DIR* d)
{
  if(d)
    {
    return _findclose(d->handle);
    }
  return 0;
}

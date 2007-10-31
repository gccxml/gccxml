#ifndef DIRENT_H
#define DIRENT_H

typedef struct DIR_s DIR;

struct dirent
{
  char d_name[256];
};

DIR* opendir(const char*);
struct dirent* readdir(DIR*);
int closedir(DIR*);

/* Hack for libcpp/files.c.  */
#if defined(_MSC_VER)
# include <io.h>
# undef S_ISBLK
# define S_ISBLK(x) 0
# define open _open
#endif

#endif

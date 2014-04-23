#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#define STRLEN 512
#define GAMENAME "mog"

enum filetype { GAMEDATA, USERDATA };

#ifndef _WIN32
int mkdirp(const char *fqfn, mode_t mode);
#endif
FILE *f1open(const char *f, const char *m, const enum filetype t);


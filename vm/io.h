#ifndef IO_H
#define IO_H
#include <string.h>

#include "gdef.h"
#include "memory.h"
#include "names.h"

noreturn sysError(char *, char*);
/* report a nonfatal system error */
noreturn sysWarn(char *, char*);
noreturn imageRead(FILE * fp);

struct __dummy_obj__{
    int di;
    object cl;
    short ds;
};

#endif

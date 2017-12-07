#ifndef G_DEF
#define G_DEF
#include <stdio.h>
#include <stdlib.h>

typedef unsigned char byte;
#define byteToInt(b) (b)
/* this is a bit sloppy - but it works */
//#define longCanBeInt(l) ((l >= -16383) && (l <= 16383))
#define longCanBeInt(l) ((l >= -1073741823) && (l <= 1073741823)) //2^30-1
/* ======== various defines that should work on all systems ==== */
#define streq(a,b) (strcmp(a,b) == 0)
#define true 1
#define false 0
#define nilobj (object) 0
#define ObjectTableMax 6500 
/* define the datatype boolean */
typedef int boolean;
typedef int object;
/*
	ignore means ``i know this function returns something,
	but I really, really do mean to ignore it 
*/
#define ignore (void)
#define noreturn void

struct objectStruct {
    object class;
    short referenceCount;
    int size;// this should be int
    object *memory;
};

/*global variable*/
extern object trueobj, falseobj;
extern object processStack;
extern int linkPointer;
extern struct objectStruct objectTable[];
extern object symbols;
#endif

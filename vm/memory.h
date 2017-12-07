#ifndef MEMORY_H
#define MEMORY_H
#include "gdef.h"
#include "memory.h"
/*
	Little Smalltalk, version 2
	Written by Tim Budd, Oregon State University, July 1987
*/

/*
	The first major decision to be made in the memory manager is what
	an entity of type object really is.  Two obvious choices are a pointer (to 
	the actual object memory) or an index into an object table.  We decided to
	use the latter, although either would work.

	Similarly, one can either define the token object using a typedef,
	or using a define statement.  Either one will work (check this?)

	The memory module itself is defined by over a dozen routines.

	All of these could be defined by procedures, and indeed this was originally
	done.  However, for efficiency reasons, many of these procedures can be
	replaced by macros generating in-line code.  For the latter approach
	to work, the structure of the object table must be known.  For this reason,
	it is given here.  Note, however, that outside of the files memory.c and
	unixio.c (or macio.c on the macintosh) ONLY the macros described in this
	file make use of this structure: therefore modifications or even complete
	replacement is possible as long as the interface remains consistent
*/

#define FREELISTMAX 1000
//#define MemoryBlockSize 50000
//#define mBlockAlloc(s) (object*)malloc(s*sizeof(object))

/*
	The most basic routines to the memory manager are incr and decr,
	which increment and decrement reference counts in objects.  By separating
	decrement from memory freeing, we could replace these as procedure calls
	by using the following macros (thereby saving procedure calls):
*/

//#define incr(x) if ((incrobj=(x))&&!isInteger(incrobj)) objectTable[incrobj>>1].referenceCount++
//#define decr(x) if (((incrobj=(x))&&!isInteger(incrobj))&& (--objectTable[incrobj>>1].referenceCount<=0)) sysDecr(incrobj);

/*
	notice that the argument x is first assigned to a global variable; this is
	in case evaluation of x results in side effects (such as assignment) which
	should not be repeated.
*/

/*
	The next most basic routines in the memory module are those that
	allocate blocks of storage.  There are three routines:
	allocObject(size) - allocate an array of objects
	allocByte(size) - allocate an array of bytes
	allocStr(str) - allocate a string and fill it in

	Again, these may be macros, or they may be actual procedure calls
*/

object allocObject(int);
object allocByte(int);
object allocStr(char*);

/*
	Integer objects are (but need not be) treated specially.

	In this memory manager, negative integers are just left as is, but
	positive integers are changed to x*2+1.  Either a negative or an odd
	number is therefore an integer, while a nonzero even number is an
	object pointer (multiplied by two).  Zero is reserved for the object ``nil''
	Since newInteger does not fill in the class field, it can be given here.
	If it was required to use the class field, it would have to be deferred
	until names.h
*/


#define isInteger(x) ((x) & 0x80000001) 
#define intValue(x) ( x<0 ? x : (x>>1) )

/*
	There are four routines used to access fields within an object.

	Again, some of these could be replaced by macros, for efficiency
	basicAt(x, i) - ith field (start at 1) of object x
	basicAtPut(x, i, v) - put value v in object x
	byteAt(x, i) - ith field (start at 0) of object x
	byteAtPut(x, i, v) - put value v in object x
*/

//#define basicAt(x,i) (sysMemPtr(x)[i-1])
//#define byteAt(x, i) ((int) ((bytePtr(x)[i-1])))
//#define simpleAtPut(x,i,y) (sysMemPtr(x)[i-1] = y)
//#define basicAtPut(x,i,y) incr(simpleAtPut(x, i, y))
//#define fieldAtPut(x,i,y) f_i=i; decr(basicAt(x,f_i)); basicAtPut(x,f_i,y)

/*
	Finally, a few routines (or macros) are used to access or set
	class fields and size fields of objects
*/

#define classField(x) objectTable[x>>1].class
#define setClass(x,y) incr(classField(x)=y)
#define sizeField(x) objectTable[x>>1].size
#define sysMemPtr(x) objectTable[x>>1].memory
#define memoryPtr(x) (isInteger(x)?(object *) 0:sysMemPtr(x))
#define bytePtr(x) ((byte *) memoryPtr(x))
#define charPtr(x) ((char *) memoryPtr(x))


/*
	There is a large amount of differences in the qualities of malloc
	procedures in the Unix world.  Some perform very badly when asked
	to allocate thousands of very small memory blocks, while others
	take this without any difficulty.  The routine mBlockAlloc is used
	to allocate a small bit of memory; the version given below
	allocates a large block and then chops it up as needed; if desired,
	for versions of malloc that can handle small blocks with ease
	this can be replaced using the following macro: 

#define mBlockAlloc(size) (object *) calloc((unsigned) size, sizeof(object))

	This can, and should, be replaced by a better memory management
	algorithm.
*/
void setFreeLists();
void sysDecr(object );
noreturn initMemoryManager();
object *mBlockAlloc(int );
void incr(object );
void decr(object );
object basicAt(object , int );
void simpleAtPut(object , int , object );
void basicAtPut(object , int , object );
void fieldAtPut(object , int ,object );
int byteAt(object , int );
void byteAtPut(object , int ,object );
void visit(register object );
int objectCount();
#endif

/*
Little Smalltalk, version 2
Written by Tim Budd, Oregon State University, July 1987

Improved incorporating suggestions by 
Steve Crawley, Cambridge University, October 1987
Steven Pemberton, CWI, Amsterdam, Oct 1987

memory management module

This is a rather simple, straightforward, reference counting scheme.
There are no provisions for detecting cycles, nor any attempt made
at compaction.  Free lists of various sizes are maintained.
At present only objects up to 255 bytes can be allocated, 
which mostly only limits the size of method (in text) you can create.

reference counts are not stored as part of an object image, but
are instead recreated when the object is read back in.
This is accomplished using a mark-sweep algorithm, similar
to those used in garbage collection.

*/
#include "memory.h"

/*
in theory the objectTable should only be accessible to the memory
manager.  Indeed, given the right macro definitions, this can be
made so.  Never the less, for efficiency sake some of the macros
can also be defined to access the object table directly

Some systems (e.g. the Macintosh) have static limits (e.g. 32K)
which prevent the object table from being declared.
In this case the object table must first be allocated via
calloc during the initialization of the memory manager.
*/



/*
The following variables are strictly local to the memory
manager module

FREELISTMAX defines the maximum size of any object.
*/

static object objectFreeList[FREELISTMAX];	/* free list of objects */

/* the current memory block being hacked up */
static object memoryBlock[MemoryBlockSize* sizeof(object)];	/* malloc'ed chunck of memory */
static int currentMemoryPosition;	/* last used position in above */


/* initialize the memory management module */
noreturn initMemoryManager()
{
	int i;

	/* set all the free list pointers to zero */
	for (i = 0; i < FREELISTMAX; i++)
		objectFreeList[i] = nilobj;

	/* set all the reference counts to zero */
	for (i = 0; i < ObjectTableMax; i++) {
		objectTable[i].referenceCount = 0;
		objectTable[i].size = 0;
	}

	/* make up the initial free lists */
	setFreeLists();
	
	/* force an allocation on first object assignment */
	currentMemoryPosition = 0;

	/* object at location 0 is the nil object, so give it nonzero ref */
	objectTable[0].referenceCount = 1;
	objectTable[0].size = 0;
}

/* setFreeLists - initialise the free lists */
void setFreeLists()
{
	int i, size;
	register int z;
	register struct objectStruct *p;

	objectFreeList[0] = nilobj;

	for (z = ObjectTableMax - 1; z > 0; z--) {
		if (objectTable[z].referenceCount == 0) {
			/* Unreferenced, so do a sort of sysDecr: */
			p = &objectTable[z];
			size = p->size;
			if (size < 0)
				size = ((-size) + 3) / 4;
			p->class = objectFreeList[size];
			objectFreeList[size] = z;
			for (i = size; i > 0;)
				p->memory[--i] = nilobj;
		}
	}
}

/*
mBlockAlloc - rip out a block (array) of object of the given size from
the current malloc block 
*/

object *mBlockAlloc(int memorySize)
{
	object *objptr;

	if (currentMemoryPosition + memorySize >= MemoryBlockSize) {

		/* we toss away space here.  Space-Frugal users may want to
		fix this by making a new object of size
		MemoryBlockSize - currentMemoryPositon - 1
		and putting it on the free list, but I think
		the savings is potentially small */
		sysError("out of memory", "malloc failed");
	}
	objptr = (object *) & memoryBlock[currentMemoryPosition];
	currentMemoryPosition += memorySize;
	return (objptr);
}

/* allocate a new memory object */
object allocObject(int memorySize)
{
	int i;
	register int position;
	boolean done;

	if (memorySize >= FREELISTMAX) {
		fprintf(stderr, "size %d\n", memorySize);
		sysError("allocation bigger than permitted", "allocObject");
	}

	/* first try the free lists, this is fastest */
	if ((position = objectFreeList[memorySize]) != 0) {
		objectFreeList[memorySize] = objectTable[position].class;
	}

	/* if not there, next try making a size zero object and
	making it bigger */
	else if ((position = objectFreeList[0]) != 0) {
		objectFreeList[0] = objectTable[position].class;
		objectTable[position].size = memorySize;
		objectTable[position].memory = mBlockAlloc(memorySize);
	}

	else {			/* not found, must work a bit harder */
		done = false;

		/* first try making a bigger object smaller */
		for (i = memorySize + 1; i < FREELISTMAX; i++)
		if ((position = objectFreeList[i]) != 0) {
			objectFreeList[i] = objectTable[position].class;
			/* just trim it a bit */
			objectTable[position].size = memorySize;
			done = true;
			break;
		}

		/* next try making a smaller object bigger */
		if (!done)
			for (i = 1; i < memorySize; i++)
		if ((position = objectFreeList[i]) != 0) {
			objectFreeList[i] = objectTable[position].class;
			objectTable[position].size = memorySize;
			objectTable[position].memory = mBlockAlloc(memorySize);
			done = true;
			break;
		}

		/* if we STILL don't have it then there is nothing */
		/* more we can do */
		if (!done)
			sysError("out of objects", "alloc");
	}

	/* set class and type */
	objectTable[position].referenceCount = 0;
	objectTable[position].class = nilobj;
	objectTable[position].size = memorySize;
	return (position << 1);
}

object allocByte(int size)
{
	object newObj;

	newObj = allocObject((size + 3) / 4);
	/* negative size fields indicate bit objects */
	sizeField(newObj) = -size;
	return newObj;
}

object allocStr(char* str)
{
	register object newSym;

	newSym = allocByte(1 + strlen(str));
	ignore strcpy(charPtr(newSym), str);
	return (newSym);
}

void incr(object z)
{
	if (z && !isInteger(z)) {
		objectTable[z >> 1].referenceCount++;
	}
}

void decr(object z)
{
	if (z && !isInteger(z)) {
		if (--objectTable[z >> 1].referenceCount <= 0) {
			sysDecr(z);
		}
	}
}


/* do the real work in the decr procedure */
void sysDecr(object z)
{
	register struct objectStruct *p;
	register int i;
	int size;

	p = &objectTable[z >> 1];
	if (p->referenceCount < 0) {
		fprintf(stderr, "object %d\n", z);
		sysError("negative reference count", "");
	}
	decr(p->class);
	size = p->size;
	if (size < 0)
		size = ((-size) + 3) / 4;
	p->class = objectFreeList[size];
	objectFreeList[size] = z >> 1;
	if (size > 0) {
		if (p->size > 0)
			for (i = size; i;)
				decr(p->memory[--i]);
		for (i = size; i > 0;)
			p->memory[--i] = nilobj;
	}
	p->size = size;
}


object basicAt(object z, int i)
{
	if (isInteger(z))
		sysError("attempt to index", "into integer");
	else if ((i <= 0) || (i > sizeField(z))) {
		ignore fprintf(stderr, "index %d size %d\n", i,
		(int) sizeField(z));
		sysError("index out of range", "in basicAt");
	} else
		return (sysMemPtr(z)[i - 1]);
	return (0);
}

void simpleAtPut(object z, int i, object v)
{
	if (isInteger(z))
		sysError("assigning index to", "integer value");
	else if ((i <= 0) || (i > sizeField(z))) {
		ignore fprintf(stderr, "index %d size %d\n", i,
		(int) sizeField(z));
		sysError("index out of range", "in basicAtPut");
	} else {
		sysMemPtr(z)[i - 1] = v;
	}
}

void basicAtPut(object z, int i, object v)
{
	simpleAtPut(z, i, v);
	incr(v);
}

void fieldAtPut(object z, int i,object v)
{
	decr(basicAt(z, i));
	basicAtPut(z, i, v);
}

int byteAt(object z, int i)
{
	byte *bp;
	unsigned char t;

	if (isInteger(z))
		sysError("indexing integer", "byteAt");
	else if ((i <= 0) || (i > -sizeField(z))) {
		fprintf(stderr, "index %d size %d\n", i, sizeField(z));
		sysError("index out of range", "byteAt");
	} else {
		bp = bytePtr(z);
		t = bp[i - 1];
		i = (int) t;
	}
	return (i);
}

void byteAtPut(object z, int i,object x)

{
	byte *bp;
	if (isInteger(z))
		sysError("indexing integer", "byteAtPut");
	else if ((i <= 0) || (i > -sizeField(z))) {
		fprintf(stderr, "index %d size %d\n", i, sizeField(z));
		sysError("index out of range", "byteAtPut");
	} else {
		bp = bytePtr(z);
		bp[i - 1] = x;
	}
}

/*
Written by Steven Pemberton:
The following routine assures that objects read in are really referenced,
eliminating junk that may be in the object file but not referenced.
It is essentially a marking garbage collector algorithm using the 
reference counts as the mark
*/

void visit(register object x)
{
	int i, s;
	object *p;

	if (x && !isInteger(x)) {
		if (++(objectTable[x >> 1].referenceCount) == 1) {
			/* then it's the first time we've visited it, so: */
			visit(objectTable[x >> 1].class);
			s = sizeField(x);
			if (s > 0) {
				p = objectTable[x >> 1].memory;
				for (i = s; i; --i)
					visit(*p++);
			}
		}
	}
}
int objectCount()
{
	register int i, j;
	j = 0;
	for (i = 0; i < ObjectTableMax; i++)
		if (objectTable[i].referenceCount > 0)
			j++;
	return j;
}

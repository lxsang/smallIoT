#ifndef NAMES_H
#define NAMES_H
#include <string.h>
#include "gdef.h"
#include "memory.h"
/*
	Little Smalltalk, version 2
	Written by Tim Budd, Oregon State University, July 1987
*/
/*
	names and sizes of internally object used internally in the system
*/

#define classSize 5
#define nameInClass 1
#define sizeInClass 2
#define methodsInClass 3
#define superClassInClass 4
#define variablesInClass 5

#define methodSize 7
#define textInMethod 1
#define messageInMethod 2
#define bytecodesInMethod 3
#define literalsInMethod 4
#define stackSizeInMethod 5
#define temporarySizeInMethod 6
#define methodClassInMethod 7
//#define watchInMethod 8
#define methodStackSize(x) intValue(basicAt(x, stackSizeInMethod))
#define methodTempSize(x) intValue(basicAt(x, temporarySizeInMethod))

#define contextSize 6
#define linkPtrInContext 1
#define methodInContext 2
#define argumentsInContext 3
#define temporariesInContext 4

#define blockSize 6
#define contextInBlock 1
#define argumentCountInBlock 2
#define argumentLocationInBlock 3
#define bytecountPositionInBlock 4

#define processSize 3
#define stackInProcess 1
#define stackTopInProcess 2
#define linkPtrInProcess 3


noreturn initCommonSymbols();	/* common symbols */
object unSyms[], binSyms[];

noreturn nameTableInsert(object,int,object,object);
int strHash(char*);
object globalKey(char*);
object nameTableLookup(object,char*);

#define globalSymbol(s) nameTableLookup(symbols, s)
#endif

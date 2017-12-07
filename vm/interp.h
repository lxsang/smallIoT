#ifndef INTERP_H
#define INTERP_H
/*
	symbolic definitions for the bytecodes
*/
#include "gdef.h"
#include "memory.h"
#include "names.h"
#include "interp.h"
#define cacheSize 211

#define Extended 0
#define PushInstance 1
#define PushArgument 2
#define PushTemporary 3
#define PushLiteral 4
#define PushConstant 5
#define AssignInstance 6
#define AssignTemporary 7
#define MarkArguments 8
#define SendMessage 9
#define SendUnary 10
#define SendBinary 11
#define pushBlock 	12
#define DoPrimitive 13
#define DoSpecial 15

/* a few constants that can be pushed by PushConstant 
/* constants 0 to 9 are the integers 0 to 9 */
#define nilConst   10
#define trueConst  11
#define falseConst 12
#define minusOne 13		/* the value -1 */

/* types of special instructions (opcode 15) */

#define SelfReturn 1
#define StackReturn 2
#define BlokReturn 3
#define Duplicate 4
#define PopTop 5
#define Branch 6
#define BranchIfTrue 7
#define BranchIfFalse 8
#define AndBranch 9
#define OrBranch 10
#define SendToSuper 11

#define BRANCH_LOC (nextByte() | (nextByte() << 8))
#define BRANCH_SIZE 2

#define nextByte() *(bp + byteOffset++)
#define ipush(x) incr(*++pst=(x))
#define stackTop() *pst
#define stackTopPut(x) decr((*pst)); incr((*pst = x))
#define stackTopFree() decr((*pst)); *pst-- = nilobj
/* note that ipop leaves x with excess reference count */
#define ipop(x) x = stackTop(); *pst-- = nilobj
#define processStackTop() ((pst-psb)+1)
#define receiverAt(n) *(rcv+n)
#define receiverAtPut(n,x) decr(receiverAt(n)); incr(receiverAt(n)=(x))
#define argumentsAt(n) *(arg+n)
#define temporaryAt(n) *(temps+n)
#define temporaryAtPut(n,x) decr(temporaryAt(n)); incr(temporaryAt(n)=(x))
#define literalsAt(n) *(lits+n)
#define contextAt(n) *(cntx+n)
#define contextAtPut(n,x) incr(contextAt(n-1)=(x))
#define processStackAt(n) *(psb+(n-1))


void flushCache(object, object);
boolean vm_execute(object aProcess, int maxsteps);
#endif

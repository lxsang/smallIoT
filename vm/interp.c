
#include "interp.h"

/*
the following variables are local to this module
*/
static object method, messageToSend;

static int messTest(object obj)
{
	return obj == messageToSend;
}

/* a cache of recently executed methods is used for fast lookup */

static struct {
	object cacheMessage;	/* the message being requested */
	object lookupClass;		/* the class of the receiver */
	object cacheClass;		/* the class of the method */
	object cacheMethod;		/* the method itself */
} methodCache[cacheSize];

/* flush an entry from the cache (usually when its been recompiled) */
void flushCache(object messageToSend, object class)
{
	int hash;

	hash = (((int) messageToSend) + ((int) class)) / cacheSize;
	methodCache[hash].cacheMessage = nilobj;
}

/*
findMethod
given a message and a class to start looking in,
find the method associated with the message
*/
static boolean findMethod(object* methodClassLocation)
{
	object methodTable, methodClass;

	method = nilobj;
	methodClass = *methodClassLocation;

	for (; methodClass != nilobj; methodClass =
	basicAt(methodClass, superClassInClass)) {
		methodTable = basicAt(methodClass, methodsInClass);
		method = hashEachElement(methodTable, messageToSend, messTest);
		if (method != nilobj)
			break;
	}

	if (method == nilobj) {	/* it wasn't found */
		methodClass = *methodClassLocation;
		return false;
	}

	*methodClassLocation = methodClass;
	return true;
}


/* the following are manipulated by primitives */

static object growProcessStack(int top, int toadd)
{
	int size, i;
	object newStack;

	if (toadd < 100)
		toadd = 100;
	size = sizeField(processStack) + toadd;
	newStack = newArray(size);
	for (i = 1; i <= top; i++) {
		basicAtPut(newStack, i, basicAt(processStack, i));
	}
	return newStack;
}

boolean vm_execute(object aProcess, int maxsteps)
{
	object returnedObject;
	int returnPoint, timeSliceCounter;
	object *pst, *psb, *rcv, *arg, *temps, *lits, *cntx;
	object contextObject, *primargs;
	int byteOffset;
	object methodClass, argarray;
	int i, j;
	register int low;
	int high;
	register object incrobj;	/* speed up increments and decrements */
	byte *bp;
	int jmp_need = 0;
	/* unpack the instance variables from the process */
	processStack = basicAt(aProcess, stackInProcess);
	psb = sysMemPtr(processStack);
	j = intValue(basicAt(aProcess, stackTopInProcess));
	pst = psb + (j - 1);
	linkPointer = intValue(basicAt(aProcess, linkPtrInProcess));

	/* set the process time-slice counter before entering loop */
	timeSliceCounter = maxsteps;

	/* retrieve current values from the linkage area */
	readLinkageBlock:
	contextObject = processStackAt(linkPointer + 1);
	returnPoint = intValue(processStackAt(linkPointer + 2));
	byteOffset = intValue(processStackAt(linkPointer + 4));
	if (contextObject == nilobj) {
		contextObject = processStack;
		cntx = psb;
		arg = cntx + (returnPoint - 1);
		method = processStackAt(linkPointer + 3);
		temps = cntx + linkPointer + 4;
	} else {			/* read from context object */
		cntx = sysMemPtr(contextObject);
		method = basicAt(contextObject, methodInContext);
		arg = sysMemPtr(basicAt(contextObject, argumentsInContext));
		temps = sysMemPtr(basicAt(contextObject, temporariesInContext));
	}

	if (!isInteger(argumentsAt(0)))
		rcv = sysMemPtr(argumentsAt(0));

	readMethodInfo:
	lits = sysMemPtr(basicAt(method, literalsInMethod));
	bp = bytePtr(basicAt(method, bytecodesInMethod)) - 1;

	while (--timeSliceCounter > 0) {
		low = (high = nextByte()) & 0x0F;
		high >>= 4;
		if (high == 0) {
			high = low;
			low = nextByte();
		}
		switch (high) {

			case PushInstance:
			ipush(receiverAt(low));
			break;

			case PushArgument:
			ipush(argumentsAt(low));
			break;

			case PushTemporary:
			ipush(temporaryAt(low));
			break;

			case PushLiteral:
			ipush(literalsAt(low));
			break;

			case PushConstant:
			switch (low) {
				case 0:
				case 1:
				case 2:
				case 3:
				case 4:
				case 5:
				case 6:
				case 7:
				case 8:
				case 9:
				ipush(newInteger(low));
				break;

				case minusOne:
				ipush(newInteger(-1));
				break;
		
				case nilConst:
				ipush(nilobj);
				break;

				case trueConst:
				ipush(trueobj);
				break;

				case falseConst:
				ipush(falseobj);
				break;

				default:
				sysError("unimplemented constant", "pushConstant");
			}
			break;

			case AssignInstance:
			receiverAtPut(low, stackTop());
			break;

			case AssignTemporary:
			temporaryAtPut(low, stackTop());
			break;

			case MarkArguments:
			returnPoint = (processStackTop() - low) + 1;
			timeSliceCounter++;	/* make sure we do send */
			break;

			case SendMessage:
			messageToSend = literalsAt(low);
			doSendMessage:
			arg = psb + (returnPoint - 1);
			if (isInteger(argumentsAt(0)))
				/* should fix this later */
				methodClass = getClass(argumentsAt(0));
			else {
				rcv = sysMemPtr(argumentsAt(0));
				methodClass = classField(argumentsAt(0));
			}

			doFindMessage:
			/* look up method in cache */
			i = (((int) messageToSend) + ((int) methodClass)) % cacheSize;
			if ((methodCache[i].cacheMessage == messageToSend) &&
			(methodCache[i].lookupClass == methodClass)) {
				method = methodCache[i].cacheMethod;
				methodClass = methodCache[i].cacheClass;
			} else {
				methodCache[i].lookupClass = methodClass;
				if (!findMethod(&methodClass)) {
					/* not found, we invoke a smalltalk method */
					/* to recover */
					j = processStackTop() - returnPoint;
					argarray = newArray(j + 1);
					for (; j >= 0; j--) {
						ipop(returnedObject);
						basicAtPut(argarray, j + 1, returnedObject);
						decr(returnedObject);
					}
					ipush(basicAt(argarray, 1));	/* push receiver back */
					ipush(messageToSend);
					messageToSend =
						newSymbol("message:notRecognizedWithArguments:");
					ipush(argarray);
					/* try again - if fail really give up */
					if (!findMethod(&methodClass)) {
						sysWarn("can't find", "error recovery method");
						/* just quit */
						return false;
					}
				}
				methodCache[i].cacheMessage = messageToSend;
				methodCache[i].cacheMethod = method;
				methodCache[i].cacheClass = methodClass;
			}

			/* save the current byte pointer */
			fieldAtPut(processStack, linkPointer + 4,
			newInteger(byteOffset));

			/* make sure we have enough room in current process */
			/* stack, if not make stack larger */
			i = 6 + methodTempSize(method) + methodStackSize(method);
			j = processStackTop();
			if ((j + i) > sizeField(processStack)) {
				processStack = growProcessStack(j, i);
				psb = sysMemPtr(processStack);
				pst = (psb + j);
				fieldAtPut(aProcess, stackInProcess, processStack);
			}

			byteOffset = 1;
			/* now make linkage area */
			/* position 0 : old linkage pointer */
			ipush(newInteger(linkPointer));
			linkPointer = processStackTop();
			/* position 1 : context object (nil means stack) */
			ipush(nilobj);
			contextObject = processStack;
			cntx = psb;
			/* position 2 : return point */
			ipush(newInteger(returnPoint));
			arg = cntx + (returnPoint - 1);
			/* position 3 : method */
			ipush(method);
			/* position 4 : bytecode counter */
			ipush(newInteger(byteOffset));
			/* then make space for temporaries */
			temps = pst + 1;
			pst += methodTempSize(method);
			/* break if we are too big and probably looping */
			if (sizeField(processStack) > 1800)
				timeSliceCounter = 0;
			goto readMethodInfo;

			case SendUnary:
			/* do isNil and notNil as special cases, since */
			/* they are so common */
			if (low <= 1) {
				if (stackTop() == nilobj) {
					stackTopPut((low ? falseobj : trueobj));
					break;
				}
			}
			returnPoint = processStackTop();
			messageToSend = unSyms[low];
			goto doSendMessage;
			break;

			case SendBinary:
			/* optimized as long as arguments are int */
			/* and conversions are not necessary */
			/* and overflow does not occur */
			if (low <= 12) {
				primargs = pst - 1;
				returnedObject = primitive(low + 60, primargs);
				if (returnedObject != nilobj) {
					/* pop arguments off stack , push on result */
					stackTopFree();
					stackTopPut(returnedObject);
					break;
				}
			}
			/* else we do it the old fashion way */
			returnPoint = processStackTop() - 1;
			messageToSend = binSyms[low];
			goto doSendMessage;
			break;
			case pushBlock:
			/*push block, 
			low is temporary location in the context
			*/
			//int upper, lower;
			//upper = low >> 4;// temporary location
			//lower = low &  0x0F; // temporary count
			// figure contextObject FIXME: redundance code
			//jmp_need= 0;
			if (contextObject == processStack) {
				returnPoint =
					intValue(processStackAt(linkPointer + 2));
				contextObject =
					newContext(linkPointer, method,
				copyFrom(processStack, returnPoint,
				linkPointer - returnPoint),
				copyFrom(processStack, linkPointer + 5,
				methodTempSize(method)));
				basicAtPut(processStack, linkPointer + 1,
				contextObject);
				fieldAtPut(processStack, linkPointer + 4,
				newInteger(byteOffset));
				//jmp_need = 1;
			}
			returnedObject = newBlock();
			basicAtPut(returnedObject, 1, contextObject);
			//printf("%d\n",low &  0x0F );
			//printf("%d\n",(low >> 4)+1);
			basicAtPut(returnedObject, 2, newInteger(low &  0x0F));
			basicAtPut(returnedObject, 3, newInteger((low >> 4)+1));
			basicAtPut(returnedObject, 4, newInteger(byteOffset + BRANCH_SIZE));
			//printf("Byte pointer %d\n",byteOffset + BRANCH_SIZE);
			ipush(returnedObject);
			/*if(jmp_need) {
			//printf("Goto the zoo\n");
			goto readLinkageBlock;
			}
			else
			*/
			byteOffset = BRANCH_LOC;
			break;
			case DoPrimitive:
			/* low gives number of arguments */
			/* next byte is primitive number */
			primargs = (pst - low) + 1;
			/* next byte gives primitive number */
			i = nextByte();
			/* a few primitives are so common, and so easy, that
			they deserve special treatment */
			switch (i) {

				case 11:		/* class of object */
				returnedObject = getClass(*primargs);
				break;
				case 21:		/* object equality test */
				if (*primargs == *(primargs + 1))
					returnedObject = trueobj;
				else
					returnedObject = falseobj;
				break;
				case 25:		/* basicAt: */
				j = intValue(*(primargs + 1));
				returnedObject = basicAt(*primargs, j);
				break;
				case 31:		/* basicAt:Put: */
				j = intValue(*(primargs + 1));
				fieldAtPut(*primargs, j, *(primargs + 2));
				returnedObject = nilobj;
				break;
				case 53:		/* set time slice */
				timeSliceCounter = intValue(*primargs);
				returnedObject = nilobj;
				break;
				case 58:		/* allocObject */
				j = intValue(*primargs);
				returnedObject = allocObject(j);
				break;
				case 87:		/* value of symbol */
				returnedObject = globalSymbol(charPtr(*primargs));
				break;
				default:
				returnedObject = primitive(i, primargs);
				break;
			}
			/* increment returned object in case pop would destroy it */
			incr(returnedObject);
			/* pop off arguments */
			while (low-- > 0) {
				stackTopFree();
			}
			/* returned object has already been incremented */
			ipush(returnedObject);
			decr(returnedObject);
			break;

			doReturn:
			returnPoint = intValue(basicAt(processStack, linkPointer + 2));
			linkPointer = intValue(basicAt(processStack, linkPointer));
			while (processStackTop() >= returnPoint) {
				stackTopFree();
			}
			/* returned object has already been incremented */
			ipush(returnedObject);
			decr(returnedObject);
			/* now go restart old routine */
			if (linkPointer != nilobj)
				goto readLinkageBlock;
			else
				return false /* all done */ ;

			case DoSpecial:
			switch (low) {
				case SelfReturn:
				incr(returnedObject = argumentsAt(0));
				goto doReturn;

				case StackReturn:
				ipop(returnedObject);
				goto doReturn;
		
				case BlokReturn:
				ipop(returnedObject);
				// then creating context pointer 
				if(contextObject != processStack)
				{
					j = intValue(basicAt(contextObject, 1));
					// first change link pointer to that of creator 
					fieldAtPut(processStack, linkPointer, basicAt(processStack, j));
					// then change return point to that of creator 
					fieldAtPut(processStack, linkPointer + 2, basicAt(processStack, j + 2));
				}
				goto doReturn;

				case Duplicate:
				/* avoid possible subtle bug */
				returnedObject = stackTop();
				ipush(returnedObject);
				break;

				case PopTop:
				ipop(returnedObject);
				decr(returnedObject);
				break;

				case Branch:
				/* avoid a subtle bug here */
				/*next 2 bytes is branch location*/
				i = BRANCH_LOC;
				//printf("GOTO : %d\n",i );
				byteOffset = i;
				break;

				case BranchIfTrue:
				ipop(returnedObject);
				//i = nextByte();
				i = BRANCH_LOC;
				//printf("GOTO IF TRUE: %d\n",i );
				if (returnedObject == trueobj) {
					/* leave nil on stack */
					pst++;
					byteOffset = i;
				}
				decr(returnedObject);
				break;

				case BranchIfFalse:
				ipop(returnedObject);
				//i = nextByte();
				i = BRANCH_LOC;
				//printf("GOTO IF FALSE: %d\n",i );
				if (returnedObject == falseobj) {
					/* leave nil on stack */
					pst++;
					byteOffset = i;
				}
				decr(returnedObject);
				break;

				case AndBranch:
				ipop(returnedObject);
				//i = nextByte();
				i = BRANCH_LOC;
				//printf("GOTO AND: %d\n",i );
				if (returnedObject == falseobj) {
					ipush(returnedObject);
					byteOffset = i;
				}
				decr(returnedObject);
				break;

				case OrBranch:
				ipop(returnedObject);
				//i = nextByte();
				i = BRANCH_LOC;
				//printf("GOTO OR: %d\n",i );
				if (returnedObject == trueobj) {
					ipush(returnedObject);
					byteOffset = i;
				}
				decr(returnedObject);
				break;

				case SendToSuper:
				i = nextByte();
				messageToSend = literalsAt(i);
				rcv = sysMemPtr(argumentsAt(0));
				methodClass = basicAt(method, methodClassInMethod);
				/* if there is a superclass, use it
				otherwise for class Object (the only 
				class that doesn't have a superclass) use
				the class again */
				returnedObject = basicAt(methodClass, superClassInClass);
				if (returnedObject != nilobj)
					methodClass = returnedObject;
				goto doFindMessage;

				default:
				sysError("invalid doSpecial", "");
				break;
			}
			break;

			default:
			sysError("invalid bytecode", "");
			break;
		}
	}

	/* before returning we put back the values in the current process */
	/* object */

	fieldAtPut(processStack, linkPointer + 4, newInteger(byteOffset));
	fieldAtPut(aProcess, stackTopInProcess, newInteger(processStackTop()));
	fieldAtPut(aProcess, linkPtrInProcess, newInteger(linkPointer));

	return true;
}

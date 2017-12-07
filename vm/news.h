#ifndef NEW_H
#define NEW_H
#include "gdef.h"
#include "memory.h"
#include "names.h"

object getClass(object);
object copyFrom(object, int, int);
object newArray(int);
object newBlock();
object newByteArray(int);
object newClass(char*);
object newChar(int);
object newContext(int,object,object, object);
object newDictionary(int);
object newFloat(double);
object newMethod();
object newLink(object,object);
object newStString(char*);
object newSymbol(char*);
//double floatValue(object);
object newInteger(int);
#endif
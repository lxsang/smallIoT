#include "gdef.h"

//boolean watching = 0;
object processStack;
int linkPointer;
object trueobj, falseobj;
object symbols;			/* table of all symbols created */
struct objectStruct objectTable[ObjectTableMax];
/*
The MIT License (MIT)

Copyright (c) 2015 LE Xuan Sang xsang.le@gmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/
#include "free_list.h"

oslot_t objectFreeList[MAX_FLIST] = {NULL};

oslot_t fl_lookup(int key)
{
	oslot_t np;
    for (np = objectFreeList[key%MAX_FLIST]; np != NULL; np = np->next)
    {
        if (key == np->key)
          return np; /* found */
    }
    return NULL; /* not found */
}
oslot_t __fl_put_el_with_key(int key)
{
	oslot_t np;
	int hashval = key%MAX_FLIST;
    if ((np = fl_lookup(key)) == NULL) { /* not found */
        np = (oslot_t) malloc(sizeof(*np));
        if (np == NULL || (np->key = key) < 0)
          return NULL;
        np->next = objectFreeList[hashval];
        objectFreeList[hashval] = np;
    }
    return np;
}
oslot_t fl_put(int key, int value)
{
	oslot_t np = __fl_put_el_with_key(key);
	if(np == NULL) return NULL;
	np->value = value;
    return np;
}
int fl_remove(int key)
{
	int hashval = key%MAX_FLIST;
	oslot_t np = objectFreeList[hashval];
	if(np!=NULL && key == np->key)
	{
		objectFreeList[hashval] = np->next;
		return 1;
	}
    for (np= objectFreeList[hashval]; np != NULL; np = np->next)
        if (np->next!=NULL&& key == np->next->key)
        {
         	np->next = np->next->next; /* found */
         	return 1;
        }
    return 0; /* not found */

}
int fl_value(int key)
{
	oslot_t as = fl_lookup(key);
	if(as == NULL) return 0;
	return as->value;
}
void fl_clear()
{
	for(size_t i = 0; i < MAX_FLIST; ++i)
	{
		objectFreeList[i] = NULL;
	}
}
#ifdef DEBUG
void  dump_fl()
{
	oslot_t np;
	for_each_slot(np)
	{
		if(np->value)
		{
			printf("Found free object at :%d\n", np->value);
		}
	}
}
#endif

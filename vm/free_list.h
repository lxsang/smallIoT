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
#ifndef FREE_LIST_H
#define FREE_LIST_H

#include <stdio.h>
//#include <stdlib.h>

#define MAX_FLIST 1024

#define for_each_slot(slot) \
    for(int i = 0; i < MAX_FLIST; i++) \
    	for(slot = objectFreeList[i];slot!= NULL; slot = slot->next)
#define FL_SET(i,v) (fl_put(i,v))
#define FL_GET(s) (fl_value(s))
/**
 * Dictionary for header
 */
typedef struct  __slot{ 
    struct __slot *next; 
    int key; 
    int value;
} * oslot_t;

extern oslot_t objectFreeList[MAX_FLIST];

oslot_t fl_lookup(int);
int fl_value(int);
oslot_t fl_put(int, int);
int fl_remove(int);
void fl_clear();
void dump_fl();
#endif
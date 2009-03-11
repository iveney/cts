#include "fast_path.h"

#ifndef   _HEAP_     
#define   _HEAP_ 


void shiftdown(heap *heapdata,int size,int position);
void shiftup(heap *heapdata,int size,int position);
void makeheap(heap *heapdata,int size);
void insert(heap *heapdata,int *size,heap data);
void getsmall(heap *heapdata,int *size,heap *data);

/*
void addelement(heap *heapdata,int &size,int x,int y,int value);
void inielement(heap &data,int x,int y,int value);


void extract_min(heap *value_heap,int &value_length,heap &data,heap *key_heap);
void shiftdown(heap *heapdata,int size,int position,heap *key_heap);
void decrease(heap *value_heap,heap *key_heap,int position,int length);
void shiftup(heap *heapdata,int position,heap *key_heap);
*/
#endif


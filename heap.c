#include <stdio.h>
#include <stdlib.h>
#include "heap.h"
void shiftdown(heap *heapdata,int size,int position){
	int flag;
	heap temp;
	if((position<<1)>size){
		// just in order to look nice
	}
	else{
		if(((position<<1)+1)<=size){
			if(heapdata[position<<1].delay<heapdata[(position<<1)+1].delay)
				flag=0;
			else flag=1;

			if(heapdata[position].delay<=heapdata[(position<<1)+flag].delay){
				//just in order to look nice
			}
			else{
				temp=heapdata[position];
				heapdata[position]=heapdata[(position<<1)+flag];
				heapdata[(position<<1)+flag]=temp;
				shiftdown(heapdata,size,(position<<1)+flag);
			}
		}
		else{
			if(heapdata[position].delay<=heapdata[(position<<1)].delay){
				//just in order to look nice
			}
			else{
				temp=heapdata[position];
				heapdata[position]=heapdata[(position<<1)];
				heapdata[(position<<1)]=temp;
			}
		}
	}
}


void shiftup(heap *heapdata,int size,int position){
	heap temp;

//	if((position!=1)&&(heapdata[position].value<heapdata[position/2].value)){
    if((position!=1)&&(heapdata[position].delay<heapdata[position>>1].delay)){
		temp=heapdata[position];
		heapdata[position]=heapdata[position>>1];
		heapdata[position>>1]=temp;
		shiftup(heapdata,size,position>>1);
	}
}
void makeheap(heap *heapdata,int size){
	int i;
	if(size<=1){
	    //in order to look nice
	}
	else{
		for(i=size>>1;i>=1;i--)
			shiftdown(heapdata,size,i);
	}
}
void insert(heap *heapdata,int *size,heap data){
	(*size) = (*size) + 1;
	heapdata[(*size)] = data;
	shiftup(heapdata,*size,*size);
}
void getsmall(heap *heapdata,int *size,heap *data){
	(*data)=heapdata[1];
	heapdata[1]=heapdata[(*size)];
	(*size)=(*size)-1;
	
	shiftdown(heapdata,*size,1);

}

/*
void addelement(heap *heapdata,int &size,int x,int y,int value){
	size=size+1;
	heapdata[size].p.x=x;
	heapdata[size].p.y=y;
	heapdata[size].value=value;
}

void inielement(heap &data,int x,int y,int value){
	data.p.x=x;
	data.p.y=y;
	data.value=value;
}

void extract_min(heap *value_heap,int &value_length,heap &data,heap *key_heap){
	data=value_heap[1];
	value_heap[1]=value_heap[value_length];
	value_length=value_length-1;

	key_heap[value_heap[1].p.x].value=1;
	shiftdown(value_heap,value_length,1,key_heap);

}

void shiftdown(heap *heapdata,int size,int position,heap *key_heap){
	int flag;
	heap temp;
	if((position<<1)>size){
		// just in order to look nice
	}
	else{
		if(((position<<1)+1)<=size){
			if(heapdata[position<<1].value<heapdata[(position<<1)+1].value)
				flag=0;
			else flag=1;

			if(heapdata[position].value<=heapdata[(position<<1)+flag].value){
				//just in order to look nice
			}
			else{
				temp=heapdata[position];
				heapdata[position]=heapdata[(position<<1)+flag];
				heapdata[(position<<1)+flag]=temp;

				key_heap[heapdata[position].p.x].value=position;
				key_heap[heapdata[(position<<1)+flag].p.x].value=(position<<1)+flag;
				shiftdown(heapdata,size,(position<<1)+flag,key_heap);
			}
		}
		else{
			if(heapdata[position].value<=heapdata[(position<<1)].value){
				//just in order to look nice
			}
			else{
				temp=heapdata[position];
				heapdata[position]=heapdata[(position<<1)];
				heapdata[(position<<1)]=temp;

				key_heap[heapdata[position].p.x].value=position;
				key_heap[heapdata[(position<<1)].p.x].value=(position<<1);
			}
		}
	}

}
void decrease(heap *value_heap,heap *key_heap,int position,int length){
	value_heap[position].value=length;
	shiftup(value_heap,position,key_heap);

}

void shiftup(heap *heapdata,int position,heap *key_heap){
	heap temp;

//	if((position!=1)&&(heapdata[position].value<heapdata[position/2].value)){
    if((position!=1)&&(heapdata[position].value<heapdata[position>>1].value)){
		temp=heapdata[position];
		heapdata[position]=heapdata[position>>1];
		heapdata[position>>1]=temp;

		key_heap[heapdata[position].p.x].value=position;
		key_heap[heapdata[position>>1].p.x].value=(position>>1);
		shiftup(heapdata,position>>1,key_heap);
	}

}*/

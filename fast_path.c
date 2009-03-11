#include <stdio.h>
#include <stdlib.h>
#include <math.h>
//#include "ds.h"
#include "fast_path.h"
#include "heap.h"

#define  maximal_heap_length 100000
#define buffer_type_num 7
#define wire_type_num 2

/*return value denotes success or false
 * input:
 * head_buffer : 0 denotes no buffer for head, 1 denotes buffer
 * tail_buffer : 0 denotes no buffer for tail, 1 denotes buffer
 * segment_num : |__|__|__|, the segment_num should be 4
 *
 * output:
 * x_path_info buffer and wire info for every segment
 * return value
 * 1 denotes success to create the critical path
 * 0 denotes fail to create the critical path
 * */
int critical_path(int head_buffer, int tail_buffer, int segment_num, int **x_path_info, BUF_NODE * store, BUF_NODE ** OBUF, DME_TREE_NODE * Rroot, DME_TREE_NODE ** Tmap)
{
	int i;
	int j;
	int flag = 0;
	int flag_free = 0;
	float min_CLR = 1000000.0f;

	int *buffer_inserted_info;
	buffer_inserted_info = (int *) malloc (sizeof(int) * segment_num);
	for(i = 0; i < segment_num; i++)
		buffer_inserted_info[i] = 1;
	buffer_inserted_info[0] = head_buffer;
	buffer_inserted_info[segment_num - 1] = tail_buffer;

	int history_num = segment_num * 2;
	int *path_arrange_info;
	path_arrange_info = (int *) malloc (sizeof(int) * history_num);

	delay_capacity temp;
	delay_capacity temp1;
	temp.history = (int *) malloc (sizeof(int) * history_num);
	temp.current_index = 0;
	temp.delay = 0.0f;//CLR
	temp.buffer_index = -1;
	temp.history[temp.current_index * 2] = -1; //-1 denotes no buffer
	temp.buffer_insert = buffer_inserted_info[temp.current_index];
	temp.isuseful = 1;

	delay_capacity *Q; //10000 is constant
	Q = (delay_capacity *) malloc (sizeof(delay_capacity) * maximal_heap_length);
	int length_of_heap = 0;

	length_of_heap++;
	Q[length_of_heap] = temp;

	while(length_of_heap > 0){
		getsmall(Q, &length_of_heap, &temp);
		printf("li %f\n", temp.delay);
		printf("li_current_index is %d\n",temp.current_index);
		if(temp.delay > min_CLR){
			flag_free = 1;
			break;
		}
		else if((temp.current_index == (segment_num-1)) &&(temp.delay < min_CLR) &&((temp.buffer_insert == 0)||(temp.buffer_index >=0)) &&(temp.isuseful == 1)){
			flag = 1;
			min_CLR = temp.delay;
			for(i = 0; i < history_num; i++)
				path_arrange_info[i] = temp.history[i];

		}
		else if((temp.current_index < (segment_num-1)) && ((temp.buffer_insert == 0) || (temp.buffer_index >=0))){
			for(i = 0; i < wire_type_num; i++){
				printf("li_in the function of insert line\n");
				temp1.history = (int *) malloc (sizeof(int) * history_num);
				for(j = 0; j < history_num; j++)
					temp1.history[j] = temp.history[j];
				temp1.current_index = temp.current_index + 1;
				temp1.buffer_index = -1;
				temp1.history[temp.current_index *2 + 1] = i;
				temp1.history[temp1.current_index * 2] = -1;
				temp1.buffer_insert = buffer_inserted_info[temp1.current_index];







				temp1.isuseful = partial_path_CLR(segment_num,temp1.current_index, temp1.history, store,head_buffer,OBUF,Rroot,Tmap,&temp1.delay );
				printf("li liang's delay is %f\n",temp1.delay);
				if(temp1.isuseful == -1)
					free(temp1.history);
				else{
					length_of_heap++;
					if(length_of_heap >= maximal_heap_length){
						printf("heap space is not enough\n");
						exit(0);
					}
					Q[length_of_heap] = temp1;
				}
			}//wire choose end for
		}//end else

		if((temp.buffer_insert == 1) && (temp.buffer_index == (-1))){
			for(i = 4; i < buffer_type_num; i++){
				printf("li_in the function of insert buffer\n");
				temp1.history = (int *) malloc (sizeof(int) * history_num);
				for(j = 0; j < history_num; j++)
					temp1.history[j] = temp.history[j];
				temp1.current_index = temp.current_index;
				temp1.buffer_index = i;
				temp1.history[temp.current_index * 2] = i;
				temp1.buffer_insert = 1;
			    temp1.isuseful = partial_path_CLR(segment_num,temp1.current_index, temp1.history, store,head_buffer,OBUF,Rroot,Tmap,&temp1.delay);
				printf("li liang's delay is %f\n",temp1.delay);
				if(temp1.isuseful != 1){
					printf("throw");
					free(temp1.history);
					if((temp1.isuseful == -1) &&(i % 2 == 1))
						continue;
				}
				else{
					length_of_heap++;
					if(length_of_heap >= maximal_heap_length){
						printf("heap space is not enough\n");
						exit(0);
					}
					Q[length_of_heap] = temp1;
				}
			}//Buffer type for
		}//Buffer add if

		free(temp.history);
		makeheap(Q, length_of_heap);
	}

	for(i = 1; i <= length_of_heap; i++){
		free(Q[i].history);
	}
	free(Q);

	if((flag == 1) && (flag_free == 1))
		free(temp.history);
	(*x_path_info) = path_arrange_info;

	return flag;// flag EQ 0 false, flag EQ 1 true
}

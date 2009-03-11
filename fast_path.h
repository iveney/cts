#ifndef FAST_PATH_H
#define FAST_PATH_H
#include"ds.h"
typedef struct delay_capacity{
	float capacity;
	double delay;
	int buffer_index;//-1 without buffer
	int buffer_insert; //0 cann't insert
	int wire_index;//0 with big resistance and small capa
	int expand_direction;//1 increase, 2 decrease
	int current_index;//0 and the end
	int *history;//the total_length is path_visi+1
	int isuseful;//-1 no, 1 yes
}delay_capacity;

#define heap delay_capacity


int critical_path(int head_buffer, int tail_buffer, int segment_num, int **x_path_info, BUF_NODE * store, BUF_NODE ** OBUF, DME_TREE_NODE * Rroot, DME_TREE_NODE ** Tmap);
#endif

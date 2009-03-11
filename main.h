#ifndef __MAIN_H__
#define __MAIN_H__

BOX frame	;
SOURCE source  ;
SINK sink	;
WIRELIB wirelib;
BUFLIB	buflib ;
VDDLIB	vddlib ;
int SlewLimit;
double cap_left;
int CapLimit ;
int HIGHWAY;
BLOCKAGE blockage;
DME_NODE * source_node;
double *init_delay_array;
int which_level;
int mid_sink_num;
int LAMBADA3;
int num_node ;
int num_buffer;
int num_wire ;
int num_sinknode ;
int num_total_nodes ;
int highway_extra_buf ;
int free_buf_num;
double level_length[20];
int sink_array_num;
int total_buf_num = 0;
int crt_num_node;
int crt_num_wire;
int crt_total_buf_num;
int new_num_node;
int new_num_wire;
int new_total_buf_num;
int new_num_total_nodes;
DME_TREE_NODE * ZERO;

#endif

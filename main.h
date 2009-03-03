#ifndef __MAIN_H__
#define __MAIN_H__

BOX frame	; 
SOURCE source  ;
SINK sink	; 
WIRELIB wirelib;
BUFLIB	buflib ; 
VDDLIB	vddlib ; 
int SlewLimit; 
int CapLimit ; 
BLOCKAGE blockage;
DME_NODE * source_node;
double *init_delay_array;
int mid_sink_num;
int num_node ; 
int num_buffer; 
int num_wire ; 
int num_sinknode ; 
int num_total_nodes ;
int highway_extra_buf ;
int free_buf_num;
int sink_array_num;


#endif

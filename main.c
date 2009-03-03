#include "ds.h"

#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<string.h>

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
int main(int argc, char **argv){
	FILE *ifp; 
	FILE *ofp;
	FILE *figfp; 
	BUF_NODE ** OBUF;  
	DME_TREE_NODE * OT ;
	DME_TREE_NODE ** OTmap ;
	
	if(argc > 4){
		printf("error: command inputfile\n");
		exit(1);
	}
	ifp = fopen( argv[1], "r") ; 
	ofp = fopen(argv[2], "w") ; 
	
	InputFile(ifp);
	preprocess_block(&blockage);

	init_delay_array = (double *) malloc (sizeof(double)*sink.num);
// 	printf("%d\n",InputFile(ifp));
/*	check_input();
	exit(0);*/
	DME_NODE *L = deferred_merge_embedding();
 	insert_buffer(source_node, &OBUF, &OT, &OTmap) ; 
	
	output_file (ofp ,OBUF, OT, OTmap) ; 
	
	return 1;
}



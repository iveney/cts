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

int num_node ; 
int num_buffer; 
int num_wire ; 
int num_sinknode ; 
int num_total_nodes ; 

int main(int argc, char **argv){
	FILE *ifp; 
	FILE *ofp ; 
	BUF_NODE ** OBUF;  
	DME_TREE_NODE * OT ;
	DME_TREE_NODE ** OTmap ; 



	
	if(argc > 3){
		printf("error: command inputfile\n");
		exit(1);
	}
	ifp = fopen( argv[1], "r") ; 
	ofp = fopen(argv[2], "w") ; 
	printf("%d\n",InputFile(ifp));
	//check_input();
	DME_NODE *L = deferred_merge_embedding();
	insert_buffer(L, &OBUF, &OT, &OTmap) ; 
	output_file (ofp ,OBUF, OT, OTmap) ; 






	return 1;
}



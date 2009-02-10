#include <stdio.h>
#include <stdlib.h>
#include "ds.h"
#include "connect.h"

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

extern UINT * shortest;

int main(int argc, char * argv[]){
	FILE *ifp; 
	FILE *ofp ; 
	BUF_NODE ** OBUF;  
	DME_TREE_NODE * OT ;
	DME_TREE_NODE ** OTmap ; 
	if(argc > 3 || argc < 2){
		printf("error: command inputfile\n");
		exit(1);
	}
	ifp = fopen( argv[1], "r") ; 
	ofp = fopen(argv[2], "w") ; 
	printf("%d\n",InputFile(ifp));

	// start to test
	constructg(&blockage);

	// input two points
	FILE * fp = fopen("twopt","r");
	if( fp == NULL )
		printf("error\n");
	NODE s,t;
	fscanf(fp,"%lu%lu%lu%lu",&s.x,&s.y,&t.x,&t.y);
	//s.x = 0; s.y = 40;
	//t.x = 55;t.y = 45;
	/*
	outputg();
	printf("\n---------------------------------------------------------\n");
	output_dirs();
	add2pt(s,t,&blockage);
	*/

	/*
	printf("\n---------------------------------------------------------\n");
	outputg();
	printf("\n---------------------------------------------------------\n");
	output_dirs();
	*/

	add2pt(s,t,&blockage);
	delpt(g_size-2,&blockage);
	delpt(g_size-1,&blockage);
	add2pt(s,t,&blockage);

	printf("\n---------------------------------------------------------\n");
	outputg();
	printf("\n---------------------------------------------------------\n");
	output_dirs();

	// (blockage.num - 2) denotes the index of first point
	dijkstra(&blockage,g_size-2);
	dijkstra(&blockage,g_size-2);

	int i;
	for(i=0;i<g_size;i++)
		printf("%10d",shortest[i]);
	printf("\n");
	printf("s to t: %lu\n",(unsigned long)shortest[g_size-1]);

	free_all();
	return 0;
}

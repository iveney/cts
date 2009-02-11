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

void init_draw(FILE *pFig){
	fprintf(pFig,"#FIG 3.1\n");
	fprintf(pFig,"Landscape\n");
	fprintf(pFig,"Center\n");
	fprintf(pFig,"Inches\n");
	fprintf(pFig,"1200 2\n");
}

void draw_blockages(FILE * fp){
	int i;
	for(i=0;i<blockage.num;i++)
		draw_block(fp,blockage.pool[i],SOLID,BLUE);
}

void draw_sinks(FILE * fp){
	int i;
	for(i=0;i<sink.num;i++)
		draw_point(fp,(double)sink.pool[i].x,(double)sink.pool[i].y,SOLID,RED);
}

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
		printf("open twopt error\n");
	NODE s,t;
	fscanf(fp,"%lu%lu%lu%lu",&s.x,&s.y,&t.x,&t.y);

	add2pt(s,t,&blockage);
	/*
	printf("\n---------------------------------------------------------\n");
	outputg();
	printf("\n---------------------------------------------------------\n");
	output_dirs();
	*/

	// (blockage.num - 2) denotes the index of first point
	/*
	dijkstra(&blockage,g_size-2);

	int i;
	for(i=0;i<g_size;i++)
		printf("%10d",shortest[i]);
	printf("\n");
	printf("s to t: %lu\n",(unsigned long)shortest[g_size-1]);
	*/

	//////////////////////////////////////////////////////////////////////////
	FILE * pFig = fopen("fig","w");
	init_draw(pFig);
	draw_blockages(pFig);
	draw_sinks(pFig);

	fclose(pFig);
	fclose(fp);
	free_all();
	return 0;
}

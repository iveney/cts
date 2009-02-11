#include <stdio.h>
#include <stdlib.h>
#include "util.h"
#include "ds.h"
#include "connect.h"
#include "bufplace.h"
#include "io.h"

#define test

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

void init_draw(FILE *pFig){
	fprintf(pFig,"#FIG 3.1\n");
	fprintf(pFig,"Landscape\n");
	fprintf(pFig,"Center\n");
	fprintf(pFig,"Inches\n");
	fprintf(pFig,"1200 2\n");
}

int main(int argc, char * argv[]){
	FILE *ifp,*ofp; 
	BUF_NODE ** OBUF;  
	DME_TREE_NODE * OT ;
	DME_TREE_NODE ** OTmap ; 
	if(argc > 3 || argc < 2)
		report_exit("error: command inputfile");
	ifp = fopen(argv[1], "r") ; 
	ofp = fopen(argv[2], "w") ; 
	if( InputFile(ifp) != 1 )
		report_exit("Error reading file");

	// start to test
	constructg(&blockage);
	NODE s,t;
	s.x=sink.pool[0].x; s.y=sink.pool[0].y;
	t.x=sink.pool[1].x; t.y=sink.pool[1].y;
	add2pt(s,t,&blockage);
	dijkstra(&blockage,g_size-2);
	/*
	int i;
	for(i=0;i<g_size;i++) printf("%10d",via[i]);
	for(i=0;i<g_size;i++) printf("%10d",shortest[i]);
	printf("\n");

	printf("\n---------------------------------------------------------\n");
	outputg();
	printf("\n---------------------------------------------------------\n");
	output_dirs();
	*/

	//////////////////////////////////////////////////////////////////////////
	// write results into file
	FILE * pFig = fopen("fig","w");
	init_draw(pFig);
	draw_blockages(pFig);
	draw_sinks(pFig);
	draw_single_source_rectilinear(pFig,g_size-2,s,t);

	fclose(pFig);
	free_all();
	return 0;
}

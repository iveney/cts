#include <stdio.h>
#include <stdlib.h>
#include "util.h"
#include "ds.h"
#include "connect.h"
#include "bufplace.h"
#include "io.h"

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

void draw_case(FILE * pFig){
	init_draw(pFig);
	draw_blockages(pFig);
	draw_sinks(pFig);
}

int main(int argc, char * argv[]){
	FILE *ifp,*ofp; 
	if(argc > 3 || argc < 2)
		report_exit("error: command inputfile");
	ifp = fopen(argv[1], "r") ; 
	ofp = fopen(argv[2], "w") ; 
	if( InputFile(ifp) != 1 )
		report_exit("Error reading file");

	/*
	int **_out[2];
	int ***out=_out;
	int x,y,z;
	int dim1=3,dim2=10,dim3=5;
	for(x=0;x<dim1;x++){
		out[x]=(int**)malloc(sizeof(int**)*dim2);
		for(y=0;y<dim2;y++){
			out[x][y]=(int*)malloc(sizeof(int*)*dim3);
			for(z=0;z<dim3;z++){
				out[x][y][z]=x*dim3*dim2+y*dim3+z;
				printf("%10d",out[x][y][z]);
			}
			printf("\n");
		}
		printf("\n\n\n\n");
	}
	*/

	// start to test
	construct_g_all(&blockage,&sink);
	/*
	 // check construct_g_all output
	printf("\n---------------------------------------------------------\n");
	outputg();
	printf("\n---------------------------------------------------------\n");
	output_dirs();
	*/

	//int src_idx=block_num*4+1;
	//dijkstra(&blockage,src_idx);
	/*
	// check dijkstra's output
	int i;
	for(i=0;i<g_size;i++) printf("%10d",via[i]);
	for(i=0;i<g_size;i++) printf("%10d",shortest[i]);
	printf("\n");
	*/

	//////////////////////////////////////////////////////////////////////////
	// write results into file
	UINT ** p = floyd();
	FILE * pFig = fopen("fig","w");
	FILE * pFig_rect = fopen("fig2","w");
	draw_case(pFig);
	draw_case(pFig_rect);

	//draw_single_source_tree(pFig,src_idx);
	//draw_single_source_rectilinear(pFig_rect,src_idx);

	fclose(pFig);
	fclose(pFig_rect);
	free_all();
	return 0;
}

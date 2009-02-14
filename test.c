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
	fprintf(pFig,"0 32 #dddddd\n");
}

void draw_case(FILE * pFig){
	init_draw(pFig);
	draw_blockages(pFig);
	draw_sinks(pFig);
}

void output_g_dirs(){
	// check construct_g_all output
	printf("\n---------------------------------------------------------\n");
	outputg();
	printf("\n---------------------------------------------------------\n");
	output_dirs();
}

void output_dijkstra(){
	// check dijkstra's output
	int i;
	printf("dijkstra output:\n");
	for(i=0;i<g_size;i++) printf("%10d",shortest[i]);
	printf("\n");
	for(i=0;i<g_size;i++) printf("%10d",via[i]);
	printf("\n");
}

void print_path(int which,int src_idx,int dst_idx){
	int p=dst_idx;
	while( p != src_idx ){
		printf("%5d",p);
		p=backtrack_pair[which][src_idx][p];
	}
	printf("\n");
}

int main(int argc, char * argv[]){
	FILE *ifp,*ofp; 
	if(argc > 3 || argc < 2)
		report_exit("error: command inputfile");
	ifp = fopen(argv[1], "r") ; 
	ofp = fopen(argv[2], "w") ; 
	if( InputFile(ifp) != 1 )
		report_exit("Error reading file");

	// start to test
	construct_g_all(&blockage,&sink);

	int src_idx=block_num*4;
	//output_g_dirs();
	//dijkstra(&blockage,src_idx);
	//output_dijkstra();

	int i;
	all_pair_shortest();

	/*
	printf("floyd\n");
	for(i=0;i<g_size;i++) printf("%10d",shortest_pair[which][src_idx][i]);
	printf("\n");
	for(i=0;i<g_size;i++) printf("%10d",backtrack_pair[which][src_idx][i]);
	printf("\n");
	*/

	// write results into file
	char buf[80];
	FILE * pFig; 
	FILE * pFig_rect;
	/*
	pFig = fopen("tmp.fig","w");
	init_draw(pFig);
	draw_rectangle(pFig,31,22,189,112,SOLID,BLUE);
	fclose(pFig);
	exit(1);
	*/
	for(i=static_num;i<g_size;i++){
		printf("node index = %d\n",i);
		sprintf(buf,"tree_%d.fig",i-static_num);
		printf("Writing %s...\n",buf);
		pFig = fopen(buf,"w");

		sprintf(buf,"rect_%d.fig",i-static_num);
		printf("Writing %s...\n",buf);
		pFig_rect = fopen(buf,"w");

		draw_case(pFig);
		draw_single_source_tree(pFig,parents[i],i);

		draw_case(pFig_rect);
		draw_single_source_rectilinear(pFig_rect,parents[i],i);

		fclose(pFig);
		fclose(pFig_rect);
	}

	free_all();
	return 0;
}

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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "util.h"
#include "ds.h"
#include "connect.h"
#include "bufplace.h"
#include "main.h"

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

	char buf[80];
	int i;
	FILE * pFig; 
	FILE * pFig_rect;

	// start to test
	preprocess_block(&blockage);
	construct_g_all(&blockage,&frame,&sink);

	printf("H:\n");
	for(i=0;i<hfbd_size;i++)
		printf("(%d,%d,%d)\n",hfbd[i].y,hfbd[i].x1,hfbd[i].x2);
	printf("V:\n");
	for(i=0;i<vfbd_size;i++)
		printf("(%d,%d,%d)\n",vfbd[i].x,vfbd[i].y1,vfbd[i].y2);

	int src_idx=block_num*4;
	//output_g_dirs();
	/*
	dijkstra(&blockage,src_idx);
	output_dijkstra();
	pFig= fopen("test.fig","w");
	draw_case(pFig);
	draw_single_source_tree(pFig,via,src_idx);
	return 0;
	*/

	int which = all_pair_shortest();

	printf("floyd\n");
	for(i=0;i<g_size;i++) 
		if(shortest_pair[which][src_idx][i]>=INFINITE)
			printf("%10s","-");
		else	
			printf("%10d",shortest_pair[which][src_idx][i]);
	printf("\n");
	/*
	   for(i=0;i<g_size;i++) printf("%10d",backtrack_pair[which][src_idx][i]);
	   printf("\n");
	   */

	// write results into file
	for(i=static_num;i<g_size;i++){
		if( !g_occupy[i] ) continue;
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

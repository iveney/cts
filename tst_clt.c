// ----------------------------------------------------------------//
// Filename : tst_clt.c
// debug cluster.c
//
// Author : Xiao Zigang
// Modifed: < Wed Mar  4 10:33:31 HKT 2009 >
// ----------------------------------------------------------------//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "util.h"
#include "ds.h"
#include "connect.h"
#include "bufplace.h"
#include "cluster.h"
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

void draw_link_sink(FILE * pFig){
	int i;
	//int cnt=0;
	for(i=0;i<link_num;i++){
		int p=i,q=-1;
		printf("%d:\t",i);
		while( p != -1 ){
			printf("%4d",p);
			if( q != -1 ){
				SNODE * s =&sink.pool[p];
				SNODE * t =&sink.pool[q];
				draw_line(pFig,
					s->x,s->y,
					t->x,t->y,
					SOLID,BLUE);
			}
			q=p;
			p=link_list[p];
		}
		printf("\n");
	}
	/*
	for(i=0;i<sink.num;i++){
		if( isTail[i] ){
			cnt++;
			int p=i,q=-1;
			printf("%d:\t",cnt);
			while( p != -1 ){
				printf("%4d",p);
				if( q != -1 ){
					SNODE * s =&sink.pool[p];
					SNODE * t =&sink.pool[q];
					draw_line(pFig,
						s->x,s->y,
						t->x,t->y,
						SOLID,BLUE);
				}
				q=p;
				p=link_list[p];
			}
			printf("\n");
		}
	}
	*/
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
	FILE * pFig = fopen("cluster.fig","w");

	// start to test
	preprocess_block(&blockage);
	construct_g_all(&blockage,&sink);
	all_pair_shortest();

	cluster_sinks(&blockage,&sink);
	draw_case(pFig);
	draw_link_sink(pFig);
	free_all();
	return 0;
}

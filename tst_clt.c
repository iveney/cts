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

void draw_link_sink(FILE * pFig){
	int i;
	for(i=0;i<link_num;i++){
		printf("%d:\t",i);
		int p=link_info[i].t,q=-1;
		while( p != -1 ){
			printf("%4d",p);
			if( q != -1 ){
				SNODE * s =&sink.pool[p];
				SNODE * t =&sink.pool[q];
				draw_wire(pFig,
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
	int cnt=0;
	for(i=0;i<sink.num;i++){
		if( isTail[i] ){
			int p=i,q=-1;
			printf("%d:\t",cnt++);
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
	preprocess_sinks(&sink);
	construct_g_all(&blockage,&sink);
	all_pair_shortest();

	cluster_sinks(&blockage,&sink);

	draw_case(pFig);
	draw_link_sink(pFig);

	free_clusters();
	free_all();
	return 0;
}

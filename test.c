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
extern int * via;
extern DIRECTION **dirs;
extern char **dir_string;

void init_draw(FILE *pFig){
	fprintf(pFig,"#FIG 3.1\n");
	fprintf(pFig,"Landscape\n");
	fprintf(pFig,"Center\n");
	fprintf(pFig,"Inches\n");
	fprintf(pFig,"1200 2\n");
}

void draw_single_source_tree(FILE * pFig,int src_idx,NODE s, NODE t){
	int i;
	NODE * node = malloc(g_size*sizeof(NODE));
	node[g_size-2]=s;
	node[g_size-1]=t;
	for(i=0;i<blockage.num;i++)
		gen_node(&blockage.pool[i],&node[i*4]);
	for(i=0;i<g_size;i++){
		if( i != src_idx )
			draw_line_node(pFig,node[i],node[via[i]],SOLID,BLUE);
	}
	free(node);
}

void draw_single_source_rectilinear(FILE * pFig,int src_idx,NODE s,NODE t){
	int i;
	NODE * node = malloc(g_size*sizeof(NODE));
	node[g_size-2]=s;
	node[g_size-1]=t;
	for(i=0;i<blockage.num;i++)
		gen_node(&blockage.pool[i],&node[i*4]);
	for(i=0;i<g_size;i++){
		if( i != src_idx ){
			int j=via[i];
			NODE temp;
			if( dirs[i][j] == UP ||
			    dirs[i][j] == DOWN ){
				temp.x = node[i].x;
				temp.y = node[j].y;
			}
			else{
				temp.x = node[j].x;
				temp.y = node[i].y;
			}
			int idx = (int)(dirs[i][j]) ;
			draw_line_node(pFig,node[i],temp,SOLID,BLUE);
			draw_line_node(pFig,node[j],temp,SOLID,BLUE);
		}
	}
	free(node);
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
	FILE * pFig = fopen("fig","w");
	init_draw(pFig);
	draw_blockages(pFig);
	draw_sinks(pFig);
	draw_single_source_rectilinear(pFig,g_size-2,s,t);

	fclose(pFig);
	free_all();
	return 0;
}

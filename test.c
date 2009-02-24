#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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


#define output_block(b) {printf("(%d,%d)\t(%d,%d)\n",b.ll.x,b.ll.y,b.ur.x,b.ur.y);}
int sort_box_hor(const void * p1,const void *p2){
	BOX * l = (BOX *) p1;
	BOX * r = (BOX *) p2;
	return (l->ll.x - r->ll.x);
}
int sort_box_ver(const void * p1,const void *p2){
	BOX * l = (BOX *) p1;
	BOX * r = (BOX *) p2;
	return (l->ll.y - r->ll.y);
}

// preprocess the block to merge the blocks to larger rectangle shape
void preprocess_block(BLOCKAGE * pBlock){
	int n = pBlock->num,i,j;
	int size=0;
	BOX * pBox = malloc(sizeof(BOX)*n);
	BOX * pPool = pBlock->pool;
	BOOL * m = malloc(sizeof(BOOL)*n);
	memcpy(pBox,pBlock->pool,sizeof(BOX)*n);
	memset(m,FALSE,sizeof(BOOL)*n);
	qsort(pPool,n,sizeof(BOX),sort_box_hor); // sort horizontally
	// merge horizontal: from pPool to pBox
	for(i=0;i<n;i++){
		if( m[i] == TRUE ) continue;
		BOX current = pPool[i];
		for(j=i+1;j<n && current.ur.x >= pPool[j].ll.x ;j++){
			if(current.ur.y == pPool[j].ur.y &&
			   current.ll.y == pPool[j].ll.y){
				current.ur = pPool[j].ur;
				m[j] = TRUE;
			}
		}
		pBox[size++] = current;
	}
	// merge vertical: from pBox to pPool
	int new_size=0;
	memset(m,FALSE,sizeof(BOOL)*n);
	qsort(pBox,size,sizeof(BOX),sort_box_ver); // sort vertically
	for(i=0;i<size;i++){
		if( m[i] == TRUE ) continue;
		BOX current = pBox[i];
		for(j=i+1;j<size && current.ur.y >= pBox[j].ll.y;j++){
			if( current.ur.x == pBox[j].ur.x &&
			    current.ll.x == pBox[j].ll.x){
				current.ur = pBox[j].ur;
				m[j] = TRUE;
			}
		}
		pPool[new_size++] = current;
	}
	pBlock->num = new_size; // update size
#ifdef DEBUG
	for(i=0;i<new_size;i++){ output_block(pBlock->pool[i]); }
#endif
	free(pBox);
	free(m);
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
	preprocess_block(&blockage);

	// start to test
	construct_g_all(&blockage,&sink);

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

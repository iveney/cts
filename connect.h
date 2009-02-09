#ifndef __CONNECT_H__
#define __CONNECT_H__

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>
#include "ds.h"

#define ABS(a) ((a)<0.0?-(a):(a))
#define MHT(s,t) (ABS((s.x)-(t.x))+ABS((s.y)-(t.y)))
#define INFINITE 10000.0
static int width=10;
static int precision=2;

typedef struct {double x,y;}Pt;
void set(Pt *p,double xx,double yy){p->x=xx;p->y=yy;}


double **g;	// a size-order matrix of graph 
int g_size=0;

// set all the member of the graph matrix to 0
// REQUIRE: the size of the graph has been set
void clearg(){
	int i,j;
	for(i=0;i<g_size;i++)
		memset(g[i],0,sizeof(double)*g_size);
}

// set all members of the graph matrix to INFINITE except the diagnal
void initg(){
	int i,j;
	for(i=0;i<g_size;i++){
		for(j=0;j<g_size;j++){
			if(i==j) g[i][j] = 0.0;
			else g[i][j] = INFINITE;
		}
	}
}

// output the matrix of graph
void outputg(){
	int i,j;
	static char format[80];
	sprintf(format,"%%%d.%dlf",width,precision); // %10.8lf like format
	for(i=0;i<g_size;i++){
		for(j=0;j<g_size;j++)
			printf(format,g[i][j]);
		printf("\n");
	}
}



// take a list of blockage, construct a graph for shortest path computation
// REQUIRE: external storage g
// list : pointer to BLOCKAGE
int constructg(BLOCKAGE * list){
	// allocate space for g
	g_size = list->num+2;
	g = (double**)malloc((g_size)*sizeof(double*));
	int i,j;
	for(i=0;i<g_size;i++)
		g[i] = (double *) malloc((g_size)*sizeof(double));
	// initialize the elements
	initg();
	// start to construct the graph ... 
	int b_i,b_j;
	int cor_i,cor_j;
	// for each corner of each blockage, 
	// determine what corners it can each
	// (in the sense of manhattan distance) : 4 for-loop
	for(b_i=0;b_i<list->num;b_i++){
		for(b_j=0;b_j<list->num;b_j++){
			for(cor_i=0;cor_i<4;cor_i++){
				for(cor_j=0;cor_j<4;cor_j++){
					if( reach() ){
						//g[i][j] = g[j][i] = MHT();
					}
				}
			}
		}
		return 0;
	}
}

// find the shortest path between two point(dijkstra)
// REQUIRE: the g has been constructed
// s      : point 1
// t      : point 2
// RETURN :  a list of point in the rectilinear path
double find_path(Pt s,Pt t){
	return 0.0;
}

// remember to free the space allocated for g
void destructg(){
	int i;
	for(i=0;i<g_size;i++) free(g[i]);
}

#endif

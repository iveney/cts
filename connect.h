#ifndef __CONNECT_H__
#define __CONNECT_H__

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>
#include "ds.h"

static int width=10;
static int precision=4;

typedef struct {double x,y;}Pt;
void set(Pt *p,double xx,double yy){p->x=xx;p->y=yy;}

#define ABS(a) ((a)<0.0?-(a):(a))
#define MHT(s,t) (ABS((s.x)-(t.x))+ABS((s.y)-(t.y)))

double **g;	// a size-order matrix of graph 
int g_size=0;

// set all the member of the graph matrix to 0
// require: the size of the graph has been set
void clearg(){
	int i,j;
	for(i=0;i<g_size;i++)
		memset(g[i],0,sizeof(double)*g_size);
}

// output all the matrix of graph
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
// require: external storage g
// list : pointer to BLOCKAGE
int constructg(BLOCKAGE * list){
	// allocate space for g
	g_size = list->num+2;
	g = (double**)malloc((g_size)*sizeof(double*));
	int i,j;
	for(i=0;i<g_size;i++)
		g[i] = (double *) malloc((g_size)*sizeof(double));
	// initialize the elements
	clearg();
	return 0;
}

/**/
void destructg(){
	int i;
	for(i=0;i<g_size;i++) free(g[i]);
}

#endif

#include <stdio.h>
#include <stdlib.h>

typedef struct {double x,y;}Pt;
void set(Pt *p,double xx,double yy){p->x=xx;p->y=yy;}

#define ABS(a) ((a)<0.0?-(a):(a))
#define MHT(s,t) (ABS((s.x)-(t.x))+ABS((s.y)-(t.y)))

double **g;	// a size-order matrix of graph 
void clearg(int size){
	int i,j;
	for(i=0;i<size;i++)
		memset(g[i],0,sizeof(double)*size);
}

// take a list of blockage, construct a graph for shortest path computation
// require: external storage g
// list : pointer to BLOCKAGE
// n    : BLOCKAGE count
int construct(BLOCKAGE * list, int n){
}

/**/
void destruct(){
}

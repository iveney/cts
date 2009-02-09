#ifndef __CONNECT_H__
#define __CONNECT_H__

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>
#include "ds.h"

// ----------------------------------------------------------------//
// constant and macros
enum BOOL_VAL{FALSE,TRUE};
enum POS{LL,LR,UR,UL};
enum DIRS{LEFT,RIGHT,UP,LOW,INVALID};
#define ABS(a) ((a)<0.0?(-(a)):(a))
#define MHT(s,t) (ABS((s.x)-(t.x))+ABS((s.y)-(t.y)))
#define MAX(a,b) ((a)>(b)?(a):(b))
#define MIN(a,b) ((a)>(b)?(b):(a))
#define EPSILON 0.0000001
// determines if two double value is the same
#define EQUALDOUBLE(a,b) (ABS((a)-(b))<EPSILON?TRUE:FALSE)
#define INFINITE 10000.0
#define H 1
#define V 0
#define L 10.0

typedef char BOOL;
typedef struct point{double x,y;}Pt;
typedef struct ver_seg{ double x,y1,y2; }VSEG;
typedef struct hor_seg{ double y,x1,x2; }HSEG;
typedef char DIRECTION;

// ----------------------------------------------------------------//
// functions operate on struct

void setpt(Pt *p,double xx,double yy){p->x=xx;p->y=yy;}
void setvseg(VSEG * v,double xx,double yy1,double yy2){
	v->x = xx;
	v->y1 = yy1; v->y2 = yy2;
}
void sethseg(HSEG * h,double yy,double xx1,double xx2){
	h->y = yy;
	h->x1 = xx1; h->x2 = xx2;
}

// ----------------------------------------------------------------//
// global variables

static int width=10;
static int precision=2;
double **g;	// a matrix of graph, order = g_size
int g_size=0;   // size of g
DIRECTION ** dirs; // the move directions between two point,same size as graph
VSEG * vlist;	// vertical list
HSEG * hlist;   // horizontal list
int v_size=0;   // size of vlist
int h_size=0;   // size of hlist

// ----------------------------------------------------------------//
// functions

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
	static char format[20];
	static char inf_string[20];
	sprintf(format,"%%%d.%dlf",width,precision); // %10.8lf like format
	sprintf(inf_string,"%%%ds",width); // %10.8lf like format
	for(i=0;i<g_size;i++){
		for(j=0;j<g_size;j++)
			if( EQUALDOUBLE(g[i][j],INFINITE) )
				printf(inf_string,"-");
			else printf(format,g[i][j]);
		printf("\n");
	}
}

// determin if two rectilinear segment intersects
// hor : the horizontal segment
// ver : the vertical segment
BOOL intersect(HSEG hor,VSEG ver){
	if( ver.x > hor.x1 && ver.x < hor.x2 &&
	    hor.y > ver.y1 && hor.y < ver.y2)
		return TRUE;
	return FALSE;
}

// determine if two points are reachable in the sense of manhattan distance
// REQUIRE: a list of vertical segments
//          a list of horizontal segments
// RETURN : their manhattan distance, 
//          and the moving direction from a to b
//          if not, return a negative double
double reachable(NODE a,NODE b){
	// there are two different manhattan paths in total
	// for each path, traverse all the line segments to see if 
	// there is intersections
	// first gen the four segments:
	//         2
	//    .---------.
	// 3  |         |  4
	//    `---------`
	//         1
	if(a.x > b.x ){// ensures a.x < b.x
		NODE tmp = a;
		a = b; b = tmp;
	}
	HSEG hor; VSEG ver;
	int i,j;

	// path 1:
	hor.y = a.y;
	hor.x1 = a.x;
	hor.x2 = b.x;

	ver.x = b.x;
	ver.y1 = MIN(a.y,b.y);
	ver.y2 = MAX(a.y,b.y);
	BOOL result = TRUE;
	for(i=0;i<v_size;i++){// for vertical list
		if( intersect(hor,vlist[i]) ){
			result = FALSE;
			break;
		}
	}
	if( result != FALSE ){
		for(j=0;j<h_size;j++){// for horizontal list
			if( intersect(hlist[j],ver) ){
				result = FALSE;
				break;
			}
		}
	}
	if( result == TRUE ) return MHT(a,b);

	// path 2:
	hor.y = b.y; // only y pos changed
	ver.x = a.x; // only x pos changed
	result = TRUE;
	for(i=0;i<v_size;i++){// for vertical list
		if( intersect(hor,vlist[i]) ){
			result = FALSE;
			break;
		}
	}
	if( result != FALSE ){
		for(j=0;j<h_size;j++){// for horizontal list
			if( intersect(hlist[j],ver) ){
				result = FALSE;
				break;
			}
		}
	}

	if( result == TRUE ) return MHT(a,b);
	return -10.0;
}

// generate four nodes from a block
// ll=0, lr=1, ur=2, ul=3
// node: store the four nodes, must have 4 elements
// b   : the block
void gen_node(BOX * b,NODE * node){
	node[LL] = b->ll;
	node[UR] = b->ur;

	node[LR].x = b->ur.x;
	node[LR].y = b->ll.y;

	node[UL].x = b->ll.x;
	node[UL].y = b->ur.y;
}

// generate horizontal and vertical list
// the list ensures that:
// for ver: y1<y2
// for hor: x1<x2
// NOTE   : if some block has a width/height < L, it is not considered a block in vertical/horizontal
// REQUIRE: a list of blockage
int gen_segments(BLOCKAGE * list){
	// for each block, there will be 2 vertical and 2 horizontal seg
	int size = list->num * 2;
	hlist = malloc(size * sizeof(HSEG));
	vlist = malloc(size * sizeof(VSEG));
	h_size=v_size=0;
	int i;
	NODE node[4];
	for(i=0;i<list->num;i++){
		BOX * pb = &list->pool[i];
		int p=h_size*2;
		int q=p+1;
		// add horizontal segment
		if( ABS(pb->ll.y - pb->ur.y ) >= L ){
			hlist[p].y = pb->ll.y;
			hlist[q].y = pb->ur.y;
			hlist[p].x1 = hlist[q].x1 = pb->ll.x;
			hlist[p].x2 = hlist[q].x2 = pb->ur.x;
			h_size++;
		}
		// add vertical segment
		if( ABS(pb->ll.x - pb->ur.x) >= L ){
			vlist[p].x = pb->ll.x;
			vlist[q].x = pb->ur.x;
			vlist[p].y1 = vlist[q].y1 = pb->ll.y;
			vlist[p].y2 = vlist[q].y2 = pb->ur.y;
			v_size++;
		}
	}
	// may sort the list from left to right, low to up
	return 0;
}

// take a list of blockage, construct a graph for shortest path computation
// REQUIRE: external storage g
// list : pointer to BLOCKAGE
int constructg(BLOCKAGE * list){
	// allocate space for g
	g_size = list->num*4+2;
	g = (double**)malloc((g_size)*sizeof(double*));
	int i,j;
	for(i=0;i<g_size;i++)
		g[i] = (double *) malloc((g_size)*sizeof(double));
	initg(); // initialize the elements
	gen_segments(list); // generate segments from blockages

	// start to construct the graph 
	int b_i,b_j;
	int cor_i,cor_j;
	// for each corner of each blockage, 
	// determine what corners it can each
	// (in the sense of manhattan distance) : 4 for-loop
	for(b_i=0;b_i<list->num;b_i++){
		BOX * boxi = &list->pool[b_i];
		for(b_j=b_i;b_j<list->num;b_j++){
			BOX * boxj = &list->pool[b_j];
			NODE nodei[4],nodej[4];
			// generate the 4 nodes of each blockage
			gen_node(boxi,nodei);
			gen_node(boxj,nodej);
			// note that need not to calculate the same point

			for(cor_i=0;cor_i<4;cor_i++){
				for(cor_j=0;cor_j<4;cor_j++){
					if(b_i == b_j && cor_i == cor_j )
						continue;
					double dist = reachable(nodei[cor_i],nodej[cor_j]);
					int idx1 = b_i*4+cor_i;
					int idx2 = b_j*4+cor_j;
					if( dist > -1.0 ){
						g[idx1][idx2]=g[idx2][idx1] = dist;
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
void destroy_g(){
	int i;
	for(i=0;i<g_size;i++) free(g[i]);
}

// remember to free the space allocated for segments
void destroy_segments(){
	int i;
	free(vlist);
	free(hlist);
}

// free all the allocated memory here
void free_all(){
	destroy_g();
	destroy_segments();
}

#endif

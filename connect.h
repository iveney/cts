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
enum POS{LL,LR,UR,UL}; // low-left, low-right, up-right, up-left
enum DIRS{INVALID,LEFT,RIGHT,UP,DOWN};
const static char *dir_string[]={"-","L","R","U","D"};
#define ABS(a) ((a)<0.0?(-(a)):(a))
#define MHT(s,t) (ABS((s.x)-(t.x))+ABS((s.y)-(t.y)))
#define MAX(a,b) ((a)>(b)?(a):(b))
#define MIN(a,b) ((a)>(b)?(b):(a))
#define EPSILON 0.0000000001
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

/*
int reverse_dir(DIRECTION origin){
	DIRECTION result;
	switch(origin){
	case LEFT:    result = RIGHT; break;
	case RIGHT:   result = LEFT; break;
	case UP:      result = DOWN; break;
	case DOWN:     result = UP; break;
	}
	return result;
}
*/

// set all the member of the graph matrix to 0
// REQUIRE: the size of the graph has been set
void clearg(){
	int i,j;
	for(i=0;i<g_size;i++)
		memset(g[i],0,sizeof(double)*g_size);
}

// set all members of the graph matrix to INFINITE except the diagnal
// n    : number of blockages
void initg(int n){
	// allocate space for g and dirs
	g_size = n*4+2;
	g = (double**)malloc((g_size)*sizeof(double*));
	dirs = (DIRECTION**)malloc((g_size)*sizeof(DIRECTION*));
	int i,j;
	for(i=0;i<g_size;i++){
		g[i] = (double *) malloc(g_size*sizeof(double));
		dirs[i] = (DIRECTION *) malloc(g_size*sizeof(DIRECTION));
		memset(dirs[i],INVALID,g_size*sizeof(DIRECTION));// INVALID = 0
	}

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

void output_dirs(){
	int i,j;
	static char format[20];
	sprintf(format,"%%%ds",width); 
	for(i=0;i<g_size;i++){
		for(j=0;j<g_size;j++){
			printf(format,dir_string[dirs[i][j]]);
		}
		printf("\n");
	}
}

// determin if two rectilinear segment intersects
// hor : the horizontal segment
// ver : the vertical segment
// note: for hor, x1<x2
//       for ver, y1<y2
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
double reachable(NODE a,NODE b,DIRECTION * a2b,DIRECTION * b2a){
	// for each path, traverse all the line segments to see if 
	// there is intersections
	// first gen the four segments:
	//         2
	//    .---------.
	// 3  |         |  4
	//    `---------`
	//         1
	// there are two different manhattan paths in {14,32,31,24}
	HSEG hor1,hor2;
	VSEG ver3,ver4;
	hor1.x1 = hor2.x1 = ver3.x = MIN(a.x,b.x);
	hor1.x2 = hor2.x2 = ver4.x = MAX(a.x,b.x);
	hor1.y = ver3.y1 = ver4.y1 = MIN(a.y,b.y);
	hor2.y = ver3.y2 = ver4.y2 = MAX(a.y,b.y);

	// hor[0] and ver[0] is the hor-move first path
	// ver[1] and hor[1] is the ver-move first path
	HSEG hor[2];
	VSEG ver[2];
	int relative;

	// determine the relative position of two points
	// and decide which two paths to use
	if( a.x < b.x ){// a is left to b
		if( a.y < b. y ){// a is low-left to b
			relative = LL;
			hor[0] = hor1;
			ver[0] = ver4;
			ver[1] = ver3;
			hor[1] = hor2;
		}
		else{// a is up-left to b
			relative = UL;
			hor[0] = hor2;
			ver[0] = ver4;
			ver[1] = ver3;
			hor[1] = hor1;
		}
	}
	else{// a is right to b
		if( a.y < b. y ){// a is low-right to b
			relative = LR;
			hor[0] = hor1;
			ver[0] = ver3;
			ver[1] = ver4;
			hor[1] = hor2;

		}
		else{// a is up-right to b
			relative = UR;
			hor[0] = hor2;
			ver[0] = ver3;
			ver[1] = ver4;
			hor[1] = hor1;
		}
	}
	
	
	// check if either of the path is valid
	// if yes, get MHT and set direction
	int i,j;

	// try path 1:
	BOOL result = TRUE;
	for(i=0;i<v_size;i++){// for vertical list
		if( intersect(hor[0],vlist[i]) ){
			result = FALSE;
			break;
		}
	}
	if( result != FALSE ){
		for(j=0;j<h_size;j++){// for horizontal list
			if( intersect(hlist[j],ver[0]) ){
				result = FALSE;
				break;
			}
		}
	}
	if( result == TRUE ) {// path 1 succeed
		// ** judge the moving directions **
		switch(relative){
		case LL:
			*a2b = RIGHT; *b2a = DOWN;break;
		case UL:
			*a2b = RIGHT; *b2a = UP;break;
		case LR:
			*a2b = LEFT;  *b2a = DOWN;break;
		case UR:
			*a2b = LEFT;  *b2a = UP;break;
		}
		return MHT(a,b);
	}

	// try path 2:
	result = TRUE;
	for(i=0;i<v_size;i++){// for vertical list
		if( intersect(hor[1],vlist[i]) ){
			result = FALSE;
			break;
		}
	}
	if( result != FALSE ){
		for(j=0;j<h_size;j++){// for horizontal list
			if( intersect(hlist[j],ver[1]) ){
				result = FALSE;
				break;
			}
		}
	}
	if( result == TRUE ) {// path 2 succeed
		switch(relative){
		case LL:
			*a2b = UP;   *b2a = LEFT; break;
		case LR:
			*a2b = UP;   *b2a = RIGHT;break;
		case UL:
			*a2b = DOWN;  *b2a = LEFT;break;
		case UR:
			*a2b = DOWN;  *b2a = RIGHT;break;
		}
		return MHT(a,b);
	}

	return -10.0;// neither succeed
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
// NOTE   : if some block has a width/height < L, 
//          it is not considered a block in vertical/horizontal
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
		int p=h_size;
		int q=p+1;
		// add horizontal segment
		if( ABS(ABS(pb->ll.y - pb->ur.y) - L) > EPSILON ){// height
			hlist[p].y = pb->ll.y;
			hlist[q].y = pb->ur.y;
			hlist[p].x1 = hlist[q].x1 = pb->ll.x;
			hlist[p].x2 = hlist[q].x2 = pb->ur.x;
			h_size+=2;
		}
		// add vertical segment
		p=v_size;
		q=p+1;
		if( ABS(ABS(pb->ll.x - pb->ur.x) - L) > EPSILON ){// width
			vlist[p].x = pb->ll.x;
			vlist[q].x = pb->ur.x;
			vlist[p].y1 = vlist[q].y1 = pb->ll.y;
			vlist[p].y2 = vlist[q].y2 = pb->ur.y;
			v_size+=2;
		}
	}
	// may sort the list from left to right, low to up
	return 0;
}

// take a list of blockage, construct a graph for shortest path computation
// REQUIRE: external storage g
// list : pointer to BLOCKAGE
int constructg(BLOCKAGE * list){
	initg(list->num); // initialize the elements
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
			// 3--2
			// |  |
			// 0--1
			gen_node(boxi,nodei);
			gen_node(boxj,nodej);

			for(cor_i=0;cor_i<4;cor_i++){
				for(cor_j=0;cor_j<4;cor_j++){
					// need not to calculate the same point
					if(b_i == b_j && cor_i == cor_j )
						continue;
					int idx1 = b_i*4+cor_i;
					int idx2 = b_j*4+cor_j;
					double dist = reachable(
							nodei[cor_i],
							nodej[cor_j],
							&dirs[idx1][idx2],
							&dirs[idx2][idx1]);
					if( dist > -1.0 ){
						g[idx1][idx2] = 
							g[idx2][idx1] = dist;
					}
				}
			}
		}
	}
	return 0;
}

// add a point to the constructed graph from blockage list `l'
// REQUIRE : the graph
// pt      : the point to add
void addpt(NODE pt,BLOCKAGE * list){
}

void add2pt(NODE s,NODE t){
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
	for(i=0;i<g_size;i++) {
		free(g[i]);
		free(dirs[i]);
	}
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

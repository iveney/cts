// ----------------------------------------------------------------//
// Filename : connect.c
// Source code for all pair shortest path and single source shortest
// path. Also some global varibales for use
//
// Author : Xiao Zigang
// Modifed: <Fri Feb 13 16:46:54 HKT 2009> 
// ----------------------------------------------------------------//

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>
#include "util.h"
#include "ds.h"
#include "connect.h"
// ----------------------------------------------------------------//
// global variables
const static char *dir_string[]={"-","L","R","U","D"}; // for output

//static int precision=2; // controls double type output precision
static int width=10;    // controls output width

// variables for graph
UINT ** g =NULL;        // a matrix of graph, order = g_size
DIRECTION ** dirs=NULL; // the move directions between two point
NODE * g_node=NULL;     // [0..static_num-1]=block corners,(rest)=sinks
BOOL * use_corner=NULL; // mark if a corner of a block is usable
BOOL * g_occupy=NULL;   // mark if g_node is a valid node
NODE * sink_node=NULL;  // points to the first sink node in g_node
BLOCKAGE * pBlock=NULL; // pointer to blockage list
int block_num=0;	// number of blockages
int static_num=0;       // number of static nodes (shoulb be block_num*4)
int sink_num=0;         // number of sinks
int g_size=0;           // total size of g(total nodes available)
int g_num=0;            // number of nodes in g currently

// stores the edges of blockages
VSEG * vlist=NULL;	// vertical list
HSEG * hlist=NULL;      // horizontal list
int v_size=0;           // size of vlist
int h_size=0;           // size of hlist

// stores some forbidden edges
VSEG * vfbd = NULL;  //
HSEG * hfbd = NULL;  //
int vfbd_size=0;
int hfbd_size=0;

// stores the forbidden static node
BOOL ** fbdnode;

// variables for dijkstra
UINT * shortest=NULL;   // shortest path shortest vector, size = g_size
int * via=NULL;	        // backtrack vector, size = g_size;
BOOL * mark=NULL;       // mark if a node is visited

// variables for floyd, use swtich array technique
// size should be 2 * g_size * g_size
UINT ** pairs;
int ** parents;
// for internal use of floyd
UINT **shortest_pair[2]={NULL,NULL};   // shortest pair matrix
int ** backtrack_pair[2]={NULL,NULL};  // backmatrix

// ----------------------------------------------------------------//
// functions operate on struct
// set the value of a vertical segment
void setvseg(VSEG * v,UINT xx,UINT yy1,UINT yy2){
	v->x = xx;
	v->y1 = yy1; v->y2 = yy2;
}
// set the value of a horizontal segment
void sethseg(HSEG * h,UINT yy,UINT xx1,UINT xx2){
	h->y = yy;
	h->x1 = xx1; h->x2 = xx2;
}

// determin if two rectilinear segment intersects
// hor : the horizontal segment
// ver : the vertical segment
// note: for hor, x1<x2
//       for ver, y1<y2
inline BOOL intersect(HSEG hor,VSEG ver){
	//if( DOUBLE_GT(ver.x,hor.x1) && DOUBLE_LT(ver.x,hor.x2) &&
	//    DOUBLE_GT(hor.y,ver.y1) && DOUBLE_LT(hor.y,ver.y2) )
	if( ver.x > hor.x1 && ver.x < hor.x2 &&
	    hor.y > ver.y1 && hor.y < ver.y2)
		return TRUE;
	return FALSE;
}

// determins if a segment is in block(including partially in)
inline BOOL inBlock(BOX * pb, void * pSeg, int segtype){
	HSEG *ph;
	VSEG *pv;
	switch(segtype){
	case H:
		ph = (HSEG*)pSeg;
		if( ph->y > pb->ll.y && ph->y < pb->ur.y ){
			if( (ph->x1>pb->ll.x && ph->x1<pb->ur.x) ||
			    (ph->x2>pb->ll.x && ph->x2<pb->ur.x) ||
			    (ph->x1>=pb->ll.x && ph->x2<=pb->ur.x) )
				return TRUE;
		}
		break;
	case V:
		pv = (VSEG*)pSeg;
		if( pv->x > pb->ll.x && pv->x < pb->ur.x ){
			if( (pv->y1>pb->ll.y && pv->y1<pb->ur.y) ||
			    (pv->y2>pb->ll.y && pv->y2<pb->ur.y) ||
			    (pv->y1>=pb->ll.y && pv->y2<=pb->ur.y) )
				return TRUE;
		}
		break;
	}
	return FALSE;
}

inline int ver_overlap(VSEG v1,VSEG v2){
	if( v1.x != v2.x ) return 0;
	else if (v1.y2<=v2.y1 || v1.y1>=v2.y2) return 0;
	else return 1;
}

inline int hor_overlap(HSEG h1,HSEG h2){
	if( h1.y != h2.y ) return 0;
	else if (h1.x2<=h2.x1 || h1.x1>=h2.x2) return 0;
	else return 1;
}

// ----------------------------------------------------------------//
// functions

// construct the whole graph with a list of blockages and sinks
void construct_g_all(BLOCKAGE * blocks, SINK * sink){
	// first construct the static part of graph(blockage corners)
	pBlock = blocks;
	block_num = blocks->num;
	sink_num = sink->num;
	static_num = block_num*4;
	g_num = static_num;               // only have static_num nodes now
	allocate_g(static_num+sink_num);  // allocate space for all
	init_g();                         // initialize the elements
	gen_block_node(blocks);           // generate a set of blockage corners
	gen_segments(blocks);             // generate segments from blockages
	constructg(blocks); 

	// now copy the sinks into g_node
	copy_sink(sink);
	int i;
	// add every sink point into the graph
	//for(i=0;i<g_size;i++) printf("g_occupy[%d]=%d\n",i,(int)g_occupy[i]);//
	for(i=0;i<sink_num;i++){
		//printf("inserting %d\n",i);
		insertpt(sink_node[i]);
	}
}

// determines if a node is in a blockage
// including the case that the node is in the bounding box of rect
// true if they intersects
BOOL inRect(NODE * node,BOX * b){
	int x = node->x,y=node->y;
	NODE * ll = &b->ll;
	NODE * ur = &b->ur;
	if( (x<ur->x && x>ll->x) &&
	    (y<ur->y && y>ll->y) ){
#ifdef DEBUG
		printf("(%d,%d) sec (%d %d),(%d %d)\n",x,y,
				ll->x,ll->y,
				ur->x,ur->y);
#endif
		return TRUE;
	}
	return FALSE;
}

#define setfbdnode(a,b,value) {fbdnode[(a)][(b)]=fbdnode[(b)][(a)]=(value);}
#define setgnode(a,b,value) {g[(a)][(b)]=g[(b)][(a)]=(value);}
#define set_g_fbd(a,b) {setfbdnode((a),(b),TRUE);setgnode((a),(b),INFINITE);}

int mark_forbidden(BLOCKAGE * block){
	NODE *nodei,*nodej;
	BOX * boxi,*boxj;
	int b_i,b_j;
	for(b_i=0;b_i<block->num;b_i++){
		boxi = &block->pool[b_i];
		nodei = g_node + b_i*4;
		for(b_j=b_i+1;b_j<block->num;b_j++){
			boxj = &block->pool[b_j];
			nodej = g_node+b_j*4;
			///// checking adjacency  /////

			// check if vertical adjacent
			BOX *p1=boxi,*p2=boxj;
			int l=b_i*4;
			int r=b_j*4;
			if(boxi->ll.x > boxj->ll.x ) {
				// ensures p1 has smaller x
				p1=boxj;
				p2=boxi;
				l=b_j*4;
				r=b_i*4;
			}
			VSEG v1,v2;
			setvseg(&v1,p1->ur.x,p1->ll.y,p1->ur.y);
			setvseg(&v2,p2->ll.x,p2->ll.y,p2->ur.y);
			// check if horizontal adjacent
			if( v1.x  != v2.x  ||
			    v1.y2 <= v2.y1 ||
			    v2.y2 <= v1.y1){
				; // do nothing
			}
			else{// they share a vertical edge
				VSEG v;
				setvseg(&v,v1.x,v1.y1,v1.y2);
				set_g_fbd(l+1,l+2);
				set_g_fbd(r+0,r+3);
				set_g_fbd(l+1,r+3);
				set_g_fbd(l+2,r+0);

				set_g_fbd(l+1,r+3);
				set_g_fbd(l+2,r+0);
				//g[l+1][l+2] = g[l+2][l+1] = INFINITE;
				//g[r+0][r+3] = g[r+3][r+0] = INFINITE;
				//g[l+1][r+3] = g[r+3][l+1] = INFINITE;
				//g[l+2][r+0] = g[r+0][l+2] = INFINITE;
				//fbdnode[l+1][r+3]=fbdnode[r+3][l+1] = 
				//fbdnode[l+2][r+0]=fbdnode[r+0][l+2] = TRUE;
				v.y1=MAX(v1.y1,v2.y1);
				v.y2=MIN(v1.y2,v2.y2);
				if( v1.y1 < v2.y1 ){// (3,0)
					set_g_fbd(l+3,r+0);
					//g[l+3][r+0] = g[r+0][l+3] = INFINITE;
					//fbdnode[l+3][r+0] = fbdnode[r+0][l+3] = TRUE;
					if( v1.y2 < v2.y2 ){//(2,1)
						set_g_fbd(l+2,r+1);
						//g[l+2][r+1] = g[r+1][l+2] = INFINITE;
						//fp[2][1] = TRUE;
					}
					else if( v1.y2 > v2.y2 ){//(0,3)
						set_g_fbd(l+0,r+3);
						//g[l+0][r+3] = g[r+3][l+0] = INFINITE;
						//fp[0][3] = TRUE;
					}
				}
				else if( v1.y1 > v2.y1 ){// (1,2)
					set_g_fbd(l+1,r+2);
					//g[l+1][r+2] = g[r+2][l+1] = INFINITE;
					//fp[1][2] = TRUE;
					if( v1.y2 > v2.y2 ){// (0,3)
						set_g_fbd(l+0,r+3);
						//g[l+0][r+3] = g[r+3][l+0] = INFINITE;
						//fp[0][3] = TRUE;
					}
					else if( v1.y2 < v2.y2 ){// (2,1)
						set_g_fbd(l+2,r+1);
						//g[l+2][r+1] = g[r+1][l+2] = INFINITE;
						//fp[2][1] = TRUE;
					}
				}
				vfbd[vfbd_size++] = v;
			}

			//memset(fp,FALSE,sizeof(fp));
			p1=boxi,p2=boxj;
			int d=b_i*4;
			int u=b_j*4;
			if(boxi->ll.y > boxj->ll.y){
				p1=boxj;
				p2=boxi;
				d=b_j*4;
				u=b_i*4;
			}
			HSEG h1,h2;
			sethseg(&h1,p1->ur.y,p1->ll.x,p1->ur.x);
			sethseg(&h2,p2->ll.y,p2->ll.x,p2->ur.x);
			if( h1.y  != h2.y  || 
			    h1.x2 <= h2.x1 ||
			    h2.x2 <= h1.x1){
				;
			}
			else{// they share a horizontal edge
				HSEG h;
				sethseg(&h,h1.y,h1.x1,h1.x2);
				set_g_fbd(d+2,d+3); 
				set_g_fbd(u+0,u+1); 
				set_g_fbd(d+2,u+0); 				
				set_g_fbd(d+3,u+1); 

				set_g_fbd(d+2,u+0);
				set_g_fbd(d+3,u+1);
				//g[d+2][d+3] = g[d+3][d+2] = INFINITE;
				//g[u+0][u+1] = g[u+1][u+0] = INFINITE;
				//g[d+2][u+0] = g[u+0][d+2] = INFINITE;
				//g[d+3][u+1] = g[u+1][d+3] = INFINITE;
				//fp[2][0] = fp[3][1] = TRUE;
				h.x1=MAX(h1.x1,h2.x1);
				h.x2=MIN(h1.x2,h2.x2);
				if( h1.x1 < h2.x1 ){
					set_g_fbd(d+1,u+0);
					//g[d+1][u+0] = g[u+3][d+0] = INFINITE;
					//fp[1][0] = TRUE;
					if( h1.x2 < h2.x2 ){
						set_g_fbd(d+2,u+3);
						//g[d+2][u+3] = g[u+3][d+2] = INFINITE;
						//fp[2][3] = TRUE;
					}
					else if( h1.x2 > h2.x2 ){
						set_g_fbd(d+0,u+1);
						//g[d+0][u+1] = g[u+1][d+0] = INFINITE;
						//fp[0][1] = TRUE;
						h.x2 = h2.x2;
					}
				}
				else if( h1.x1 > h2.x1 ){
					set_g_fbd(d+3,u+3);
					//g[d+3][u+2] = g[u+2][d+3] = INFINITE;
					//fp[3][2] = TRUE;
					if( h1.x2 > h2.x2 ){
						set_g_fbd(d+0,u+1);
						//g[d+0][u+1] = g[u+1][d+0] = INFINITE;
						//fp[0][1] = TRUE;
					}
					else if( h1.x2 < h2.x2 ){
						set_g_fbd(d+2,u+3);
						//g[d+2][u+3] = g[u+3][d+2] = INFINITE;
						//fp[2][3] = TRUE;
					}
				}
				hfbd[hfbd_size++] = h;
			}// end of share horizontal edge
		}// end of for b_j
	}//end of for b_i
	return 0;
	///// end of checking adjacency  /////
}

// take a list of blockage, construct a graph for shortest path computation
// REQUIRE: external storage g
// list : pointer to BLOCKAGE
int constructg(BLOCKAGE * block){
	// start to construct the graph 
	mark_forbidden(block);
	int b_i,b_j;
	int cor_i,cor_j;
	// for each corner of each blockage, 
	// determine what corners it can each
	// (in the sense of manhattan distance) : 4 for-loop
	NODE *nodei,*nodej;
	BOX * boxi,*boxj;
	for(b_i=0;b_i<block->num;b_i++){
		boxi = &block->pool[b_i];
		nodei = g_node + b_i*4;
		for(b_j=b_i+1;b_j<block->num;b_j++){
			boxj = &block->pool[b_j];
			nodej = g_node+b_j*4;
			BOOL fp[4][4]; // forbid pair
			memset(fp,FALSE,sizeof(fp));
			// generate the 4 nodes of each blockage
			// 3--2
			// |  |
			// 0--1

			for(cor_i=0;cor_i<4;cor_i++){
				// handle the intersection case
				if( inRect(&nodei[cor_i],boxj) ){
					// mark it unusable
					use_corner[b_i*4+cor_i] = FALSE;
					continue;
				}
				for(cor_j=0;cor_j<4;cor_j++){
					if( inRect(&nodej[cor_j],boxi) ){
						use_corner[b_j*4+cor_j] = FALSE;
						continue;
					}
					// marked as not connectable
					// need not to calculate the same point
					int idx1 = b_i*4+cor_i;
					int idx2 = b_j*4+cor_j;
					if( fbdnode[idx1][idx2] ) continue;
					BOOL result = reach( nodei[cor_i], nodej[cor_j],
					       idx1,idx2);
					//if( cor_i == 3 && cor_j == 0 )
						//printf("result = %d\n",result);
				}// end of for cor_j
			}// end of for cor_i

		}//end of for b_j

		// for block b_i, connect its four points(self connect)
		reach(nodei[0],nodei[1],b_i*4+0,b_i*4+1);
		reach(nodei[1],nodei[2],b_i*4+1,b_i*4+2);
		reach(nodei[2],nodei[3],b_i*4+2,b_i*4+3);
		reach(nodei[3],nodei[0],b_i*4+3,b_i*4+0);

	}// end of for b_i
	return 0;
}

// allocate space for graph, directions, shortest path, via, 
// g_node and mark, floyd and backtrack 
// set all members of the graph matrix to INFINITE except the diagnal
// n    : number of max nodes
void allocate_g(int size){
	g_size		= size;
	g		= (UINT**) malloc((g_size)*sizeof(UINT*));
	g_node		= (NODE*)  malloc((g_size)*sizeof(NODE));
	use_corner      = (BOOL*) malloc((g_size)*sizeof(BOOL));
	sink_node	= g_node+static_num;   // points to first sink node
	g_occupy	= (BOOL*)  malloc((g_size)*sizeof(BOOL));
	dirs		= (DIRECTION**) malloc((g_size)*sizeof(DIRECTION*));
	fbdnode         = (BOOL**) malloc((g_size)*sizeof(BOOL*));

	// for dijkstra
	shortest = (UINT *) malloc(g_size * sizeof(UINT));
	via      = (int *) malloc(g_size * sizeof(int));
	mark     = (BOOL*) malloc(g_size * sizeof(BOOL));

	// for floyd
	int x,y;
	for(x=0;x<2;x++){// dimension 2 allocation
		shortest_pair[x]  = malloc(sizeof(UINT**) * g_size);
		backtrack_pair[x] = malloc(sizeof(int**) * g_size);
		for(y=0;y<g_size;y++){// dimension 3 allocation
			shortest_pair[x][y] = malloc(sizeof(UINT*) * g_size);
			backtrack_pair[x][y] = malloc(sizeof(int*) * g_size);
		}
	}

	int i;
	for(i=0;i<g_size;i++){// 2-dimension allocation
		g[i] = (UINT *) malloc(g_size*sizeof(UINT));
		dirs[i] = (DIRECTION *) malloc(g_size*sizeof(DIRECTION));
		fbdnode[i] = (BOOL*) malloc(g_size*sizeof(UINT));
	}
}

// initialize the graph to be not connected
// the directions between points are INVALID
void init_g(){
	int i,j;
	for(i=0;i<g_size;i++){
		for(j=0;j<g_size;j++){
			if(i==j) g[i][j] = 0;
			else g[i][j] = INFINITE;
			dirs[i][j] = INVALID;
			fbdnode[i][j] = FALSE;
		}
		g_occupy[i]=FALSE;
		use_corner[i]=TRUE;
	}
}

// generate all nodes of blocakges and stores it
// REQUIRE: a list of blockages
BOOL gen_block_node(BLOCKAGE * blockage){
	int i;
	for(i=0;i<block_num;i++) gen_node(&blockage->pool[i],&g_node[i*4]);
	// allocate [0..static_num] to the blockage corner nodes
	for(i=0;i<static_num;i++) 
		g_occupy[i] = TRUE; 
	return TRUE;
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

// NOTE : for each node, adjust one unit
	/*
	--node[LL].x;
	--node[UR].y;
	++node[LR].x;
	--node[LR].y;
	--node[UL].x;
	++node[UL].y;
	++node[UR].x;
	++node[UR].y;
	*/
}

// sort the vertical list (ascending x)
int sort_vseg(const void *v1,const void *v2){
	VSEG *p1=(VSEG*)v1;
	VSEG *p2=(VSEG*)v2;
	return (p1->x - p2->x);
}

int sort_hseg(const void *h1,const void *h2){
	HSEG *p1=(HSEG*)h1;
	HSEG *p2=(HSEG*)h2;
	return (p1->y - p2->y);
}

// generate horizontal and vertical list
// the list ensures that:
// for ver: y1<y2
// for hor: x1<x2
// NOTE   : if some block has a width/height < L, 
//          it is not considered a block in vertical/horizontal
// REQUIRE: a list of blockage
int gen_segments(BLOCKAGE * block){
	// for each block, there will be 2 vertical and 2 horizontal seg
	int size = block->num * 2;
	hlist = malloc(size * sizeof(HSEG));
	vlist = malloc(size * sizeof(VSEG));
	hfbd = malloc(size * sizeof(HSEG));
	vfbd = malloc(size * sizeof(HSEG));
	h_size=v_size=0;
	hfbd_size = vfbd_size = 0;
	int i;
	for(i=0;i<block->num;i++){
		BOX * pb = &block->pool[i];
		int p=h_size;
		int q=p+1;
		// add horizontal segment
		//if( ABS(pb->ll.y - pb->ur.y) > _L_ ){// height
			hlist[p].y = pb->ll.y;
			hlist[q].y = pb->ur.y;
			hlist[p].x1 = hlist[q].x1 = pb->ll.x;
			hlist[p].x2 = hlist[q].x2 = pb->ur.x;
			h_size+=2;
		//}
		// add vertical segment
		p=v_size;
		q=p+1;
		//if( ABS(pb->ll.x - pb->ur.x) > _L_ ){// width
			vlist[p].x = pb->ll.x;
			vlist[q].x = pb->ur.x;
			vlist[p].y1 = vlist[q].y1 = pb->ll.y;
			vlist[p].y2 = vlist[q].y2 = pb->ur.y;
			v_size+=2;
		//}
	}
	// may sort the block from left to right, low to up
	qsort(vlist,v_size,sizeof(VSEG),sort_vseg);
	qsort(hlist,h_size,sizeof(HSEG),sort_hseg);
	return 0;
}

// copy a list of sink nodes into g_node;
void copy_sink(SINK * sink){
	int i;
	for(i=0;i<sink->num;i++){
		sink_node[i].x = sink->pool[i].x;
		sink_node[i].y = sink->pool[i].y;
	}
}

// determine if two points are reach in the sense of manhattan distance
// REQUIRE: a list of vertical segments
//          a list of horizontal segments
// RESULT : update their distance in g
//          and the moving direction from a to b
//          if not, return FALSE
BOOL reach(NODE a,NODE b,int idx_a,int idx_b){
	// for each path, traverse all the line segments to see if 
	// there is intersections
	// first gen the four segments:
	//         2
	//    .---------.
	// 3  |         |  4
	//    `---------`
	//         1
	// there are two different manhattan paths in {14,32,31,24}
	DIRECTION * a2b = &dirs[idx_a][idx_b];
	DIRECTION * b2a = &dirs[idx_b][idx_a];
	HSEG hor1,hor2;
	VSEG ver3,ver4;
	// NOTE: MIN/MAX does not consider precision
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
	if( a.x<b.x ){// a is left to b
		if( a.y<b.y ){// a is low-left to b
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
		if( a.y<b.y ){// a is low-right to b
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

	// try path 1: hor first then ver
	// check if horizontal path cut by some block
	for(i=0;i<v_size;i++)
		if( intersect(hor[0],vlist[i]) )
			goto CHECK_PATH2;
	printf("pass 1\n");

	// check if it passes some forbidden vertical segment
	for(i=0;i<hfbd_size;i++)
		if( hor_overlap(hor[0],hfbd[i]) )
			goto CHECK_PATH2;
	printf("pass 2\n");

	// check if vertical path cut by some block
	for(j=0;j<h_size;j++)
		if( intersect(hlist[j],ver[0]) )
			goto CHECK_PATH2;
	printf("pass 3\n");

	// check if it passes some forbidden horizontal segment
	for(j=0;j<vfbd_size;j++){
		//printf("checking:(%d %d %d)\n",ver[0].x,ver[0].y1,ver[0].y2);
		//printf("and:(%d %d %d)\n",vfbd[j].x,vfbd[j].y1,vfbd[j].y2);
		if( ver_overlap(ver[0],vfbd[j]) )
			goto CHECK_PATH2;
	}
	printf("pass 4\n");

	// finally... check if it is inside blockages
	for(i=0;i<pBlock->num;i++){
		if( inBlock(&pBlock->pool[i],(void*)&hor[0],H) ){
			printf("hor violating block %d\n",i);
			goto CHECK_PATH2;
		}
		if( inBlock(&pBlock->pool[i],(void*)&ver[0],V) )
		{
			printf("ver violating block %d\n",i);
			goto CHECK_PATH2;
		}
	}
	printf("pass 5\n");

	// path 1 succeed
	// ** judge the moving directions **
	switch(relative){
		case LL:
			*a2b = RIGHT; *b2a = DOWN; 
			break;
		case UL:
			*a2b = RIGHT; *b2a = UP;
			break;
		case LR:
			*a2b = LEFT;  *b2a = DOWN;
			break;
		case UR:
			*a2b = LEFT;  *b2a = UP;
			break;
	}
	g[idx_a][idx_b] = g[idx_b][idx_a] = MHT(a,b);
#ifdef DEBUG
	printf("UPDATE : (%lu,%lu) -> (%lu,%lu) : %lu\n",
			a.x,a.y,b.x,b.y,g[idx_a][idx_b]);
#endif
	goto SUCCEED;

CHECK_PATH2:
	// try path 2: ver first then hor
	// check if horizontal path cut by some block
	for(i=0;i<v_size;i++)
		if( intersect(hor[1],vlist[i]) )
			goto FAIL;

	// check if it passes some forbidden vertical segment
	for(i=0;i<hfbd_size;i++)
		if( hor_overlap(hor[1],hfbd[i]) )
			goto FAIL;

	// check if vertical path cut by some block
	for(j=0;j<h_size;j++)
		if( intersect(hlist[j],ver[1]) )
			goto FAIL;

	// check if it passes some forbidden horizontal segment
	for(j=0;j<vfbd_size;j++)
		if( ver_overlap(ver[1],vfbd[j]) )
			goto FAIL;

	for(i=0;i<pBlock->num;i++){
		if( inBlock(&pBlock->pool[i],(void*)&hor[1],H) || 
		    inBlock(&pBlock->pool[i],(void*)&ver[1],V) )
		    goto FAIL;
	}

	switch(relative){
		case LL:
			*a2b = UP;   *b2a = LEFT; 
			break;
		case LR:
			*a2b = UP;   *b2a = RIGHT;
			break;
		case UL:
			*a2b = DOWN;  *b2a = LEFT;
			break;
		case UR:
			*a2b = DOWN;  *b2a = RIGHT;
			break;
	}
	g[idx_a][idx_b] = g[idx_b][idx_a] = MHT(a,b);
#ifdef DEBUG
	printf("UPDATE : (%lu,%lu) -> (%lu,%lu) : %lu\n",
			a.x,a.y,b.x,b.y,g[idx_a][idx_b]);
#endif
SUCCEED:
	return TRUE;
FAIL:
	return FALSE;// neither succeed
}

// allocate for available position
// return the index of in g_node
// return -1 if can not found( the g_node is full )
int allocate_node(){
	// search from the starting position of sink node
	int i;
	//printf("static_num=%d,g_size=%d\n",static_num,g_size);
	for(i=static_num;i<g_size;i++)
		if( g_occupy[i] == FALSE ){
			g_occupy[i] = TRUE;
			return i;
		}
	return -1;
}

// add a point to the constructed graph
// return   : the index of the point in g_node
int insertpt(NODE pt){
	// search for an available position
	int pt_idx = allocate_node();
	if(pt_idx == -1)
		report_exit("Can not find available position for inserting!");
	// update the graph
#ifdef DEBUG
	printf("allocated:%d\n",pt_idx);
#endif
	int i;
	g_node[pt_idx] = pt;
	for(i=0;i<g_size;i++){
		if( i!=pt_idx && g_occupy[i] == TRUE &&
		    use_corner[i] == TRUE ){
			printf("reaching:%d -> %d\n",pt_idx,i);
			BOOL result = reach(pt,g_node[i],pt_idx,i);
			printf("RESULT = %d\n",result);
		}
	}
	++g_num;
	return pt_idx;
}

// remove a sink to the constructed graph
// REQUIRE  : the point's index can not be index of blockage corners
// pt_idx   : the index of the point in the graph(g_node)
// return   : TRUE if successfully removed
BOOL removept(int pt_idx){
	if( (pt_idx<static_num) || (g_occupy[pt_idx] != TRUE) )
		report_exit("removept: invalid");
	int i;
	g_occupy[pt_idx] = FALSE;
	// update the graph
	for(i=0;i<g_size;i++){
		g[pt_idx][i] = g[i][pt_idx] = INFINITE;
		dirs[pt_idx][i] = dirs[i][pt_idx] = INVALID;
	}
	--g_num;
	return TRUE;
}

// add a point to the constructed graph from blockage list `l'
// note that this function only compute's the 4*n point of blockage
// NOT regarding another point to be added
// REQUIRE : the constructed graph
// pt      : the point to add
// index   : this point's index
// NOTE    : this function is obsolete now
void addpt(NODE pt,int index,BLOCKAGE * list){
	int i,j;
	NODE corners[4];
	for(i=0;i<(list->num);i++){
		BOX * box = &list->pool[i];
		gen_node(box,corners);
		for(j=0;j<4;j++){
			int corner_idx = i*4+j;
			BOOL result;
			result = reach(pt,corners[j],index,corner_idx);
#ifdef DEBUG
			if( result ) printf("(%d,%d) -> (%d,%d) added into graph\n",
					pt.x,pt.y,
					corners[j].x,corners[j].y);
#endif
		}
	}
}

// delete a point from the constructed graph from blockage list 
// pt_idx  : the point's index in the graph
void delpt(int pt_idx,BLOCKAGE * list){
	int i;
	for(i=0;i<g_size;i++){
		g[pt_idx][i] = g[i][pt_idx] = INFINITE;
		dirs[pt_idx][i] = dirs[i][pt_idx] = INVALID;
		g[i][i] = 0;
	}
}

// add the given two point into the constructed graph
// note that if they are connected, there is a one-bend manhattan path
// s,t  : the two point to be added
// list : the blockages
// NOTE : this function is obsolete now
void add2pt(NODE s,NODE t,BLOCKAGE * list){
	int s_idx = 4*list->num;
	int t_idx = s_idx+1;
	// add two point first
	addpt(s,s_idx,list);
	addpt(t,t_idx,list);

	// update their connectivity
	reach(s,t,s_idx,t_idx);
}

// initialize floyd
void init_all_pair(){
	// initialize shortest_pair[0][i][j] to original graph
	int i,j;
	for(i=0;i<g_size;i++){
		for(j=0;j<g_size;j++){
			shortest_pair[0][i][j] = g[i][j];
			/*
			if(g[i][j] == INFINITE)
				printf("%10s","-");
			else
				printf("%10d",shortest_pair[0][i][j]);
				*/
			if( (i == j) || (g[i][j] == INFINITE) )
				backtrack_pair[0][i][j] = -1;
			else
				backtrack_pair[0][i][j] = i;
		}
		//printf("\n");
	}
}

// wrapper for floyd
// after called, can use the variable pairs and parents.
int all_pair_shortest(){
	int i = floyd();
	pairs=shortest_pair[i];
	parents=backtrack_pair[i];
	return i;
}

// use floyd to compute all pair's shortest path
// RETURN : an integer to indicate which array to use
int floyd(){
	init_all_pair();
	// p for current updating one, q for previous one
	int p=1,q=0; 
	int i,j,k;
	UINT *** d = shortest_pair;  // for simplication
	int *** bt = backtrack_pair; // NOT SURE whether there is problem
	for(k=0;k<g_size;k++){
		if( g_occupy[k] == FALSE ) continue;
		for(i=0;i<g_size;i++){
			// skip if not occupied by any node
			if( g_occupy[i] == FALSE ) continue;
			for(j=0;j<g_size;j++){
				if( g_occupy[j] == FALSE ) continue;
				UINT usep = d[q][i][k]+d[q][k][j];
				if( d[q][i][j] <= usep ){
					d[p][i][j] = d[q][i][j]; 
					bt[p][i][j] = bt[q][i][j];
				}
				else{
					d[p][i][j] = usep;
					bt[p][i][j] = bt[q][k][j];
				}
			}
		}
		// IMPORTANT: update switch
		q=p;
		p=(p+1)%2;
	}
	return q;
}

// initialize the shortest distance,via,mark vector from source point
void init_single_source(int src_idx){
	int i;
	for(i=0;i<g_size;i++){
		shortest[i] = INFINITE;
		via[i] = -1;
		mark[i] = FALSE;

		shortest[i] = g[src_idx][i];
		if( i!=src_idx && shortest[i] != INFINITE )
			via[i] = src_idx;
	}
	mark[src_idx] = TRUE;
}

// find the shortest path using dijkstra algorithm
// the result is stored shortest path vector and backtrack vector
// REQUIRE: the g has been constructed, with g_num node
// RETURN : 
void dijkstra(BLOCKAGE * list,int src_idx){
	init_single_source(src_idx);
	int i,j;
	for(i=0;i<g_num-1;i++){// iteratively add (g_num-1) point 
		UINT now_dist = INFINITE;
		int index = -1;
		for(j=0;j<g_size;j++){// search next point to add
			if( g_occupy[j] == TRUE && // skip empty position in g_node
			    mark[j]     != TRUE && 
			    shortest[j] <  now_dist ){
				now_dist = shortest[j];
				index = j;
			}
		}
		if( index == -1 ) // the graph is not connected?
			report_exit("Cannot find node to connect in dijkstra");

		// now add this point 
		mark[index] = TRUE;
		// update index
		for(j=0;j<g_size;j++){
			UINT new_dist = shortest[index] + g[index][j];
			if( g_occupy[j] == TRUE && 
			    mark[j]     != TRUE && 
			    shortest[j] >  new_dist ){// note:NO '=' here
				shortest[j] = new_dist;
				via[j] = index;
			}
		}
	}
}

// output the matrix of graph
void outputg(){
	int i,j;
	static char format[20];
	static char inf_string[20];
	sprintf(format,"%%%dd",width); // %10d like format
	sprintf(inf_string,"%%%ds",width); 
	for(i=0;i<g_size;i++){
		for(j=0;j<g_size;j++)
			if( g[i][j] == INFINITE )
				printf(inf_string,"-");
			else printf(format,g[i][j]);
		printf("\n");
	}
}

// output the manhattan directions of the graph
void output_dirs(){
	int i,j;
	static char format[20];
	sprintf(format,"%%%ds",width); 
	for(i=0;i<g_size;i++){
		for(j=0;j<g_size;j++){
			printf(format,dir_string[(int)dirs[i][j]]);
		}
		printf("\n");
	}
}

// free the space allocated for g, MUST be called at the end
void destroy_g(){
	int i;
	free(shortest);
	free(via);
	free(mark);
	for(i=0;i<g_size;i++) {
		free(g[i]);
		free(dirs[i]);
		free(fbdnode[i]);
	}
	free(g);
	free(dirs);
	free(fbdnode);

	int x,y;
	for(x=0;x<2;x++){
		for(y=0;y<g_size;y++){// free dimension 3 
			free(shortest_pair[x][y]);
			free(backtrack_pair[x][y]);
		}
		free(shortest_pair[x]);
		free(backtrack_pair[x]);
	}

	free(g_node);
	free(g_occupy);
	free(use_corner);

	g = NULL; dirs = NULL; via = NULL; shortest = NULL; mark = NULL;
	g_node = NULL; g_occupy = NULL;
}

// cancel the adjacent point
void cancel_pt(){
	//printf("%d\n%d\n",h_size,v_size);
}

// free the space allocated for segments, MUST be called at the end
void destroy_segments(){
	free(vlist);
	free(hlist);
	free(vfbd);
	free(hfbd);
	vfbd = NULL;
	hfbd = NULL;
	vlist = NULL; hlist = NULL;
}

// free the allocated memory of g, segments,dirs,
void free_all(){
	destroy_g();
	destroy_segments();
}

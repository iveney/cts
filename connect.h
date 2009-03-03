#ifndef __CONNECT_H__
#define __CONNECT_H__
#include "ds.h"

// ----------------------------------------------------------------//
// constants macros typedefs
//#define EPSILON 1E-9   // double type precision
// determines if two double value is ``the same''
//#define DOUBLE_EQ(a,b) ( (ABS((a)-(b))<EPSILON)?TRUE:FALSE )
//#define DOUBLE_GT(a,b) ( ((a)>((b)+EPSILON))?TRUE:FALSE )
//#define DOUBLE_GE(a,b) ( ((a)>((b)-EPSILON))?TRUE:FALSE )
//#define DOUBLE_LT(a,b) ( ((a)<((b)-EPSILON))?TRUE:FALSE )
//#define DOUBLE_LE(a,b) ( ((a)<((b)+EPSILON))?TRUE:FALSE )

typedef struct ver_seg{
	UINT x,y1,y2; 
//	int which;   // which blockage
//	DIRECTION d; // L or R
}VSEG;
typedef struct hor_seg{
	UINT y,x1,x2; 
//	int which;  // which blockage
//	DIRECTION d; // U or D
}HSEG;
enum SEGTYPE{H,V};

/* connect.c */
UINT **g;
int g_size;
int *mapping;
UINT *shortest;
int *via;
BOOL *mark;
DIRECTION **dirs;
NODE *g_node;
BOOL *use_corner;
BOOL *g_occupy;
NODE *sink_node;
int block_num;
int sink_num;
int static_num;
int g_num;
VSEG *vlist;
HSEG *hlist;
int v_size;
int h_size;
UINT **pairs;
int **parents;
UINT **shortest_pair[2];
int **backtrack_pair[2];
const static char *dir_string[];
static int width;
VSEG *vfbd;
HSEG *hfbd;
int vfbd_size;
int hfbd_size;
BLOCKAGE *pBlock;
BOOL **fbdnode;
void setvseg(VSEG *v, UINT xx, UINT yy1, UINT yy2);
void sethseg(HSEG *h, UINT yy, UINT xx1, UINT xx2);
void construct_g_all(BLOCKAGE *blocks, SINK *sink);
BOOL pt_in_rect(NODE *node, BOX *b);
int mark_forbidden(BLOCKAGE *block);
int constructg(BLOCKAGE *block);
void allocate_g(int size);
void init_g(void);
BOOL gen_block_node(BLOCKAGE *blockage);
void gen_node(BOX *b, NODE *node);
int sort_vseg(const void *v1, const void *v2);
int sort_hseg(const void *h1, const void *h2);
int gen_segments(BLOCKAGE *block);
void copy_sink(SINK *sink);
BOOL reach(NODE a, NODE b, int idx_a, int idx_b);
int allocate_node(void);
int insertpt(NODE pt,int id);
BOOL removept(int pt_idx);
void addpt(NODE pt, int index, BLOCKAGE *list);
void delpt(int pt_idx, BLOCKAGE *list);
void add2pt(NODE s, NODE t, BLOCKAGE *list);
void init_all_pair(void);
int all_pair_shortest(void);
int floyd(void);
void init_single_source(int src_idx);
void dijkstra(BLOCKAGE *list, int src_idx);
int sort_box_hor(const void *p1, const void *p2);
int sort_box_ver(const void *p1, const void *p2);
void preprocess_block(BLOCKAGE *pBlock);
void outputg(void);
void output_dirs(void);
void destroy_g(void);
void destroy_segments(void);
void free_all(void);
void update_dist(BLOCKAGE *,int src);
int zip_path(int);
#endif

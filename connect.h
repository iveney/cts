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

typedef struct ver_seg{UINT x,y1,y2; }VSEG;
typedef struct hor_seg{UINT y,x1,x2; }HSEG;

UINT **g;
int g_size;
UINT * shortest;
int * via;
BOOL * mark;
DIRECTION ** dirs;

void setvseg(VSEG *v, UINT xx, UINT yy1, UINT yy2);
void sethseg(HSEG *h, UINT yy, UINT xx1, UINT xx2);
void allocate_g(int n);
void init_g(void);
void outputg(void);
void output_dirs(void);
BOOL intersect(HSEG hor, VSEG ver);
BOOL reach(NODE a, NODE b, int idx_a, int idx_b);
void gen_node(BOX *b, NODE *node);
int gen_segments(BLOCKAGE *list);
int constructg(BLOCKAGE *list);
void addpt(NODE pt, int index, BLOCKAGE *list);
void add2pt(NODE s, NODE t, BLOCKAGE *list);
void init_single_source(int src_idx);
void delpt(int pt_idx, BLOCKAGE *list);
void floyd(BLOCKAGE *list);
void dijkstra(BLOCKAGE *list, int src_idx);
void destroy_g(void);
void destroy_segments(void);
void free_all(void);

#endif

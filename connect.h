#ifndef __CONNECT_H__
#define __CONNECT_H__
#include "ds.h"

// ----------------------------------------------------------------//
// constants macros typedefs
enum BOOL_VAL{FALSE,TRUE};
enum POS{LL,LR,UR,UL}; // low-left, low-right, up-right, up-left
enum DIRS{INVALID,LEFT,RIGHT,UP,DOWN};
#define ABS(a) ((a)<0.0?(-(a)):(a))
#define MHT(s,t) (ABS((s.x)-(t.x))+ABS((s.y)-(t.y)))
#define MAX(a,b) ((a)>(b)?(a):(b))
#define MIN(a,b) ((a)>(b)?(b):(a))
//#define EPSILON 1E-9   // double type precision
#define INFINITE 1E9       // big number to denote infinite
#define L 10.0
// determines if two double value is ``the same''
//#define DOUBLE_EQ(a,b) ( (ABS((a)-(b))<EPSILON)?TRUE:FALSE )
//#define DOUBLE_GT(a,b) ( ((a)>((b)+EPSILON))?TRUE:FALSE )
//#define DOUBLE_GE(a,b) ( ((a)>((b)-EPSILON))?TRUE:FALSE )
//#define DOUBLE_LT(a,b) ( ((a)<((b)-EPSILON))?TRUE:FALSE )
//#define DOUBLE_LE(a,b) ( ((a)<((b)+EPSILON))?TRUE:FALSE )

typedef unsigned long UINT;
typedef char BOOL;
typedef struct ver_seg{UINT x,y1,y2; }VSEG;
typedef struct hor_seg{UINT y,x1,x2; }HSEG;
typedef char DIRECTION;

extern UINT **g;
extern int g_size;
void setvseg(VSEG *v, UINT xx, UINT yy1, UINT yy2);
void sethseg(HSEG *h, UINT yy, UINT xx1, UINT xx2);
void clearg(void);
void initg(int n);
void outputg(void);
void output_dirs(void);
BOOL intersect(HSEG hor, VSEG ver);
BOOL reach(NODE a, NODE b, int idx_a, int idx_b);
void gen_node(BOX *b, NODE *node);
int gen_segments(BLOCKAGE *list);
int constructg(BLOCKAGE *list);
void addpt(NODE pt, int index, BLOCKAGE *list);
void add2pt(NODE s, NODE t, BLOCKAGE *list);
void init_source(int src_idx);
void dijkstra(BLOCKAGE *list, int src_idx);
void destroy_g(void);
void destroy_segments(void);
void free_all(void);
#endif

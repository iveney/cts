#ifndef BASIC_STRUCTURE_H
#define BASIC_STRUCTURE_H

#define ABS(a) ((a)<0.0?(-(a)):(a))
#define MHT(s,t) (ABS((s.x)-(t.x)) + ABS((s.y)-(t.y)))
#define MAX(a,b) ((a)>(b)?(a):(b))
#define MIN(a,b) ((a)>(b)?(b):(a))
#define Manhattan(x1,y1,x2,y2) ((( (x1)-(x2) ) >= 0? ((x1)-(x2)) : ((x2)-(x1)))  +  (((y1)-(y2) ) >= 0? ((y1)-(y2)) : ((y2)-(y1))))

#define INFINITE 2<<29 // big number to denote infinite
#define _L_ INFINITE
#define SMALL_BUF_SIZE 100
#define LARGE_BUF_SIZE 1000
#define ELMORE 1
#define BIG_NUM 20
#define EPSILON 1E-9   // double type precision
#define OFFSET 100
#define LAMBADA_w1 1400001
#define LAMBADA_w2 700001
#define LARGESTNODE 9999
//#define LAMBADA 200000.00
//
typedef unsigned long UINT;
typedef char BOOL;
typedef char DIRECTION;

enum BOOL_VAL{FALSE,TRUE};
enum POS{LL,LR,UR,UL}; // low-left, low-right, up-right, up-left
enum DIRS{INVALID,LEFT,RIGHT,UP,DOWN};
enum LINE_STYLE{SOLID,Dashed};
enum COLORS{BLACK,BLUE,GREEN,CYAN,RED,WHITE};


typedef struct inode{
	int x ;
	int y ;
	DIRECTION dir_to;
	DIRECTION dir_from;
}NODE ;

typedef struct isnode{
	int index ;
	int x;
	int y;
	int lc;
}SNODE;

typedef struct ibox{
	NODE ll;
	NODE ur;
} BOX;

typedef struct isource{
	int name ;
	NODE location;
	int bufname ;

}SOURCE;

typedef struct iwire{
	int wiretype ;
	double r;
	double c;
} WIRE;

typedef struct ibuffer{
	int buf_id;
	char *spice_subckt ;
	int inverted;
	double icap ;
	double ocap ;
	double ores ;
} BUFFER;

typedef struct isink{
	int num;
	SNODE *pool ;

}SINK ;

typedef struct iwirelib{
	int num;
	WIRE *lib;

}WIRELIB ;

typedef struct ibuflib{
	int num;
	BUFFER *lib;
}BUFLIB ;

typedef struct ivddlib{
	int num;
	double *lib;
}VDDLIB ;



typedef struct iblockage{
	int num;
	BOX *pool;
} BLOCKAGE ;


typedef struct dme_node{
	int left;
	int right;
	int upper;
	int lower;
	int x1;
	int y1;
	int x2;
	int y2;
	double weight;
	double capacitance;
	double resistance;
	double left_length;
	double right_length;
	double to_sink_delay;
	double downstream_length;
	double first_buf_fraction;
	int delta_length_buf;
	int select_x;
	int select_y;
	int sx;
	int sy;
	int visited;
	BOOL is_select;
	int blockage_node;
	int sink_index;
	int node_id;
	int buf_num;
	int buf_unit;
	int level;
	int tree_level;
	int duplicate_first_buf;
	int downstream_buf_num;
	int downstream_total_buf_num;
	int upstream_buf_num;
	int reduntant;
	int detour;
	int wire_type;
	double factor;
	struct dme_node * pleft;
	DIRECTION left_direction;
	struct dme_node * pright;
	DIRECTION right_direction;
}DME_NODE;




typedef struct buf_node{
	double x;
	double y;
	int buf_id;
	int wire_type;
	int buf_type;
	int units;
//	int buf_lev;
	struct buf_node * next ;
}BUF_NODE ;

typedef struct dme_tree_node{

	struct dme_tree_node * ls;
	DIRECTION left_direction;
	struct dme_tree_node * rs;
	DIRECTION right_direction;
	struct dme_tree_node * fa ;
	int 	   buf_num ;
//	int 	   delta_length_buf ;
	double 	   factor ;
	double 	   delay;
        int        is_fake ;
	int	   duplicate_first_buf;
	int  	   altitude;
	int  	   x;
	int  	   y;
	int	   left;
	int	   right;
	int 	   node_id;
	int 	   is_sink ;
	int 	   sink_index ;
	int	   is_blk ;
	int	   reduntant ;
    int        detour ;
	int    wire_type;
	double first_buf_fraction;
	double	   capacitance;
}DME_TREE_NODE;

typedef struct tem_node{
	int altitude ;
	int x;
	int y;
	int	    prev ;
	int	    next ;
	int 	   isit ;
} BUF_POS;

typedef struct cusink{
	int node_id;
	int sink_index;
	double latency[4];
} CUSINK;

#endif

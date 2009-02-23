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
#define OFFSET 100
#define LAMBADA 1250000.00

//#define LAMBADA 200000.00
//
typedef unsigned long UINT;
typedef char BOOL;
typedef char DIRECTION;

enum LINE_STYLE{SOLID,Dashed};
enum COLORS{BLACK,BLUE,GREEN,CYAN,RED,WHITE};
enum BOOL_VAL{FALSE,TRUE};
enum POS{LL,LR,UR,UL}; // low-left, low-right, up-right, up-left
enum DIRS{INVALID,LEFT,RIGHT,UP,DOWN};

typedef struct inode{
	int x ;
	int y ; 

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
	int icap ;
	int ocap ; 
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
	double left;
	double right;
	double upper;
	double lower;
	double x1;
	double y1; 
	double x2;
	double y2;
	double weight;
	double capacitance;
	double to_sink_delay;
	double select_x;
	double select_y;
	int blockage_detour;
	int visited;
	int blockage_node;
	int sink_index;
	int node_id;
	struct dme_node * pleft;
	struct dme_node * pright;
}DME_NODE;




typedef struct buf_node{
	double x;
	double y;
	int buf_id;
//	int buf_lev;
	struct buf_node * next ; 
}BUF_NODE ;

typedef struct dme_tree_node{
	
	struct dme_tree_node * ls;
	struct dme_tree_node * rs;
	double  altitude;
	double  x; 
	double  y; 
	int	   left;
	int	   right; 
	int 	   node_id; 
	int 	   is_sink ; 
	int 	   sink_index ; 
	int	   is_blk ;
}DME_TREE_NODE;

typedef struct tem_node{
	double altitude ; 
	double x; 
	double y;
	int	    prev ; 
	int	    next ; 
	int 	   isit ; 
} BUF_POS;



#endif

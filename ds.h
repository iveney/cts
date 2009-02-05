#ifndef BASIC_STRUCTURE_H
#define BASIC_STRUCTURE_H

#define SMALL_BUF_SIZE 100
#define LARGE_BUF_SIZE 1000
#define MAX(a,b) ((a)>(b)?(a):(b))
#define MIN(a,b) ((a)>(b)?(b):(a))


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
	double r	; 
	double c	; 
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

#endif

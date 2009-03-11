#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<string.h>
#include "ds.h"
#include "bufplace.h"
//#include "main.h"

// okay 1, fault 0

extern BOX frame	;
extern SOURCE source  ;
extern SINK sink	;
extern WIRELIB wirelib;
extern BUFLIB	buflib ;
extern VDDLIB	vddlib ;
extern int SlewLimit;
extern int CapLimit ;
extern BLOCKAGE blockage;

extern double cap_left;
extern int num_node ;
extern int num_buffer;
extern int num_wire ;
extern int num_sinknode ;
extern int num_total_nodes ;
extern int highway_extra_buf ;
extern int * sink_alt_array;
extern int mid_sink_num;
extern int total_buf_num;

int InputFile(FILE *ifp){

char buf[SMALL_BUF_SIZE] ;
char prefix1[SMALL_BUF_SIZE];
char prefix2[SMALL_BUF_SIZE];
int	 i,j ;
int  x1,y1,x2,y2;
double d1 = 0.0;
double d2 = 0.0;

	if (ifp == NULL){
		printf("No such input file: IO ERROR 1\n");
		return 0 ;
	}

// frame
	fgets(buf,SMALL_BUF_SIZE,ifp);
	sscanf(buf,"%d %d %d %d",&x1,&y1,&x2,&y2);

	frame.ll.x = x1 ;
	frame.ll.y = y1 ;
	frame.ur.x = x2 ;
	frame.ur.y = y2 ;
	fgets(buf,SMALL_BUF_SIZE,ifp);

//	source
	sscanf(buf,"%s %d %d %d %d",prefix1,&x1,&y1,&x2,&y2);
	if ( strcmp(prefix1,"source")){
		printf("No source node: IO ERROR 2\n");
		return 0;
	}
	source.name = x1 ;
	source.location.x = y1 ;
	source.location.y = x2 ;
	source.bufname = y2 ;

// sink
	fgets(buf,SMALL_BUF_SIZE,ifp);
	sscanf(buf,"%s %s %d",prefix1,prefix2,&x1);


	if ( strcmp(prefix1,"num")){
		printf("Wrong num of sink: IO ERROR 3\n");
		return 0;
	}

	if ( strcmp(prefix2,"sink")){
		printf("Wrong num of sink: IO ERROR 4\n");
		return 0;
	}
	sink.num = x1 ;
	sink.pool = (SNODE *) malloc(sizeof(SNODE) * sink.num) ;
	for(i = 0; i< sink.num; i++){
		fgets(buf,SMALL_BUF_SIZE,ifp);
		sscanf(buf,"%d %d %d %d", &x1,&y1,&x2,&y2);
		sink.pool[i].index = x1 ;
		sink.pool[i].x	= y1 ;
		sink.pool[i].y	= x2 ;
		sink.pool[i].lc = y2 ;
	}

	fgets(buf,SMALL_BUF_SIZE,ifp);
	sscanf(buf,"%s %s %d",prefix1,prefix2, &x1);


//	wirelib
	if ( strcmp(prefix1,"num")){
		printf("Wrong num of wirelib: IO ERROR 5\n");
		return 0;
	}
	if ( strcmp(prefix2,"wirelib")){
		printf("Wrong num of wirelib: IO ERROR 6\n");
		return 0;
	}
	if ( x1 != 2 ){
		printf("Wrong num of wirelib: IO ERROR 7\n");
		return 0;
	}
	wirelib.num = x1 ;
	wirelib.lib = (WIRE *)malloc(sizeof(WIRE) * wirelib.num) ;
	for ( i = 0 ; i < wirelib.num ; i++){
		fgets(buf,SMALL_BUF_SIZE,ifp);
		sscanf(buf,"%d %lf %lf",&x1,&d1,&d2);
		wirelib.lib[i].wiretype = x1;
		wirelib.lib[i].r	= d1;
		wirelib.lib[i].c	= d2;

	}

//	buflib
	fgets(buf,SMALL_BUF_SIZE,ifp);
	sscanf(buf,"%s %s %d",prefix1,prefix2,&x1);
	if ( strcmp(prefix1,"num")){
		printf("Wrong num of buflib: IO ERROR 8\n");
		return 0;
	}
	if ( strcmp(prefix2,"buflib")){
		printf("Wrong num of buflib: IO ERROR 9\n");
		return 0;
	}

	buflib.num = x1;
	buflib.lib = (BUFFER *) malloc (sizeof(BUFFER) * buflib.num);
	for(i = 0 ; i < buflib.num; i++){
		fgets(buf,SMALL_BUF_SIZE,ifp);
		sscanf(buf,"%d %s %d %lf %lf %lf",&x1,prefix1,&y1,&(buflib.lib[i].icap),&(buflib.lib[i].ocap),&(buflib.lib[i].ores));
		buflib.lib[i].buf_id = x1 ;
		buflib.lib[i].spice_subckt = (char *)malloc(sizeof(char)*SMALL_BUF_SIZE);
		strcpy(buflib.lib[i].spice_subckt, prefix1);
		buflib.lib[i].inverted = y1;
/*		buflib.lib[i].icap = x2;
		buflib.lib[i].ocap = y2;*/
		;

	}

// vdd
	fgets(buf,SMALL_BUF_SIZE,ifp);
	sscanf(buf,"%s %s %d %lf",&(prefix1[0]),&(prefix2[0]),&x1,&d2);
	d1 = (double)x1 ;
	if ( strcmp(prefix1,"simulation")){
		printf("Wrong simulation vdd: IO ERROR 10\n");
		return 0;
	}
	if ( strcmp(prefix2,"vdd")){
		printf("Wrong simulation vdd: IO ERROR 11\n");
		return 0;
	}


	vddlib.num = 2 ;
	vddlib.lib = (double *) malloc(sizeof(double)*vddlib.num);
	vddlib.lib[0] = d1 ;
	vddlib.lib[1] = d2 ;

	fgets(buf,SMALL_BUF_SIZE,ifp);
	sscanf(buf,"%s %s %d",&(prefix1[0]),&(prefix2[0]),&x1);
	SlewLimit = x1;
	fgets(buf,SMALL_BUF_SIZE,ifp);
	sscanf(buf,"%s %s %d",&(prefix1[0]),&(prefix2[0]),&x1);
	CapLimit = x1;

// blockage
	fgets(buf,SMALL_BUF_SIZE,ifp);
	sscanf(buf,"%s %s %d",prefix1,prefix2,&x1);
	if ( strcmp(prefix1,"num")){
		printf("Wrong num of blockage: IO ERROR 12\n");
		return 0;
	}
	if ( strcmp(prefix2,"blockage")){
		printf("Wrong num of blockage: IO ERROR 13\n");
		return 0;
	}

	blockage.num = x1;
	blockage.pool = (BOX *) malloc (sizeof(BOX) * blockage.num);
	for(i = 0 ; i < blockage.num; i++){
		fgets(buf,SMALL_BUF_SIZE,ifp);
		sscanf(buf,"%d %d %d %d",&x1,&y1,&x2,&y2);
		blockage.pool[i].ll.x = x1 ;
		blockage.pool[i].ll.y = y1;
		blockage.pool[i].ur.x = x2;
		blockage.pool[i].ur.y = y2;

	}

	return 1;

}


int check_input(){
	int i ;
	printf("check input\n");
	printf("%d,%d,%d,%d\n",frame.ll.x,frame.ll.y,frame.ur.x,frame.ur.y);
	printf("source %d %d %d %d\n", source.name, source.location.x,source.location.y,source.bufname);
	printf("num sink %d\n",sink.num);
	for(i=0;i<sink.num; i++){
		printf("%d %d %d %d\n", sink.pool[i].index,sink.pool[i].x, sink.pool[i].y, sink.pool[i].lc);

	}

	printf("num wirelib %d\n", wirelib.num);
	for(i=0;i<wirelib.num;i++)
		printf("%d %f %f\n",wirelib.lib[i].wiretype,wirelib.lib[i].r,wirelib.lib[i].c);

	printf("num buflib %d\n", buflib.num);
	for(i=0;i<buflib.num;i++)
		printf("%d %s %d %f %f %f\n",buflib.lib[i].buf_id,buflib.lib[i].spice_subckt,buflib.lib[i].inverted, buflib.lib[i].icap,buflib.lib[i].ocap,buflib.lib[i].ores);

	printf("simulation vdd %f %f\n",vddlib.lib[0],vddlib.lib[1]);

	printf("limit slew %d\n", SlewLimit);

	printf("limit cap %d\n", CapLimit);

	printf("num blockage %d\n", blockage.num);

	for(i=0;i<blockage.num;i++){
			printf("%d %d %d %d\n",blockage.pool[i].ll.x,blockage.pool[i].ll.y,blockage.pool[i].ur.x,blockage.pool[i].ur.y);
	}
	return 0 ;

}

int constradict(int x,int y, BOX * blk){
int x1 = blk->ll.x;
int y1 = blk->ll.y;
int x2 = blk->ur.x;
int y2 = blk->ur.y;

	if( x>x1 && x<x2 && y > y1 && y < y2){
		printf("error, blk is in the blockage\n");
		exit(0);
	}

	else if ( x < x1 || x > x2 || y < y1 || y > y2)
		return 0;
	else {
		if ( x == x1 && y >= y1 && y <= y2){
			if (frame.ll.x < x1)
				return 3;
		}
		else if ( y == y1 && x >= x1 && x <= x2){
			if (frame.ll.y < y1)
				return 2;
		}
		else if ( x == x2 && y >= y1 && y <= y2){
			if (frame.ur.x > x2)
				return 4;
		}
		else if ( y == y2 && x >= x1 && x <= x2){
			if (frame.ur.y > y2)
				return 1;
		}
		else {
			printf(" wrong type of node\n");
			exit(0);
		}
	}

	printf(" another wrong type of node\n");
	exit(0);
}

void check_buf_node(BUF_NODE *bt,BLOCKAGE *block,int num,int *direct){
int x = (int) bt->x;
int y = (int) bt->y;
int i;
	for ( i = 0 ; i < num ; i++){
		if(((*direct)=constradict(x,y,(block->pool + i))) > 0 ){
			return;
		}

	}
}
void check_point_node(DME_TREE_NODE *bt,BLOCKAGE *block,int num,int *direct){
int x = (int) bt->x;
int y = (int) bt->y;
int i;
	for ( i = 0 ; i < num ; i++){
		if(((*direct)=constradict(x,y,(block->pool + i))) > 0 ){
			return;
		}

	}



}
void adjust_buf_node(BUF_NODE * bt, BLOCKAGE * block,int num){
int direct = -1;
	check_buf_node(bt,block,num,&direct);
	switch(direct){
		case 1:
				(bt->y) += 0.1;
				break;
		case 2:
				(bt->y) -= 0.1;
				break;
		case 3:
				(bt->x) -= 0.1;
				break;
		case 4:
				(bt->x) += 0.1;
				break;
		default:
				break;
	}
	return;

}

void adjust_point_node(DME_TREE_NODE * tr, BLOCKAGE * block,int num){
int direct = -1;
	check_point_node(tr,block,num,&direct);
	switch(direct){
		case 1:
				(tr->y) += 1;
				break;
		case 2:
				(tr->y) -= 1;
				break;
		case 3:
				(tr->x) -= 1;
				break;
		case 4:
				(tr->x) += 1;
				break;
		default:
				break;
	}
	return;

}

void adjust_buf(BUF_NODE ** OBUF, BLOCKAGE * block){
int num = block->num;
BUF_NODE * bf;
int i;
	if ( num <= 0 )
		return;
	for(i=0;i<num_total_nodes;i++){
		bf = (*(OBUF+i))->next;
		while(bf!=NULL){
			adjust_buf_node(bf,block,num);
			bf = bf->next;
		}

	}
}

void adjust_point(DME_TREE_NODE ** Tmap, BLOCKAGE * block){
int num = block->num;
BUF_NODE * bf;
int i;
	if ( num <= 0 )
		return;

	for(i=0;i<num_total_nodes;i++){
			adjust_point_node((*(Tmap+i)),block,num);
	}

}


void output_file(FILE *fp, BUF_NODE ** OBUF, DME_TREE_NODE * OT, DME_TREE_NODE ** OTmap,int label){
	if ( fp ==  NULL) {
		printf(" error input file name \n");
		return ;
	}
	if (label == 1)
		print_fig_3(OT,OTmap);
	fprintf(fp,"sourcenode 0 0\n");
	fprintf(fp,"num node %d\n", num_node);
	Out_put_files_coordinate(fp,OBUF,OT);
	fprintf(fp,"num sinknode %d\n", num_sinknode);
	Out_put_files_sink(fp, OT) ;
	fprintf(fp,"num wire %d\n", num_wire);
	Out_put_files_connect(fp,OBUF, OT);
	//fprintf(fp,"num buffer %d\n",num_buffer*2+highway_extra_buf*HIGHWAY);
	fprintf(fp,"num buffer %d\n",total_buf_num);
	Out_put_files_buf(fp, num_total_nodes, OBUF, OTmap);

}

void get_init_delay(DME_NODE *L){
	int i;
	int x;
	double y;
	char buf[SMALL_BUF_SIZE] ;
	FILE * ifp = fopen( "initDelay.txt", "r") ;
	double * tmp = (double *) malloc (sizeof(double)*sink.num);

	for(i=0;i<sink.num*4; i++){
		fgets(buf,SMALL_BUF_SIZE,ifp);
		sscanf(buf,"%d %lf",&x,&y);
//		printf("%d %f\n",x,y);
		tmp[x-1] += y;
	}
	for(i=0;i<sink.num; i++){
		tmp[i] /= 4.0f;
		L[i].to_sink_delay = tmp[i];
//		printf("%d %f\n",i,L[i].to_sink_delay);
	}
	fclose(ifp);

}

void fprint_sink(FILE *fp, DME_TREE_NODE * Troot){
	if ( Troot == NULL)
		return ;
	if ( Troot->sink_index != -1)
        fprintf(fp,"%d %d %d %f\n",Troot->sink_index,Troot->x,Troot->y,Troot->capacitance);
	fprint_sink(fp,Troot->ls);
	fprint_sink(fp,Troot->rs);
}

void gen_inputfile(FILE * ifp,DME_TREE_NODE * Troot, DME_TREE_NODE ** map,int flag){
int i;
int x1,y1,x2,y2;
	x1 = frame.ll.x ; 	y1 = frame.ll.y;
	x2 = frame.ur.x ; 	y2 = frame.ur.y;
        fprintf(ifp,"%d %d %d %d\n", x1,y1,x2,y2);
        fprintf(ifp,"source %d %d %d %d\n",source.name,source.location.x,source.location.y,source.bufname);
        if (flag == 0)
			fprintf(ifp,"num sink %d\n",mid_sink_num);
		else if (flag == 1)
			fprintf(ifp,"num sink %d\n",num_sinknode);
		fprint_sink(ifp,Troot);
        fprintf(ifp,"num wirelib %d\n",wirelib.num);
        for(i = 0 ; i < wirelib.num; i++)
                fprintf(ifp,"%d %f %f\n",wirelib.lib[i].wiretype,wirelib.lib[i].r,wirelib.lib[i].c);
        fprintf(ifp,"num buflib %d\n",buflib.num);
 //       for(i=0; i< buflib.num; i++)
                 fprintf(ifp,"%d %s %d %f %f %f\n",buflib.lib[0].buf_id,buflib.lib[0].spice_subckt,buflib.lib[0].inverted,35.0,buflib.lib[0].ocap,buflib.lib[0].ores);
                fprintf(ifp,"%d %s %d %f %f %f\n",buflib.lib[1].buf_id,buflib.lib[1].spice_subckt,buflib.lib[1].inverted,4.2,buflib.lib[1].ocap,buflib.lib[1].ores);

        fprintf(ifp,"simulation vdd %.1f %.1f\n",vddlib.lib[0],vddlib.lib[1]);
        fprintf(ifp,"limit slew %d\n",SlewLimit);
	if ( flag == 0)	
        	fprintf(ifp,"limit cap %d\n",CapLimit);
	else if(flag == 1)
        	fprintf(ifp,"limit cap %d\n",(int)cap_left);
		
        fprintf(ifp,"num blockage %d\n", 0);

}

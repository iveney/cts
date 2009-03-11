#include <stdlib.h>
#include "ds.h"
//#include "main.h"
#include "connect.h"
#include "bufplace.h"


#define Lsmall 10.0
#define FIFO_SIZE 100
#define STACK_SIZE 100
#define BLACK 0
#define BLUE	  1
#define GREEN 2
#define CYAN	  3
#define RED	  4
#define WHITE  7
#define Dashed 1
#define SOLID   0
#define MAX_DOUBLE 999999999

#define Absdist(x1,x2) ((x1)-(x2))>=0?((x1)-(x2)):((x2)-(x1))

extern SINK sink ;
extern int HIGHWAY;
extern BLOCKAGE blockage;
extern DME_NODE *source_node;
extern BOX  frame ;
extern BOX  frame ;
extern WIRELIB wirelib;
extern BUFLIB	buflib ;
extern VDDLIB	vddlib ;
extern int num_node ;
extern int num_buffer;
extern int num_wire ;
extern int num_sinknode ;
extern int num_total_nodes ;
extern int total_buf_num;
extern int LAMBADA3;


int WIRETYPE = 0;
int BUFTYPE = 0;
int BUF_UNIT = 2;
int realson = -1;

static int buf_i   ;
static int buf_a = 0;




BUF_POS * FIFO1 ;
BUF_POS * FIFO2 ;

static int Findex1 = 0 ;
static int Findex2 = 0 ;
BUF_NODE ** bufnode;

void add_DME_node(DME_NODE *l , DME_TREE_NODE ** Troot, int x, int y, DME_TREE_NODE *father){
int i, j ,n ;

	if ( l == NULL) return ;

	if ( Troot == NULL){
		Troot = (DME_TREE_NODE **) malloc ( sizeof (DME_TREE_NODE *));
		if ( Troot == NULL) {
			printf(" mallock Error 1 \n");
			exit(1);
		}
	}


	if ( (*Troot) == NULL ){
		(*Troot) = (DME_TREE_NODE *) malloc ( sizeof(DME_TREE_NODE)) ;
		if ( (*Troot) == NULL){
			printf(" mallock Error 2 \n");
			exit(1);
		}
		(*Troot) ->capacitance = l -> capacitance;
		(*Troot) ->altitude = Manhattan(l ->select_x, l ->select_y,x,y)  ;
		(*Troot) -> x = l->select_x ;
		(*Troot) -> y = l->select_y;
		(*Troot) ->detour = l ->detour ;
		(*Troot) -> delay = l->to_sink_delay;
		(*Troot) -> left_direction = l->left_direction ;
		(*Troot) -> right_direction = l ->right_direction;
		(*Troot) -> duplicate_first_buf = l ->duplicate_first_buf ;
		(*Troot) -> is_sink = 0 ;
		(*Troot) -> buf_num = l ->buf_num ;
		(*Troot) ->sink_index = l ->sink_index ;
		(*Troot) ->is_blk = l ->blockage_node ;
	//	(*Troot) ->delta_length_buf = l ->delta_length_buf ;
		(*Troot) ->first_buf_fraction = l -> first_buf_fraction ;
		(*Troot) ->factor = l ->factor;
		(*Troot) ->reduntant = l->reduntant ;
		(*Troot) ->left = (*Troot)->right =  0;
		(*Troot) ->ls = (*Troot) -> rs = NULL;
		(*Troot) ->fa = father ;
		(*Troot) ->is_fake = 0 ;
		(*Troot) ->wire_type = l->wire_type;
	}

	add_DME_node(l -> pleft, &((*Troot)->ls),l->select_x,l->select_y,(*Troot)) ;
	add_DME_node(l -> pright, &((*Troot)->rs),l->select_x,l->select_y,(*Troot)) ;

}

void relative_dist(DME_TREE_NODE * Troot, int x, int y ){
	if (Troot == NULL)
		return ;
	Troot ->altitude = Manhattan(Troot->x, Troot->y,x,y)  ;
	relative_dist(Troot->ls, Troot->x,Troot->y) ;
	relative_dist(Troot->rs,Troot->x,Troot->y) ;

}

void construct_DME_tree(DME_NODE * l, int lnum, DME_TREE_NODE **Troot){
	if ( (*Troot) == NULL ){
		(*Troot) = (DME_TREE_NODE *) malloc ( sizeof(DME_TREE_NODE)) ;
		if ( (*Troot) == NULL){
			printf(" mallock Error 1 \n");
			exit(1);
		}
	}


	(*Troot) -> capacitance = l -> capacitance ;
	(*Troot) -> delay = l->to_sink_delay;
	(*Troot) -> left_direction = l->left_direction;
	(*Troot) -> right_direction = l->right_direction ;
	(*Troot) -> duplicate_first_buf = l ->duplicate_first_buf ;
	(*Troot) -> is_sink = l->sink_index;
	(*Troot) ->sink_index = l ->sink_index ;
	(*Troot) ->is_blk = l ->blockage_node ;
//	(*Troot) ->delta_length_buf = l->delta_length_buf ;
	(*Troot) -> first_buf_fraction = l ->first_buf_fraction ;
	(*Troot) ->detour  = 0 ;
	//(*Troot) ->factor = source_node ->factor;
	(*Troot) ->ls = (*Troot) -> rs = NULL;
	(*Troot) ->fa = NULL;
	(*Troot) ->buf_num = l->buf_num;
	(*Troot) ->altitude = 0;
	(*Troot) -> x = 0;
	(*Troot) -> y = 0;
	(*Troot) ->reduntant = 0 ;
	(*Troot) ->is_fake = 0 ;
	(*Troot) ->wire_type = l->wire_type;
	add_DME_node(l->pleft,& ((*Troot)->ls), 0, 0,(*Troot));

}


int  settle_DME_tree(DME_TREE_NODE * Troot , int  father) {
	if ( Troot == NULL)
		return ;

	Troot -> altitude += father;
	father  = Troot->altitude ;

	if(Troot -> ls != NULL)
		settle_DME_tree(Troot->ls, Troot->altitude);

	if(Troot -> rs != NULL)
		settle_DME_tree(Troot->rs, Troot->altitude);
}

double max_sink_alt (int * sink_alt_array, int lnum){
int i ;
int max_value = 0;
	for ( i = 0 ; i < lnum ; i++)
		if ( sink_alt_array[i] > max_value ) {
			max_value = sink_alt_array[i] ;
		}

	return (double)max_value ;


}


extern int sink_array_num ;

void construct_SINK_array( DME_TREE_NODE * Troot , CUSINK *sink_alt_array,int quta){
	if (Troot == NULL)
		return ;
	construct_SINK_array(Troot->ls, sink_alt_array,quta);
	construct_SINK_array(Troot->rs, sink_alt_array,quta);
	if ( Troot -> ls == NULL) {
		if ( sink_array_num >  quta -1 ) {
			printf(" out of range error 1 !\n");
			exit(1);
		}
		(sink_alt_array+sink_array_num)->node_id = Troot->node_id ;
		(sink_alt_array+sink_array_num)->sink_index = Troot->sink_index ;
		(sink_alt_array+sink_array_num)->latency[0] = 0.0;
		(sink_alt_array+sink_array_num)->latency[1] = 0.0;
		(sink_alt_array+sink_array_num)->latency[2] = 0.0;
		(sink_alt_array+sink_array_num)->latency[3] = 0.0;
		Troot ->left = sink_array_num ;
		Troot ->right= sink_array_num;
		sink_array_num ++ ;

	}
	else if ( Troot -> rs == NULL){
		Troot ->left = Troot ->ls ->left ;
		Troot ->right = Troot->ls->right ;
		}
	else {
		Troot ->left = Troot->ls->left ;
		Troot ->right = Troot ->rs->right ;

	}

}

int allnode = 0;

static int icommap = 0  ;



void construct_DME_map(DME_TREE_NODE * Troot, DME_TREE_NODE ** Tmap){

	if ( Troot == NULL)
		return ;
	Troot -> node_id = icommap ;
	//Troot -> wire_type = WIRETYPE;
	(*(Tmap+icommap)) = Troot ;
	icommap ++ ;
	if ( allnode < icommap ) allnode = icommap ;
	construct_DME_map(Troot ->ls, Tmap );
	construct_DME_map(Troot ->rs , Tmap);
}

static int quta=0;

int count_DME_tree(DME_TREE_NODE * Troot){
	if ( Troot == NULL)
		return ;
	if ( Troot->sink_index != -1)
		quta ++;
	count_DME_tree(Troot->ls);
	count_DME_tree(Troot->rs);
	return quta ;
}

void wopao(){
	return ;
}

static int blk_index1 = 0;
int check_DME_tree(DME_TREE_NODE * Troot){

	if ( Troot == NULL)
		return ;
	if ( Troot->ls == NULL)
		Troot->is_sink = 1 ;
	if ( Troot->is_blk == 1)
		blk_index1++ ;
	if ( Troot->is_blk == 1 && Troot->sink_index != -1){
		printf(" wo bei pian le \n");
		wopao();
//		exit(0);
	}
//	if (Troot->reduntant > 0){
//		printf(" node is %d\n",Troot->node_id);
//		printf(" buf num is %d\n",Troot->buf_num);
//		sleep(1);
//	}
//	printf(" %d\t", i++);
//	if ( Troot->fa != NULL)
//	printf(" fa: %d\n ", Troot->fa->node_id);

//	printf("node is  %d \t  wiretype is %d\n", Troot->node_id, Troot->wire_type);
//	printf(" %d %d \t", Troot->x , Troot->y) ;
//	printf(" blk is %d\n", Troot->is_blk );
//	if ( Troot->fa != NULL)
//		printf(" father is %d\n", Troot->fa->node_id);
//	printf(" altitude %d\n",Troot->altitude);
//	printf(" node is %d\t",Troot->node_id);
//	printf("left  %d\t",Troot->left_direction);
//	printf("righ %d\n",Troot->right_direction);
//	printf("\n");

//	printf("buf_num is %d\n", Troot->buf_num);
//	printf("buf latency is %d\n", Troot->delta_length_buf);
//	printf("\n");
//	if ( Troot->ls != NULL)
//		printf(" left son (%d %d) \n", Troot->ls->x, Troot->ls->y);
//	if ( Troot->rs != NULL)
//		printf(" right son (%d %d)\n", Troot->rs->x, Troot->rs->y);
//	printf("\n");
	check_DME_tree(Troot ->ls );
	check_DME_tree(Troot ->rs );
	return blk_index1;
}







void check_sink_alt_array(double * sink_alt_array, int lnum){
int i ;
	for ( i = 0 ;i < lnum ; i++)
		printf(" %f\t ", *(sink_alt_array+i) );
	printf("\n");


}


void putFIFO(BUF_POS * b, int totalnum){

	if(Findex2 >= totalnum){
		printf(" out of FIFO range error 1 !\n");
		exit(1);
	}

	(FIFO2+Findex2)->altitude = b ->altitude ;
	(FIFO2+Findex2)->x = b ->x;
	(FIFO2+Findex2)->y = b ->y;
	(FIFO2+Findex2)->prev = b ->prev;
	(FIFO2+Findex2)->next = b ->next;
	(FIFO2+Findex2)->isit = b ->isit;
	Findex2 ++ ;
}

int popFIFO(BUF_POS * b){
	Findex1 -- ;
	if (Findex1 < 0 ){
		return 0 ;
	}
	b->altitude = (FIFO1+Findex1)->altitude ;
	b->x = (FIFO1+Findex1)->x ;
	b->y = (FIFO1+Findex1)->y ;
	b->next= (FIFO1+Findex1)->next ;
	b->prev= (FIFO1+Findex1)->prev ;
	b->isit= (FIFO1+Findex1)->isit ;
	return 1;
}

int poreFIFO(){
int i ;
	if (Findex2 <= 0 )
		return 0;
	Findex1 = Findex2 ;
	Findex2 = 0 ;
	for (i = 0 ;i < Findex1 ; i++){
		(FIFO1+i )->altitude = (FIFO2+i ) ->altitude ;
		(FIFO1+i )->x = (FIFO2+i ) ->x;
		(FIFO1+i )->y = (FIFO2+i ) ->y;
		(FIFO1+i )->prev = (FIFO2+i ) ->prev;
		(FIFO1+i )->next = (FIFO2+i ) ->next;
		(FIFO1+i )->isit = (FIFO2+i ) ->isit;

	}
	return 1;

}

void cpy_double (int *saa, int *sbb, int lnum ){
int i ;
	for ( i = 0 ; i < lnum ; i++)
		sbb[i] = saa[i] ;

}

int find_shortest_path(int nd, DME_TREE_NODE * Troot, DME_TREE_NODE **map , int *scc){
int i ;
int left, right ;
int mini;
int minv ;
	left = (*(map+nd))->left ;
       right = ( *(map+nd)) -> right ;

//	printf(" left is %d\n", left);
//	printf(" right is %d\n", right);

	if ( left == -1 && right == -1)
		return  -1;
	for ( i = left ; i<=right ; i++){
		if ( i == left){
			mini = i ;
			minv = scc[i] ;
		}
		if (scc[i] < minv ){
			mini = i ;
			minv = scc[i] ;
		}

	}
	if (minv >= MAX_DOUBLE) return -1;
	return mini;

}


int find_one_buf_pos(int loopi, BUF_POS * ibp, DME_TREE_NODE *Troot, DME_TREE_NODE ** map,BUF_POS *ibm, int stepv, double factor) {
int remainV = (int)((double)stepv * factor); // remainV < 0 means okay , I find
int distV ;
int i ;
int x1, y1 ;
int x2, y2 ;
int left, right;
BUF_NODE * bf;
BUF_NODE * bt;
int lastnode;
int dubudu = 0;
//printf(" remainV is %f\n", remainV);
//printf(" disV is %f\n", distV);

// int find_right_direction(target, left_son's left and right, righ_son's left and right)
	if ( loopi == 0) {
//printf(" step is %d\t delta is %d\n", stepv, (*(map+ibp->next))->delta_length_buf);
// 		sleep(3);
		stepv = (int)((double)stepv * (*(map+ibp->next))->first_buf_fraction ) ;
		lastnode = ibp->next;
		if((*(map+lastnode))->duplicate_first_buf == 1 )
			dubudu = 1;
		remainV = (int) ((double)stepv * factor);
		if ((*(map+ibp->next))->is_blk ) {
			printf(" error when check if the first node is blockage node\n");
			return 0;
		}
	}

	if ( loopi == 1){
		if((*(map+lastnode))->duplicate_first_buf == 1 ){
			stepv = stepv - (int)(buflib.lib[0].icap*2/wirelib.lib[0].c);

		}
		remainV = (int)((double)stepv * factor);
	}


	stepv =(int)((double) stepv * factor) ;
	ibm -> altitude = ibp -> altitude - stepv;
	ibm -> x	=	ibp -> x;
	ibm -> y	=	ibp -> y;
	ibm -> prev =	ibp -> prev;
	ibm -> next =	ibp -> next;
	ibm -> isit	=	ibp -> isit;

//	printf(" next is %d\n", ibm->next);

	while(1){
		distV = ibp->altitude - (*(map + ibm -> prev))->altitude ;
//		printf("remainV is %d\n", remainV);
//		printf("disV is %d\n", distV);

		if ( remainV - distV <= 0 ){
			break;
		}
		if ( (*(map+ibm->prev))->fa == NULL  ){
			printf("the last node is %d\n",  (*(map+ibm->prev))->node_id);
			return 0 ;
		}

		ibm -> next = ibm -> prev;
		if ( (*(map+ibm->prev))->fa != NULL ){
				ibm->prev = (*(map+ibm->prev))->fa->node_id;
				continue;
			}
	}
	remainV -=( ibp->altitude - ((*(map+ibm->next))->altitude));
	// here I need to add the coordination to the new tree
	x1 =  (*(map+ibm->prev))-> x ;
	y1 =  (*(map+ibm->prev))-> y ;
   	x2 =  (*(map+ibm->next))-> x ;
	y2 =  (*(map+ibm->next))-> y ;

//	printf(" x1 y1 x2 y2 is %d %d %d %d\n", x1,y1,x2,y2);
//	printf(" remainV is %d\n", remainV);
//	printf(" adx1x2 is %d\n", Absdist(x1,x2));

	if ( (*(map+ibm->prev)) != NULL && (*(map+ibm->prev))->ls == (*(map+ibm->next))){
		remainV = (*(map+ibm->next))->altitude - (*(map+ibm->prev))->altitude - remainV;
//		printf("remainV is %d\n",remainV);
		switch ((*(map+ibm->prev))->left_direction){
			case UP :
				if ( (Absdist(y1,y2)) >= remainV ){
					ibm -> y= y1 + remainV ;
					ibm -> x = x1 ;
				}
				else {
					remainV -= Absdist(y1,y2);
					ibm -> y= y2 ;
					ibm -> x = (x1>=x2)?(x1-remainV):(x1+remainV);
				}
				break;
			case DOWN :
				if ( (Absdist(y1,y2)) >= remainV ){
					ibm -> y= y1 - remainV ;
					ibm -> x = x1 ;
				}
				else {
					remainV -= Absdist(y1,y2);
					ibm -> y= y2 ;
					ibm -> x = (x1>=x2)?(x1-remainV):(x1+remainV);
				}

				break;
			case LEFT :
				if ( (Absdist(x1,x2)) >= remainV ){
					ibm -> x = x1-remainV ;
					ibm -> y = y1 ;
				}
				else {
					remainV -= Absdist(x1,x2);
					ibm -> x = x2 ;
					ibm -> y = (y1>=y2)?(y1-remainV):(y1+remainV);

				}
				break;
			case RIGHT :
				if ( (Absdist(x1,x2)) >= remainV ){
					ibm -> x = x1 + remainV ;
					ibm -> y = y1 ;
				}
				else {
					remainV -= Absdist(x1,x2);
					ibm -> x = x2 ;
					ibm -> y = (y1>=y2)?(y1-remainV):(y1+remainV);

				}
				break;
			default :
				break;
			}

	}

	 if (  (*(map+ibm->prev)) != NULL && (*(map+ibm->prev))->rs == (*(map+ibm->next) )){
		remainV = (*(map+ibm->next))->altitude - (*(map+ibm->prev))->altitude - remainV;
		switch ((*(map+ibm->prev))->right_direction){
			case UP :
				if ( (Absdist(y1,y2)) >= remainV ){
					ibm -> y= y1 + remainV ;
					ibm -> x = x1 ;
				}
				else {
					remainV -= Absdist(y1,y2);
					ibm -> y= y2 ;
					ibm -> x = (x1>=x2)?(x1-remainV):(x1+remainV);
				}
				break;
			case DOWN :
				if ( (Absdist(y1,y2)) >= remainV ){
					ibm -> y= y1 - remainV ;
					ibm -> x = x1 ;
				}
				else {
					remainV -= Absdist(y1,y2);
					ibm -> y= y2 ;
					ibm -> x = (x1>=x2)?(x1-remainV):(x1+remainV);
				}

				break;
			case LEFT :
				if ( (Absdist(x1,x2)) >= remainV ){
					ibm -> x = x1-remainV ;
					ibm -> y = y1 ;
				}
				else {
					remainV -= Absdist(x1,x2);
					ibm -> x = x2 ;
					ibm -> y = (y1>=y2)?(y1-remainV):(y1+remainV);

				}
				break;
			case RIGHT :
				if ( (Absdist(x1,x2)) >= remainV ){
					ibm -> x = x1 + remainV ;
					ibm -> y = y1 ;
				}
				else {
					remainV -= Absdist(x1,x2);
					ibm -> x = x2 ;
					ibm -> y = (y1>=y2)?(y1-remainV):(y1+remainV);

				}
				break;
			default :
				break;
			}

	}


/*

	if ( (Absdist(x1,x2)) >= remainV ){
		ibm -> x = (x1>=x2)?(x1-remainV):(x1+remainV);
		ibm -> y = y1 ;
	}
	else {
		remainV -= Absdist(x1,x2);
		ibm -> x = x2 ;
		ibm -> y = (y1>=y2)?(y1-remainV):(y1+remainV);

	}
*/

/*
	if (   ((Absdist(ibm->x, x1 )) + ((Absdist(ibm->y, y1 ))) <= ((LAMBADA) /(Lsmall)))){
		ibm->next = ibm->prev ;
		ibm->x = x1;
		ibm->y = y1;

	}
	else if (   ((Absdist(ibm->x, x2 )) + ((Absdist(ibm->y, y2 ))) <= ((LAMBADA) /(Lsmall)))){
		ibm->prev = ibm->next ;
		ibm->x = x2;
		ibm->y = y2;
	}
*/
	int wtype = (*(map+ibm->next))-> wire_type;
	left = (*(map+ibm->next))->left ;
	right = (*(map+ibm->next))->right;
	buf_i ++ ;
	i = (*(map + ibm ->next))->node_id ;
	bf = (*(bufnode+i)) ;
	if ( dubudu == 1)
		(*(map + i))->duplicate_first_buf = 1;

	if ( bf->next == NULL){
		bf->next= (BUF_NODE *) malloc(sizeof(BUF_NODE)) ;
		bf->next->x = ibm->x;
		bf->next->y = ibm->y;
		bf->next->buf_id = buf_i ;
		bf->next->wire_type = wtype;
		bf->next->buf_type = BUFTYPE;
		bf->next->units = BUF_UNIT;
		bf->next->next = NULL ;
		total_buf_num += BUF_UNIT;
		buf_i ++;
	}
	else {
		bt = bf->next ;
		bf->next= (BUF_NODE *) malloc(sizeof(BUF_NODE)) ;
		bf->next->x = ibm->x;
		bf->next->y = ibm->y;
		bf->next->buf_id = buf_i ;
		bf->next->wire_type = wtype;
		bf->next->buf_type = BUFTYPE;
		bf->next->units = BUF_UNIT;
		bf->next->next = bt;
		total_buf_num += BUF_UNIT;
		buf_i ++;


	}

/*
	while ( bf->next != NULL )
		bf = bf->next ;
	bf->next= (BUF_NODE *) malloc(sizeof(BUF_NODE)) ;
	bf->next->x = ibm->x;
	bf->next->y = ibm->y;
	bf->next->buf_id = buf_i ;
	bf->next->next = NULL ;
	buf_i ++;
*/

	return 1;
}



static int StackIndex = 0 ;
int *Stack ;
void putStack(int nd){
	if ( StackIndex >= STACK_SIZE){
		printf(" Stack out of range error1\n");
		exit(0);
	}
	Stack[StackIndex++] = nd ;

}

int 	popStack(){
	if (StackIndex <= 0 ){
		//printf("Stack out of range error2\n");
		return -1;
	}
	return Stack[--StackIndex] ;
}

void draw_point( FILE *fp , double x1, double y1, int dash, int colour){
double factor , upleft_x, upleft_y;
	if (frame.ur.x != 0)
		factor = (double)9500 / frame.ur.x;

	if (factor > 9500 / frame.ur.y)
		factor = (double)9500 / frame.ur.y;
	upleft_x = x1*factor + OFFSET;
	upleft_y = y1*factor + OFFSET;
	fprintf(fp,"1 3 %d 1 %d 7 50 -1 -1 0 1 0 %.0f %.0f 40 40 %.0f %.0f %.0f %.0f\n",dash,colour, upleft_x,upleft_y,upleft_x,upleft_y,upleft_x,upleft_y+40);

}

void draw_coord(FILE *fp, double x1, double y1, int index, double delay){
double factor , upleft_x, upleft_y;
	if (frame.ur.x != 0)
		factor = (double)9500 / frame.ur.x;

	if (factor > 9500 / frame.ur.y)
		factor = (double)9500 / frame.ur.y;
	upleft_x = x1*factor + OFFSET;
	upleft_y = y1*factor + OFFSET;

/*	if (delay > 0.0)
		printf("node_id %d\t delay is %f\n",index,delay);*/
	fprintf(fp,"4 0 0 50 -1 0 12 0.0000 4 150 315 %.0f %.0f %d\\001\n",upleft_x,upleft_y,index);
}
void draw_wire(FILE *fp, double x1, double y1, double x2, double y2, int dash, int colour){
double factor , upleft_x, upleft_y,downright_x,downright_y;
double mid_x , mid_y ;
		if (frame.ur.x != 0)
			factor = (double)9500 / frame.ur.x;
		if (factor > 9500 / frame.ur.y)
			factor = (double)9500 / frame.ur.y;
		upleft_x = x1*factor + OFFSET;
		upleft_y = y1*factor + OFFSET;
		downright_x =x2 * factor + OFFSET;
		downright_y = y2 * factor + OFFSET;

		if (upleft_x == downright_x ||upleft_y == downright_y){
			fprintf(fp,"2 1 %d 1 %d 7 50 -1 -1 0 0 0 -1 0 0 2\n", dash,colour);
			fprintf(fp,"\t%.0f %.0f %.0f %.0f\n",upleft_x,upleft_y,downright_x,downright_y);
		}
		else{
			mid_x = downright_x;
			mid_y = upleft_y;
			fprintf(fp,"2 1 %d 1 %d 7 50 -1 -1 0 0 0 -1 0 0 2\n", dash,colour);
			fprintf(fp,"\t%.0f %.0f %.0f %.0f\n",upleft_x,upleft_y,mid_x,mid_y);
			fprintf(fp,"2 1 %d 1 %d 7 50 -1 -1 0 0 0 -1 0 0 2\n", dash,colour);
			fprintf(fp,"\t%.0f %.0f %.0f %.0f\n",mid_x,mid_y,downright_x,downright_y);
		}

}







void print_fig_3(DME_TREE_NODE *Troot , DME_TREE_NODE ** map){
	FILE* fp;
	char * filename = "clockTree.fig";
	fp = (FILE *) fopen(filename,"w");
	double factor;
	double upleft_x;
	double upleft_y;
	double downright_x;
	double downright_y;
	double width;
	double height;
	double t1_x,t1_y,t2_x,t2_y;
	BUF_NODE *bf ;
	DME_TREE_NODE *t ;
	Stack = (int *) malloc(sizeof(int)*STACK_SIZE);


	if (frame.ur.x != 0)
		factor = (double)9500 / frame.ur.x;

	if (factor > 9500 / frame.ur.y)
		factor = (double)9500 / frame.ur.y;
// 	printf("%f\n",factor);
	fprintf(fp,"#FIG 3.1\n");
	fprintf(fp,"Landscape\n");
	fprintf(fp,"Center\n");
	fprintf(fp,"Inches\n");
	fprintf(fp,"1200 2\n");
	int i;
	int nd;

	// draw root 0
	upleft_x =(double)( Troot->x );
	upleft_y = (double)(Troot->y );
	upleft_x = upleft_x*factor + OFFSET;
	upleft_y = upleft_y*factor + OFFSET;
	fprintf(fp,"1 3 0 1 0 7 50 -1 -1 0 1 0 %.0f %.0f 40 40 %.0f %.0f %.0f %.0f\n",upleft_x,upleft_y,upleft_x,upleft_y,upleft_x,upleft_y+40);

	putStack(Troot->node_id) ;
	while ( (nd = popStack()) != -1){
		t = (*(map+nd))->ls ;
		if ( t != NULL) {
			putStack( t->node_id);
			bf = (*(bufnode+t->node_id))->next ;
			if (bf != NULL){
				upleft_x =(double)( bf->x );
				upleft_y = (double)(bf->y );
				draw_coord(fp,upleft_x,upleft_y,bf->buf_id,0.0);
				if( bf->next == NULL &&  t->duplicate_first_buf == 1)
					draw_point(fp,upleft_x,upleft_y,SOLID, RED) ;
				else
					draw_point(fp,upleft_x,upleft_y, SOLID, GREEN) ;
				draw_wire(fp,(double)((*(map+nd))->x), (double)((*(map+nd))->y),upleft_x,upleft_y, SOLID, BLACK);
			}
			else {
				draw_coord(fp,(double)(t->x),(double)(t->y),t->node_id,t->delay);
				draw_point(fp,(double)(t->x),(double)(t->y),SOLID, BLACK) ;
				draw_wire(fp,(double)((*(map+nd))->x),(double)( (*(map+nd))->y),(double)(t->x),(double)(t->y),SOLID, BLACK);
			}
			while( bf != NULL){
				if ( bf->next != NULL){
					upleft_x = (double)(bf->next->x) ;
					upleft_y = (double)(bf->next->y) ;
					draw_coord(fp,upleft_x,upleft_y,bf->next->buf_id,0.0);
					if( bf->next->next == NULL &&  t->duplicate_first_buf == 1)		draw_point(fp,upleft_x,upleft_y,SOLID, RED) ;
					else
						draw_point(fp,upleft_x,upleft_y,SOLID, GREEN) ;
					draw_wire(fp,(double)(bf->x), (double)(bf->y),upleft_x,upleft_y,SOLID, BLACK);
				}
				else {
					draw_coord(fp,(double)(t->x),(double)(t->y),t->node_id,t->delay);
					draw_point(fp,(double)(t->x),(double)(t->y),SOLID,BLACK) ;
					draw_wire(fp,(double)(bf->x), (double)(bf->y),(double)(t->x),(double)(t->y),SOLID, BLACK);
				}
				bf = bf->next ;
			}

		}
		t = (*(map+nd))->rs ;
		if ( t != NULL) {
			putStack( t->node_id);
			bf = (*(bufnode+t->node_id))->next ;
			if (bf != NULL){
				upleft_x = (double)(bf->x );
				upleft_y = (double)(bf->y );
				draw_coord(fp,upleft_x,upleft_y,bf->buf_id,0.0);
				if( bf->next == NULL &&  t->duplicate_first_buf == 1)
					draw_point(fp,upleft_x,upleft_y,SOLID, RED) ;
				else
					draw_point(fp,upleft_x,upleft_y, SOLID, GREEN) ;


	//			draw_point(fp,upleft_x,upleft_y,SOLID, GREEN) ;
				draw_wire(fp,(double)((*(map+nd)))->x, (double)((*(map+nd))->y),upleft_x,upleft_y,SOLID, BLACK);
			}
			else {
				draw_coord(fp,(double)(t->x),(double)(t->y),t->node_id,t->delay);
				draw_point(fp,(double)(t->x),(double)(t->y),SOLID, BLACK) ;
				draw_wire(fp,(double)((*(map+nd))->x),(double)( (*(map+nd))->y),(double)(t->x),(double)(t->y),SOLID, BLACK);
			}
			while( bf != NULL){
				if ( bf->next != NULL){
					upleft_x = bf->next->x ;
					upleft_y = bf->next->y ;
					draw_coord(fp,upleft_x,upleft_y,bf->next->buf_id,0.0);
					if( bf->next->next == NULL &&  t->duplicate_first_buf == 1)		draw_point(fp,upleft_x,upleft_y,SOLID, RED) ;
					else
						draw_point(fp,upleft_x,upleft_y,SOLID, GREEN) ;

//					draw_point(fp,upleft_x,upleft_y,SOLID, GREEN) ;

					draw_wire(fp,(double)(bf->x), (double)(bf->y),upleft_x,upleft_y,SOLID, BLACK);

				}
				else {
					draw_coord(fp,(double)(t->x),(double)(t->y),t->node_id,t->delay);
					draw_point(fp,(double)(t->x),(double)(t->y),SOLID,BLACK) ;
					draw_wire(fp,(double)(bf->x), (double)(bf->y),(double)(t->x),(double)(t->y),SOLID, BLACK);
				}
				bf = bf->next ;
			}

		}


	}
// 	printf(" blocage nu mis %d\n",blockage.num);
// 	exit(0);

	for ( i = 0 ; i < blockage.num ; i++){
		upleft_x = (double) (blockage.pool[i].ll.x);
		upleft_y = (double) (blockage.pool[i].ll.y);
		downright_x = (double) (blockage.pool[i].ur.x);
		downright_y = (double) (blockage.pool[i].ur.y);
		draw_wire(fp,upleft_x,upleft_y,downright_x,downright_y, Dashed, RED);
		draw_wire(fp,downright_x,downright_y,upleft_x,upleft_y,Dashed, RED);
	}


	fclose(fp);
}








void Enhance_Buf(DME_TREE_NODE *Troot,  DME_TREE_NODE ** map, int totalnum,int realsink){
int i ;
BUF_NODE * bf;
	buf_a = 0;
	for ( i = 1 ; i < totalnum ; i++){
		// ((*(map+i))->is_blk) !=1 &&
		if (   ((*(map+i))->duplicate_first_buf ==1)){
			bf = (*(bufnode + i))->next ;
			if (bf == NULL  )
				continue;
//			while( bf ->next != NULL)
//				bf = bf ->next ;
//			bf->next = (BUF_NODE *)malloc(sizeof(BUF_NODE));
//			bf->next->buf_id = bf->buf_id;
/*			printf("(%d %d)\n",(*(map+i))->x,(*(map+i))->y);
			printf(" I am here\n"); */
			buf_a ++;
			buf_a ++;
//			bf->next->x = bf->x ;
//			bf->next->y = bf->y;
//			bf->next->next = NULL ;
		}

	}
	num_node = buf_i -  realsink;
	num_buffer = (buf_i+buf_a-totalnum+1)/2;
	num_wire = totalnum-1+(buf_i-totalnum+1)/2;
	num_sinknode = realsink ;
	num_total_nodes  = totalnum ;



}

void cpy_buf( BUF_POS *b1, BUF_POS * b2){
	b1->altitude = b2->altitude ;
	b1->isit = b2->isit ;
	b1->next  = b2->next ;
	b1->prev = b2->prev ;
	b1->x  = b2->x;
	b1->y =  b2->y ;
}


void Buffer_Placement (int  nodei , DME_TREE_NODE *Troot, int K, int lnum, int totalnum, DME_TREE_NODE ** DME_tree_map,double factor){
int ii, flag;
int j;
int minNode ;
int Lmin ;
int stepv;
int wtype;
BUF_POS bp ;
BUF_POS bm ;
BUF_POS bn ;
	bp.altitude = (*(DME_tree_map+nodei))->altitude;
	bp.x =  (*(DME_tree_map+nodei))->x;
	bp.y = (*(DME_tree_map+nodei))->y;
	bp.prev =  (*(DME_tree_map+nodei))->fa->node_id;
	bp.next = (*(DME_tree_map+nodei))->node_id;
	bp.isit = 1 ;
	wtype = (*(DME_tree_map+nodei))->wire_type;
	for (ii = 0 ; ii < K ; ii++){

					if ( nodei <= realson)
						stepv = LAMBADA3;
					else{
						if ( wtype == 0)
							stepv = LAMBADA_w1 ;
						else if (wtype == 1)
							stepv = LAMBADA_w2 ;
						else{
							printf("buffer_place type error\n");
							exit(0);
						}
					}
					flag = find_one_buf_pos(ii, &bp,  Troot, DME_tree_map,&bm, stepv,factor) ;
					if ( flag == 1 )
						cpy_buf(&bp,&bm);
					else {
						printf(" fail to find new position for buf\n");
						exit(0);
					}
	}









return ;


}




static int blk_index = 0;
int check_DME(DME_NODE * af) {
	if ( af == NULL) return ;
//	printf("(%d, %d) %d\n", af->select_x, af->select_y, af->buf_num);
//	printf("blk %d sink %d\t",af->blockage_node,af->sink_index);
//	printf("left dir %d left dir %d\t",af->left_direction,af->left_direction);
//	if ( af->duplicate_first_buf > 0){
//		printf("???????????????? (%d %d)\n",af->select_x,af->select_y);
//		printf(" %d \n", af->duplicate_first_buf);
//	}
	if(af->blockage_node == 0 && af->pleft != NULL){
		DME_NODE *aa=af->pleft;
		int length = Manhattan(af->select_x, af->select_y, aa->select_x, aa->select_y);
//		printf("%d %d, (%d %d) %f. %d %d!! left\n",length, aa->detour, af->select_x, af->select_y,af->left_length,aa->select_x,aa->select_y);
		while(aa->blockage_node == 1){
			length += Manhattan(aa->select_x, aa->select_y, aa->pleft->select_x, aa->pleft->select_y);
			aa = aa->pleft;
//			printf("%d %d, %f!! left\n",length, aa->detour, af->left_length);
		}
		if(ABS(length + aa->detour - af->left_length)>5){
			printf("%d %d %d %d, %f shit!! left\n",af->select_x, af->select_y, aa->select_x, aa->select_y, af->left_length);
			printf("%d %d, %f shit!! left\n",length, aa->detour, af->left_length);
			exit(0);
		}
	}
	if(af->blockage_node == 0 && af->pright != NULL){
		DME_NODE *aa=af->pright;
		int length = Manhattan(af->select_x, af->select_y, aa->select_x, aa->select_y);
		while(aa->blockage_node == 1){
			length += Manhattan(aa->select_x, aa->select_y, aa->pleft->select_x, aa->pleft->select_y);
			aa = aa->pleft;
		}
		if(ABS(length+aa->detour-af->right_length)>5){
			printf("%d %d %d %d, %f shit!! right\n",af->select_x, af->select_y, af->pright->select_x, af->pright->select_y, af->right_length);
			exit(0);
		}
	}
	if ( af ->blockage_node == 1 )
		blk_index ++ ;
	check_DME(af -> pleft) ;
	check_DME(af -> pright) ;
	return blk_index ;

}


void Out_put_files_coordinate(FILE *fp, BUF_NODE ** bufnode1, DME_TREE_NODE * Troot){
BUF_NODE * b ;
	if (Troot == NULL || Troot->ls == NULL )
		return ;
	if (Troot->node_id != 0)
		fprintf(fp, "%d %d %d \n", Troot->node_id, (int)Troot->x, (int)Troot->y);
	if (Troot->ls != NULL){
		if (Troot->ls->node_id >= LARGESTNODE)
			b = NULL;
		else
			b = (*(bufnode1 + Troot->ls->node_id))->next ;
		while( b != NULL){
			fprintf(fp,"%d %.1f %.1f\n", b->buf_id, b->x,b->y);
			fprintf(fp,"%d %.1f %.1f\n", (b->buf_id)+1, b->x, b->y);
			b = b->next ;
		}
	}
	if (Troot->rs != NULL){
		b = (*(bufnode1 + Troot->rs->node_id))->next ;
		while( b != NULL){
			fprintf(fp,"%d %.1f %.1f\n", b->buf_id, b->x, b->y);
			fprintf(fp,"%d %.1f %.1f\n", (b->buf_id)+1,b->x,b->y);
			b = b->next ;
		}
	}
	Out_put_files_coordinate(fp,bufnode1,Troot->ls);
	Out_put_files_coordinate(fp,bufnode1,Troot->rs);


}



void Out_put_files_sink(FILE *fp, DME_TREE_NODE * Troot){
	if (Troot == NULL )
		return ;

	if (Troot->ls == NULL)
		fprintf(fp,"%d %d\n", Troot->node_id, (int) Troot->sink_index);

	Out_put_files_sink(fp,Troot->ls);
	Out_put_files_sink(fp,Troot->rs);


}







void Out_put_files_connect(FILE * fp, BUF_NODE ** bufnode1, DME_TREE_NODE * Troot){
BUF_NODE * b ;
//static int i;
	if (Troot == NULL)
		return ;



	if (Troot->ls != NULL){
		if (Troot->ls->node_id >= LARGESTNODE)
			b = NULL;
		else
			b = (*(bufnode1 + Troot->ls->node_id))->next ;
		if ( b != NULL)
			fprintf(fp,"%d %d %d\n", Troot->node_id, b->buf_id,b->wire_type);
		else
			fprintf(fp,"%d %d %d\n", Troot->node_id, Troot->ls->node_id,Troot->ls->wire_type);

		while( b != NULL){
		//	printf("%d %d %d\n", b->buf_id, b->buf_id,0);
			if ( b->next != NULL)
				fprintf(fp,"%d %d %d\n", b->buf_id+1, b->next->buf_id,b->next->wire_type);
			else
				fprintf(fp,"%d %d %d\n", b->buf_id+1, Troot->ls->node_id,Troot->ls->wire_type);
			b = b->next ;
		}

	}
	if (Troot->rs != NULL){
		b = (*(bufnode1 + Troot->rs->node_id))->next ;
		if ( b != NULL)
			fprintf(fp,"%d %d %d\n", Troot->node_id, b->buf_id,b->wire_type);
		else
			fprintf(fp,"%d %d %d\n", Troot->node_id, Troot->rs->node_id,Troot->rs->wire_type);

		while( b != NULL){
		//	printf("%d %d %d\n", b->buf_id, b->buf_id,0);
			if ( b->next != NULL)
				fprintf(fp,"%d %d %d\n", b->buf_id+1, b->next->buf_id,b->next->wire_type);
			else
				fprintf(fp,"%d %d %d\n", b->buf_id+1, Troot->rs->node_id,Troot->rs->wire_type);
			b = b->next ;
		}

	}


	Out_put_files_connect(fp,bufnode1,Troot->ls);
	Out_put_files_connect(fp,bufnode1,Troot->rs);


}

extern int highway_extra_buf ;

void Out_put_files_buf(FILE * fp, int lnum,   BUF_NODE ** bufnode1 ,  DME_TREE_NODE ** map){
BUF_NODE * b ;
int i,j;
	highway_extra_buf = 0 ;
//	printf("duplicate\n");
	for ( i = 1; i < lnum ; i++){
		if ((*(bufnode1 + i)) == NULL) continue;
		b = (*(bufnode1 + i))->next ;

		while ( b != NULL){
			for ( j = 0 ; j < b->units ; j++)
				fprintf(fp,"%d %d %d\n", b->buf_id, b->buf_id+1,b->buf_type);

			//(*(map+i))-> != 1 &&
//			if (  (*(map+i))->duplicate_first_buf ==1 && b->next == NULL){
//				printf("(%d %d)\n",(*(map+i))->x,(*(map+i))->y);
//				fprintf(fp,"%d %d %d\n", b->buf_id, b->buf_id+1,b->buf_type);
//				fprintf(fp,"%d %d %d\n", b->buf_id, b->buf_id+1,b->buf_type);
//			}
//			if ( i == 1 ){
//				j = HIGHWAY;
//				while ( j--){
//					fprintf(fp,"%d %d %d\n", b->buf_id, b->buf_id+1,b->buf_type);
//				}
//			}

			b = b->next ;
		}

	}


}

void cpy_dme_tree_node(DME_TREE_NODE * d1, DME_TREE_NODE * d2, int mode){

	// d1 ->altitude = d2->altitude;
	d1-> x = d2->x ;
	d1-> y = d2->y;
	d1 ->detour = d2 ->detour ;
	d1 -> delay = d2->delay;
	d1-> left_direction = d2->left_direction ;
	d1 -> right_direction = d2 ->right_direction;
	d1 -> wire_type = d2->wire_type;
	d1 -> is_sink = d2 -> is_sink ;
	d1 -> buf_num = d2 ->buf_num ;
//	d1 ->delta_length_buf = d2 ->delta_length_buf ;
	d1 ->first_buf_fraction = d2 -> first_buf_fraction;
	d1 ->factor = d2 ->factor;
	d1 ->reduntant = d2->reduntant ;
	d1 ->fa = d2->fa;
	if ( mode == -1){
		d1->ls = NULL;
		d1->rs = NULL;
		d1 -> duplicate_first_buf = 0 ;
		d1->sink_index = -1;
		d1->is_blk = 1 ;
	}
	else if ( mode == 1){
		d1 ->ls = d2 -> ls;
		d1 ->rs = d2 -> rs;
		d1 -> duplicate_first_buf = d2 ->duplicate_first_buf ;
		d1 ->sink_index = d2 ->sink_index ;
		d1 ->is_blk = d2 ->is_blk;
	}

}

DME_TREE_NODE * stream_down(DME_TREE_NODE * tg, DME_TREE_NODE * src ,int * num ,int flag,int wtype){
DME_TREE_NODE * temp ;
int loop  = 0;
	if ( src == NULL){
		printf(" error when doing detour: no source\n");
		exit(0);
	}

	if ( tg->ls != NULL){
		printf(" error when doing detour: the target is not empty\n");
		exit(0);
	}

	if ( (flag == 0 && src->ls == NULL) ||(flag==1 && src->rs==NULL))
		return tg ;

	do {
		if ( loop == 0 && flag == 1){
			temp = tg;
			tg -> ls = (DME_TREE_NODE *) malloc( sizeof(DME_TREE_NODE));
			tg = tg->ls;
			src = src -> rs;
			cpy_dme_tree_node(tg,src,-1);
			tg -> wire_type = wtype ;
			tg ->is_blk = 1;
			tg->fa = temp ;
			(*num) ++ ;
			loop ++ ;
		}
		else {
			temp = tg;
			tg -> ls = (DME_TREE_NODE *) malloc( sizeof(DME_TREE_NODE));
			tg = tg->ls;
			src = src -> ls;
			cpy_dme_tree_node(tg,src,-1);
			tg -> wire_type = wtype ;
			tg ->is_blk = 1;
			tg->fa = temp ;
			(*num) ++ ;
			loop ++ ;
		}
	}
	while ( src->is_blk == 1) ;

	return tg ;

}

DME_TREE_NODE * stream_up(DME_TREE_NODE * tg, DME_TREE_NODE * src , int  num, int flag,int wtype){
DME_TREE_NODE * temp ;
DIRECTION  moveon;
	if ( src == NULL){
		printf(" error when doing detou0r: no source\n");
		exit(0);
	}

	if ( tg->ls != NULL){
		printf(" error when doing detour: the target is not empty\n");
		exit(0);
	}

	if ( src->fa == NULL)
		return tg ;

	do {
		temp = tg;
		tg -> ls = (DME_TREE_NODE *) malloc( sizeof(DME_TREE_NODE));
		tg = tg->ls;
		src = src -> fa;

		if ( flag == 1 && num == 1)
			moveon = src->right_direction;
		else
			moveon = src->left_direction ;

		switch(moveon) {
			case RIGHT:
				if ( temp->y  == src -> y )
					temp->left_direction = LEFT;
				else if ( temp->y  > src -> y)
					temp->left_direction  = DOWN;
				else
					temp->left_direction = UP;
				break;
			case LEFT:
				if (temp->y == src -> y)
					temp->left_direction = RIGHT;
				else if ( temp->y  > src -> y)
					temp->left_direction  = DOWN;
				else
					temp->left_direction = UP;
				break;
			case UP:
				if ( temp->x == src->x)
					temp->left_direction = DOWN ;
				else if ( temp->x > src->x )
					temp->left_direction = LEFT ;
				else
					temp->left_direction = RIGHT ;
				break;
			case DOWN:
				if ( temp->x == src->x)
					temp->left_direction = UP ;
				else if ( temp->x > src->x )
					temp->left_direction = LEFT ;
				else
					temp->left_direction = RIGHT ;
				break;
			default:
				break;

		}

		cpy_dme_tree_node(tg,src,-1);
		tg -> wire_type = wtype ;
		if ( flag == 1 && num == 1)
			tg->left_direction = src->right_direction;
		tg ->is_blk = 1;
		tg->fa = temp ;
		num -- ;

	}
	while ( src->is_blk == 1 && num>0) ;

	return tg ;

}



DME_TREE_NODE * stream_down_stop(DME_TREE_NODE * tg, DME_TREE_NODE * src, int length ,int * num, int flag,int wtype){
DME_TREE_NODE * temp ;
DIRECTION moveon;
int time = 0 ;
int  tour = length ;
int x1, y1 , x2, y2 ;
	if ( src == NULL){
		printf(" error when doing detour: no source\n");
		exit(0);
	}

	if ( tg->ls != NULL){
		printf(" error when doing detour: the target is not empty\n");
		exit(0);
	}

	if ((flag==0 && src->ls == NULL) || ( flag == 1 && src->rs == NULL))
		return tg ;

	if ( flag == 1){
		while ((time==0)?( tour >= src->rs->altitude):(tour >= src->ls->altitude)){
			temp = tg;
			tg -> ls = (DME_TREE_NODE *) malloc( sizeof(DME_TREE_NODE));
			tg = tg->ls;
			if (time == 0)
				src = src ->rs;
			else
				src = src ->ls;
			cpy_dme_tree_node(tg,src,-1);
			tg -> wire_type = wtype ;
			tg ->is_blk = 1;
			tg->fa = temp ;
			if ( time == 0)
				src = src->rs ;
			else
				src = src->ls ;
			tour -= src->altitude ;
			(*num)++;
			time ++ ;
		}

		if ( tour > 0){
			(*num)++ ;
			x1 = src -> x ;  y1 = src->y;
			if ( time == 0){
				x2 = src->rs->x ;
				y2 = src->rs->y;
			}
			else {
				x2 = src -> ls -> x ;
				y2 = src->ls->y;
			}

			temp = tg ;
			tg -> ls = (DME_TREE_NODE *) malloc( sizeof(DME_TREE_NODE));
			tg = tg->ls ;
			tg -> wire_type = wtype ;
			tg -> ls = NULL ;
			tg -> rs = NULL;
			tg -> is_blk = 1 ;
			tg -> sink_index = -1;
			tg -> fa = temp ;
			if (time == 0)
				moveon = src->right_direction ;
			else
				moveon = src->left_direction ;
			switch (moveon){
				case UP :
					if ( (Absdist(y1,y2)) >= tour ){
						tg-> y= y1 + tour ;
						tg -> x = x1 ;
					}
					else {
						tour -= Absdist(y1,y2);
						tg-> y= y2 ;
						tg-> x = (x1>=x2)?(x1-tour):(x1+tour);
					}
					break;
				case DOWN :
					if ( (Absdist(y1,y2)) >= tour ){
						tg -> y= y1 - tour;
						tg -> x = x1 ;
					}
					else {
						tour -= Absdist(y1,y2);
						tg -> y= y2 ;
						tg -> x = (x1>=x2)?(x1-tour):(x1+tour);
					}

					break;
				case LEFT :
					if ( (Absdist(x1,x2)) >= tour ){
						tg -> x = x1-tour ;
						tg -> y = y1 ;
					}
					else {
						tour -= Absdist(x1,x2);
						tg -> x = x2 ;
						tg -> y = (y1>=y2)?(y1-tour):(y1+tour);

					}
					break;
				case RIGHT :
					if ( (Absdist(x1,x2)) >= tour ){
						tg -> x = x1 + tour ;
						tg -> y = y1 ;
					}
					else {
						tour -= Absdist(x1,x2);
						tg -> x = x2 ;
						tg -> y = (y1>=y2)?(y1-tour):(y1+tour);

					}
					break;
				default :
					break;
				}



		}



	}
	else if ( flag == 0){
		while ( tour >= src->ls->altitude){
			temp = tg;
			tg -> ls = (DME_TREE_NODE *) malloc( sizeof(DME_TREE_NODE));
			tg = tg->ls;
			src = src -> ls;
			cpy_dme_tree_node(tg,src,-1);
			tg ->is_blk = 1;
			tg -> wire_type = wtype ;
			tg->fa = temp ;
			src = src->ls ;
			tour -= src->altitude ;
			(*num)++;
		}

	if ( tour > 0){
		(*num)++ ;
		x1 = src -> x ;  y1 = src->y;
		x2 = src -> ls -> x ;  y2 = src->ls->y  ;
		temp = tg ;
		tg -> ls = (DME_TREE_NODE *) malloc( sizeof(DME_TREE_NODE));
		tg = tg->ls ;
		tg -> wire_type = wtype ;
		tg -> ls = NULL ;
		tg -> rs = NULL;
		tg -> is_blk = 1 ;
		tg -> sink_index = -1;
		tg -> fa = temp ;
		switch (src->left_direction){
			case UP :
				if ( (Absdist(y1,y2)) >= tour ){
					tg-> y= y1 + tour ;
					tg -> x = x1 ;
				}
				else {
					tour -= Absdist(y1,y2);
					tg-> y= y2 ;
					tg-> x = (x1>=x2)?(x1-tour):(x1+tour);
				}
				break;
			case DOWN :
				if ( (Absdist(y1,y2)) >= tour ){
					tg -> y= y1 - tour;
					tg -> x = x1 ;
				}
				else {
					tour -= Absdist(y1,y2);
					tg -> y= y2 ;
					tg -> x = (x1>=x2)?(x1-tour):(x1+tour);
				}

				break;
			case LEFT :
				if ( (Absdist(x1,x2)) >= tour ){
					tg -> x = x1-tour ;
					tg -> y = y1 ;
				}
				else {
					tour -= Absdist(x1,x2);
					tg -> x = x2 ;
					tg -> y = (y1>=y2)?(y1-tour):(y1+tour);

				}
				break;
			case RIGHT :
				if ( (Absdist(x1,x2)) >= tour ){
					tg -> x = x1 + tour ;
					tg -> y = y1 ;
				}
				else {
					tour -= Absdist(x1,x2);
					tg -> x = x2 ;
					tg -> y = (y1>=y2)?(y1-tour):(y1+tour);

				}
				break;
			default :
				break;
			}



	}
		}
	return tg ;

}






void detour_DME_tree(DME_TREE_NODE * Troot){
int i,round;
int tour ;
int round_num = 0 ;
int lrson = 0 ;
int wtype;
DME_TREE_NODE * di ;
DME_TREE_NODE * ofa ;
DME_TREE_NODE * nfa = (DME_TREE_NODE *) malloc(sizeof(DME_TREE_NODE));
DME_TREE_NODE * temp1;
DME_TREE_NODE * temp2;

int altitude1, altitude2,delta;
	if ( Troot == NULL )
		return ;

	if ( Troot->is_blk != 1 && Troot->fa != NULL){
		lrson = 0 ;
		tour = Troot->detour ;
		wtype = Troot->wire_type;
		ofa = Troot ;
		if (tour > 0){
			do {
				temp2 = ofa;
				ofa = ofa ->fa ;
			}
			while ( ofa->is_blk == 1) ;
			if ( ofa->rs != NULL && temp2 == ofa->rs)
				lrson  = 1;
			cpy_dme_tree_node( nfa, ofa, -1) ;
			altitude1 = ofa->altitude ;
			altitude2 = Troot->altitude ;
			delta = altitude2-altitude1;
			di = nfa;
			if ( (round = (tour/(2*delta))) > 0){
				for ( i = 1; i <= round ; i++){
						round_num = 0 ;
						di = stream_down(di,ofa,&round_num,lrson,wtype);
						di = stream_up(di ,Troot,round_num,lrson,wtype);
				}

				tour -= (round * delta *2 );
			}


			if ( tour >= 2 * delta){
				printf("detour error after detour times of tour \n");
				exit(0);
			}
			tour /= 2;
//			printf(" tour is %d\n",tour);
			if ( tour > 0){
				round_num = 0 ;
				di = stream_down_stop(di,ofa,tour,&round_num,lrson,wtype);
				if (round > 0)
					di = stream_up(di,di ,round_num,0,wtype);
				else
					di = stream_up(di,di ,round_num,lrson,wtype);
			}



		if (lrson == 0){
			di->ls = ofa->ls;
			di->ls->fa = di;
			ofa->ls = nfa->ls;
			ofa->ls->fa = ofa;
		}
		else if( lrson==1){
			di->ls = ofa->rs ;
			di->ls->fa = di;
			ofa->rs = nfa->ls;
			ofa->rs->fa = ofa;
		}


			free(nfa);




		}

	}

	if ( Troot->ls != NULL)
		detour_DME_tree(Troot->ls);
	if(Troot->rs != NULL)
		detour_DME_tree(Troot->rs);


}


void draw_rectangle(FILE *fp, double x1, double y1, double x2, double y2, int dash, int colour){
double factor , upleft_x, upleft_y,downright_x,downright_y,upright_x,upright_y,downleft_x,downleft_y;
double mid_x , mid_y ; 
		if (frame.ur.x != 0)
			factor = (double)9500.0 / frame.ur.x;
		if (factor > 9500.0 / frame.ur.y)
			factor = (double)9500.0 / frame.ur.y; 
		upleft_x = x1*factor + OFFSET;
		upleft_y = y1*factor + OFFSET;
		downright_x = x2 * factor + OFFSET; 
		downright_y = y2 * factor + OFFSET;

		upright_x = downright_x;
		upright_y = upleft_y;
		downleft_x = upleft_x;
		downleft_y = downright_y;

		fprintf(fp,"2 2 %d 1 32 32 50 -1 20 0.000 0 0 -1 0 0 5\n", dash);//,colour,colour);
		fprintf(fp,"\t%.0f %.0f %.0f %.0f %.0f %.0f %.0f %.0f %.0f %.0f\n",upleft_x,upleft_y,upright_x,upright_y,
				downright_x,downright_y,downleft_x,downleft_y,upleft_x,upleft_y);
		/*
		   mid_x = downright_x;
		   mid_y = upleft_y;
		   fprintf(fp,"2 1 %d 1 %d 7 50 -1 -1 0 0 0 -1 0 0 2\n", dash,colour);
		   fprintf(fp,"\t%.0f %.0f %.0f %.0f\n",upleft_x,upleft_y,mid_x,mid_y);	
		   fprintf(fp,"2 1 %d 1 %d 7 50 -1 -1 0 0 0 -1 0 0 2\n", dash,colour);
		   fprintf(fp,"\t%.0f %.0f %.0f %.0f\n",mid_x,mid_y,downright_x,downright_y);				
		   */
}

extern int free_buf_num ;
extern int mid_sink_num ;
void insert_buffer(DME_NODE * L  , BUF_NODE *** OBUF, DME_TREE_NODE ** OT , DME_TREE_NODE *** OTmap){
int i, K;
int num_blknode ;
int num_totalnode ;
int quta_sink;
DME_TREE_NODE * Tr = NULL ;
double max_path  = 0 ;
BUF_NODE * bf;
DME_TREE_NODE ** DME_tree_map1 ;
DME_TREE_NODE *look;
		total_buf_num = 0;
		num_buffer = 0;
		highway_extra_buf = 0;
//		num_blknode = check_DME( L+ 2 * sink.num -2 ) ;
		// exit(0);
// 		printf(" num_blk node is %d\n", num_blknode ) ;

//              static int blk_index = 0;

		check_DME(L) ;
		construct_DME_tree( L, sink.num , &Tr) ;
		settle_DME_tree(Tr, 0 );
		detour_DME_tree(Tr);
		relative_dist(Tr,0,0);
		settle_DME_tree(Tr, 0 );
	//	num_blknode = check_DME_tree(Tr) ;
	//	printf("after detour!!\n");
		blk_index = 0;
		check_DME(L);
		blk_index1 = 0;
		num_blknode = 0;
		num_blknode = check_DME_tree(Tr) ;
		quta = 0 ;
		quta_sink = 0 ;
		quta_sink = count_DME_tree(Tr) ;
		num_totalnode = 0;
		num_totalnode = 2 * quta_sink  + num_blknode;
// 		printf(" sink is %d\t node is %d\n",quta_sink,num_blknode);

		//sink_alt_array = (int *) malloc(sizeof(int) * quta_sink);
		//construct_SINK_array( Tr , sink_alt_array,quta_sink);
		buf_i = 0;
		buf_i = num_totalnode -1 ;

		bufnode = (BUF_NODE **) malloc (sizeof (BUF_NODE *) *(num_totalnode )) ;
		free_buf_num = 0;
		free_buf_num = num_totalnode ;
		mid_sink_num = quta_sink;
		DME_tree_map1 = ( DME_TREE_NODE **) malloc (sizeof(DME_TREE_NODE *) * (num_totalnode));

		for ( i = 0 ; i <num_totalnode ; i++){

			(*(bufnode+i)) = (BUF_NODE *) malloc (sizeof(BUF_NODE));
			(*(bufnode+i))-> x = (*(bufnode+i))-> y = -1;
			(*(bufnode+i))->buf_id = -1 ;
			(*(bufnode+i)) -> next = NULL;
		}
		icommap = 0;
		allnode = 0;
		construct_DME_map(Tr, DME_tree_map1);
		num_blknode = 0;
		num_blknode = check_DME_tree(Tr) ;
//		printf("\n");
//		blk_index1 = 0 ;
		//check_DME_tree(Tr);
		look = Tr->ls;
		while(look->is_blk == 1)
			look = look->ls;
		realson = look->node_id;
// 		printf(" total node is %d\n", num_totalnode);
// 		printf(" the root's buf num is %d\n", (*(DME_tree_map1+1))->buf_num);
// 		sleep(1);
		for ( i = 1 ; i < allnode ; i++){
			if (  !((*(DME_tree_map1+i))-> is_blk)	) {
				K = (*(DME_tree_map1+i))->buf_num ;
				if ( i<=realson)
					BUF_UNIT = HIGHWAY;
				else
					BUF_UNIT = 2;

//				printf(" node is %d\n",i);
//				printf("K is %d\n",K);
				Buffer_Placement (i, Tr, K, quta_sink, num_totalnode, DME_tree_map1,(*(DME_tree_map1+i))->factor);
			}

		}

// 		printf(" 2: total node is %d\n", num_totalnode);
		Enhance_Buf(Tr, DME_tree_map1,num_totalnode,quta_sink);
		bf = (*(bufnode+1))->next ;
		highway_extra_buf = 0;
		while(bf!=NULL){
			highway_extra_buf ++ ;
			bf =bf->next ;
		}

//		check_sink_alt_array(sink_alt_array,sink.num);

//		max_path = max_sink_alt(sink_alt_array,sink.num);

//		K = ceil( max_path / LAMBADA );
// 		printf("K is %d\n", K);
//		if ( !(K%2) ) K += 1 ;
// 		printf("K is %d\n", K);

//		Buffer_Placement (Tr, K, sink_alt_array, sink.num, num_totalnode, DME_tree_map1) ;
		(*OBUF) = bufnode ;
		(*OT) = Tr ;
		(*OTmap) = DME_tree_map1 ;





}

void draw_rect(FILE *fp,double llx,double lly,double urx,double ury,int dash,int color){
	draw_wire(fp,llx,lly,urx,lly,dash,color);
	draw_wire(fp,llx,ury,urx,ury,dash,color);

	draw_wire(fp,llx,lly,llx,ury,dash,color);
	draw_wire(fp,urx,lly,urx,ury,dash,color);
}

void draw_block(FILE *fp,BOX b,int dash,int color){
	draw_rect(fp,(double)b.ll.x,(double)b.ll.y,(double)b.ur.x,(double)b.ur.y,dash,color);
}

void draw_blockages(FILE * fp){
	int i;
	for(i=0;i<blockage.num;i++){
		BOX p=blockage.pool[i];
		draw_rectangle(fp,(double)p.ll.x,
				(double)p.ll.y,
				(double)p.ur.x,
				(double)p.ur.y,SOLID,BLACK);
		draw_block(fp,blockage.pool[i],SOLID,BLACK);
	}
}

void draw_sinks(FILE * fp){
	int i;
	for(i=0;i<sink.num;i++){
		draw_coord(fp,(double)sink.pool[i].x,(double)sink.pool[i].y,i,0);
		draw_point(fp,(double)sink.pool[i].x,(double)sink.pool[i].y,SOLID,RED);
	}
}

void draw_wire_node(FILE *fp,NODE s,NODE t,int dash, int color){
	draw_wire(fp,(double)s.x,(double)s.y,(double)t.x,(double)t.y,
			dash,color);
}

void draw_line(FILE *fp, double x1, double y1, double x2, double y2, int dash, int colour){
	double factor , upleft_x, upleft_y,downright_x,downright_y; 	

	if (frame.ur.x != 0)
		factor = (double)9500 / frame.ur.x;
	if (factor > 9500 / frame.ur.y)
		factor = (double)9500 / frame.ur.y; 
	upleft_x = x1*factor + OFFSET;
	upleft_y = y1*factor + OFFSET;
	downright_x = x2 * factor + OFFSET; 
	downright_y = y2*factor + OFFSET;

	fprintf(fp,"2 1 %d 1 %d 7 50 -1 -1 0 0 0 -1 0 0 2\n", dash,colour);
	fprintf(fp,"\t%.0f %.0f %.0f %.0f\n",upleft_x,upleft_y,downright_x,downright_y);	
}

void draw_line_node(FILE *fp,NODE s,NODE t, int dash, int color){
	draw_line(fp,(double)s.x,(double)s.y,(double)t.x,(double)t.y,
			dash,color);
}

// draw the shortest path tree
// REQUIRE : the dijkstra has been called!
// pFig    : the FILE object to write to
// src_idx : the source point
void draw_single_source_tree(FILE * pFig,int * back,int src_idx){
	int i;
	for(i=0;i<g_size;i++){
		if( g_occupy[i] == TRUE && i != src_idx && back[i] != -1 )
			draw_line_node(pFig,g_node[i],g_node[back[i]],SOLID,BLUE);
	}
}

// draw the rectilinear shortest path we found
// REQUIRE : the dijkstra has been called!
// pFig    : the FILE object to write to
// src_idx : the source point
void draw_single_source_rectilinear(FILE * pFig,int * back,int src_idx){
	int i;
	for(i=0;i<g_size;i++){
		if( g_occupy[i] == TRUE && i != src_idx && back[i] != -1){
			int j=back[i]; // generate the meeting point first
			NODE temp;
			if( dirs[i][j] == UP ||	
			    dirs[i][j] == DOWN ){// backtrace the nodes
				temp.x = g_node[i].x;
				temp.y = g_node[j].y;
			}
			else{
				temp.x = g_node[j].x;
				temp.y = g_node[i].y;
			}
			int idx = (int)(dirs[i][j]) ;
			draw_line_node(pFig,g_node[i],temp,SOLID,BLUE);
			draw_line_node(pFig,g_node[j],temp,SOLID,BLUE);
		}
	}
}



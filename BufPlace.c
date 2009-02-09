
#include "BufPlace.h"



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
#define MAX_DOUBLE 9999999999999.99

#define Absdist(x1,x2) ((x1)-(x2))>=0?((x1)-(x2)):((x2)-(x1)) 

extern SINK sink ; 
extern BLOCKAGE blockage;

extern BOX  frame ;

extern int num_node ; 
extern int num_buffer; 
extern int num_wire ; 
extern int num_sinknode ; 
extern int num_total_nodes ; 

static int buf_i   ;
static int buf_a = 0;




BUF_POS * FIFO1 ;
BUF_POS * FIFO2 ;

static int Findex1 = 0 ; 
static int Findex2 = 0 ;
BUF_NODE ** bufnode; 



void add_DME_node(DME_NODE *l , DME_TREE_NODE ** Troot, double x, double y){
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
		
		(*Troot) ->altitude = Manhattan(l ->select_x, l ->select_y,x,y)  ;
		(*Troot) -> x = l->select_x ; 
		(*Troot) -> y = l->select_y; 
		(*Troot) -> is_sink = 0 ;
		(*Troot) ->sink_index = l ->sink_index ;
		(*Troot) ->is_blk = l ->blockage_node ; 
		(*Troot) ->left = (*Troot)->right =  0;
		(*Troot) ->ls = (*Troot) -> rs = NULL;

	}
	
	add_DME_node(l -> pleft, &((*Troot)->ls),l->select_x,l->select_y) ;
	add_DME_node(l -> pright, &((*Troot)->rs),l->select_x,l->select_y) ;
	
}

void construct_DME_tree(DME_NODE * l, int lnum, DME_TREE_NODE **Troot){
	if ( (*Troot) == NULL ){
		(*Troot) = (DME_TREE_NODE *) malloc ( sizeof(DME_TREE_NODE)) ; 
		if ( (*Troot) == NULL){
			printf(" mallock Error 1 \n");
			exit(1);
		}
	}
	(*Troot) ->altitude = 0; 
	(*Troot) ->left = (*Troot)->right =  0;
	(*Troot) -> x = (*Troot) -> y = 0.0 ; 
	(*Troot) ->ls = (*Troot) -> rs = NULL;
	(*Troot) -> is_sink = 0 ;
	add_DME_node(l+2*lnum-2,& ((*Troot)->ls), 0.0, 0.0);
	
}


void settle_DME_tree(DME_TREE_NODE * Troot , double  father) { 
	if ( Troot == NULL) 
		return ; 
	
	Troot -> altitude += father; 
	father  = Troot->altitude ;

	if(Troot -> ls != NULL)  
		settle_DME_tree(Troot->ls, Troot->altitude);
	if(Troot -> rs != NULL)
		settle_DME_tree(Troot->rs, Troot->altitude);
}

double max_sink_alt (double * sink_alt_array, int lnum){
int i ; 
double max_value = 0.0; 
	for ( i = 0 ; i < lnum ; i++)
		if ( sink_alt_array[i] > max_value ) {
			max_value = sink_alt_array[i] ; 
		}
		
	return max_value ; 
	

}


void construct_SINK_array( DME_TREE_NODE * Troot , double *sink_alt_array){
static int lnum = 0 ; 	
	if (Troot == NULL) 
		return ; 
	construct_SINK_array(Troot->ls, sink_alt_array); 
	construct_SINK_array(Troot->rs, sink_alt_array); 
	if ( Troot -> ls == NULL) {
		if ( lnum >  sink.num -1 ) {
			printf(" out of range error 1 !\n");
			exit(1); 
		}
		sink_alt_array[lnum] = Troot->altitude ; 
		Troot ->left = lnum ; 
		Troot ->right= lnum; 
		lnum ++ ; 
		
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


void construct_DME_map(DME_TREE_NODE * Troot, DME_TREE_NODE ** Tmap){
static int i = 0 ; 

	if ( Troot == NULL) 
		return ; 
	Troot -> node_id = i ; 
	(*(Tmap+i)) = Troot ; 
	i ++ ; 
	construct_DME_map(Troot ->ls, Tmap ); 
	construct_DME_map(Troot ->rs , Tmap); 	
}



void check_DME_tree(DME_TREE_NODE * Troot){
static int i = 0 ; 
	if ( Troot == NULL) 
		return ; 
	if ( Troot->ls == NULL)
		Troot->is_sink = 1 ;
	printf(" %d\t", i++);
	printf(" %f %f \t", Troot->x , Troot->y) ; 
	printf(" %f \t", Troot->altitude);
	printf("\n");
//	printf(" left %d \t", Troot->left);
//	printf(" right %d\n", Troot->right); 
	check_DME_tree(Troot ->ls ); 
	check_DME_tree(Troot ->rs ); 	

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

void cpy_double (double *saa, double *sbb, int lnum ){
int i ; 
	for ( i = 0 ; i < lnum ; i++)
		sbb[i] = saa[i] ; 

} 

int find_shortest_path(int nd, DME_TREE_NODE * Troot, DME_TREE_NODE **map , double *scc){
int i ; 
int left, right ; 
int mini; 
double minv ; 
	left = (*(map+nd))->left ;
       right = ( *(map+nd)) -> right ; 

	printf(" left is %d\n", left);
	printf(" right is %d\n", right);
	   
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


int find_one_buf_pos(BUF_POS * ibp, int target, DME_TREE_NODE *Troot, DME_TREE_NODE ** map,BUF_POS *ibm, double stepv, double *sbb) {
double remainV = stepv; // remainV < 0 means okay , I find
double distV ; 
int i ;
double x1, y1 ; 
double x2, y2 ;
int left, right; 
BUF_NODE * bf; 

//printf(" remainV is %f\n", remainV);
//printf(" disV is %f\n", distV);

// int find_right_direction(target, left_son's left and right, righ_son's left and right)
	ibm -> altitude = ibp -> altitude + stepv; 
	ibm -> x	=	ibp -> x; 
	ibm -> y	=	ibp -> y;
	ibm -> prev =	ibp -> prev; 
	ibm -> next =	ibp -> next;
	ibm -> isit	=	ibp -> isit;

	printf(" next is %d\n", ibm->next);
	
	while (1){
		distV = (*(map + ibm -> next))->altitude - ibp->altitude ;
		if ( remainV - distV <= 0 ){
			break;
		}
		if ( (*(map+ibm->next))->ls == NULL  ){	
			sbb[ (*(map+ibm->next))->left ] = MAX_DOUBLE ;
			return 0 ;
		}
		
		ibm -> prev = ibm -> next ; 
		if ( (*(map+ibm->next))->ls != NULL )
			if ( target >= (*(map+ibm->next))->ls->left && target <= (*(map+ibm->next))->ls->right){
				ibm->next = (*(map+ibm->next))->ls->node_id;
				continue;
			}
		if ( (*(map+ibm->next))->rs != NULL )
			if ( target >= (*(map+ibm->next))->rs->left && target <= (*(map+ibm->next))->rs->right){
				ibm->next = (*(map+ibm->next))->rs->node_id;			
			}
						
	}
	
	remainV -= ((*(map+ibm->prev))->altitude - ibp->altitude);

	// here I need to add the coordination to the new tree
	x1 =  (*(map+ibm->prev))-> x ; 
	y1 =  (*(map+ibm->prev))-> y ; 
   	x2 =  (*(map+ibm->next))-> x ;
	y2 =  (*(map+ibm->next))-> y ;

	printf(" x1 y1 x2 y2 is %f %f %f %f\n", x1,y1,x2,y2);
	printf(" remainV is %f\n", remainV);
	printf(" adx1x2 is %f\n", Absdist(x1,x2));
	if ( (Absdist(x1,x2)) >= remainV ){
		ibm -> x = (x1>=x2)?(x1-remainV):(x1+remainV);
		ibm -> y = y1 ;	
	}
	else {
		remainV -= Absdist(x1,x2);
		ibm -> x = x2 ; 
		ibm -> y = (y1>=y2)?(y1-remainV):(y1+remainV);
		
	}

	
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
	left = (*(map+ibm->next))->left ; 
	right = (*(map+ibm->next))->right; 
	printf(" the forbidden left is %d\t right is %d\n", left,right);
	for (i = left ; i <= right ; i ++){
		sbb[i] = MAX_DOUBLE;
	
	}
	buf_i ++ ; 
	i = (*(map + ibm ->next))->node_id ;
	bf = (*(bufnode+i)) ;	
	printf("%d\n",i);

	while ( bf->next != NULL )
		bf = bf->next ; 
	bf->next= (BUF_NODE *) malloc(sizeof(BUF_NODE)) ; 	
	bf->next->x = ibm->x; 
	bf->next->y = ibm->y;
	bf->next->buf_id = buf_i ; 
	bf->next->next = NULL ; 
	buf_i ++;
	

	
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

void draw_wire(FILE *fp, double x1, double y1, double x2, double y2, int dash, int colour){
double factor , upleft_x, upleft_y,downright_x,downright_y; 	
double mid_x , mid_y ; 
		if (frame.ur.x != 0)
			factor = (double)9500 / frame.ur.x;
		if (factor > 9500 / frame.ur.y)
			factor = (double)9500 / frame.ur.y; 
		upleft_x = x1*factor + OFFSET;
		upleft_y = y1*factor + OFFSET;
		downright_x = x2 * factor + OFFSET; 
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
	char * filename = "t1.fig";
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
	upleft_x = Troot->x; 
	upleft_y = Troot->y;
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
				upleft_x = bf->x ;
				upleft_y = bf->y ;
				draw_point(fp,upleft_x,upleft_y, SOLID, GREEN) ;
				draw_wire(fp,(*(map+nd))->x, (*(map+nd))->y,upleft_x,upleft_y, SOLID, BLACK);
			}
			else {
				draw_point(fp,t->x,t->y,SOLID, BLACK) ;				
				draw_wire(fp,(*(map+nd))->x, (*(map+nd))->y,t->x,t->y,SOLID, BLACK);
			}
			while( bf != NULL){
				if ( bf->next != NULL){
					upleft_x = bf->next->x ;
					upleft_y = bf->next->y ;					
					draw_point(fp,upleft_x,upleft_y,SOLID, GREEN) ;
					draw_wire(fp,bf->x, bf->y,upleft_x,upleft_y,SOLID, BLACK);
				}
				else {
					draw_point(fp,t->x,t->y,SOLID,BLACK) ;				
					draw_wire(fp,bf->x, bf->y,t->x,t->y,SOLID, BLACK);
				}
				bf = bf->next ;
			}

		}
		t = (*(map+nd))->rs ;
		if ( t != NULL) {
			putStack( t->node_id);
			bf = (*(bufnode+t->node_id))->next ; 
			if (bf != NULL){
				upleft_x = bf->x ;
				upleft_y = bf->y ;
				draw_point(fp,upleft_x,upleft_y,SOLID, GREEN) ;
				draw_wire(fp,(*(map+nd))->x, (*(map+nd))->y,upleft_x,upleft_y,SOLID, BLACK);
			}
			else {
				draw_point(fp,t->x,t->y,SOLID, BLACK) ;				
				draw_wire(fp,(*(map+nd))->x, (*(map+nd))->y,t->x,t->y,SOLID, BLACK);
			}
			while( bf != NULL){
				if ( bf->next != NULL){
					upleft_x = bf->next->x ;
					upleft_y = bf->next->y ;					
					draw_point(fp,upleft_x,upleft_y,SOLID, GREEN) ;
					draw_wire(fp,bf->x, bf->y,upleft_x,upleft_y,SOLID, BLACK);
				}
				else {
					draw_point(fp,t->x,t->y,SOLID, BLACK) ;				
					draw_wire(fp,bf->x, bf->y,t->x,t->y,SOLID, BLACK);
				}
				bf = bf->next ;
			}

		}


	}


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








void Enhance_Buf(DME_TREE_NODE *Troot,  DME_TREE_NODE ** map, int totalnum){
int i ; 
BUF_NODE * bf; 
	for ( i = 1 ; i < totalnum ; i++){
		if ( (*(map+i))->is_sink != 1 ){
			bf = (*(bufnode + i))->next ; 
			if (bf == NULL)
				continue;
//			while( bf ->next != NULL)
//				bf = bf ->next ; 
//			bf->next = (BUF_NODE *)malloc(sizeof(BUF_NODE));
//			bf->next->buf_id = bf->buf_id; 
			buf_a ++;
			buf_a ++;
//			bf->next->x = bf->x ; 
//			bf->next->y = bf->y;
//			bf->next->next = NULL ;
		}

	}




}

void Buffer_Placement (DME_TREE_NODE * Troot, int K, double * saa, int lnum, int totalnum, DME_TREE_NODE ** DME_tree_map){
int ii, flag; 
int j; 
int minNode ; 
double Lmin ; 
double stepv; 
double * sbb = (double *) malloc(sizeof(double) * lnum ); 
BUF_POS bp ; 
BUF_POS bm ; 

FIFO1 = (BUF_POS *) malloc ( sizeof(BUF_POS) * totalnum);
FIFO2 = (BUF_POS *) malloc ( sizeof(BUF_POS) * totalnum);



	bp.altitude = 0.0 ; 
	bp.x = 0.0 ;
	bp.y = 0.0 ;
	bp.prev = 0 ; 
	bp.next = 0 ;
	bp.isit = 1 ;
	putFIFO( &bp , totalnum ) ; 
	poreFIFO();
	for (ii = 0 ; ii < K ; ii++){
	//	while ( poreFIFO() ){
			cpy_double(saa,sbb,lnum); 
			while ( popFIFO(&bp)  ){
				printf(" bp altitude is %f\n", bp.altitude);
				while ( (minNode = find_shortest_path(bp.next,Troot,DME_tree_map,sbb) ) != -1){
					for ( j = 0 ; j < lnum ; j++)
						printf(" sbb %d is %f\n", j, sbb[j]);
					
					//if (sbb[minNode] >= MAX_DOUBLE) break;
					Lmin = saa[minNode] - bp.altitude ; 
				
					stepv = Lmin/(double)(K-ii+1) ;
					printf("stepv is %f\n", stepv);
					//sleep(3);
					// find a buffer insertion point during the shortest path 
					// always shield the explored path 
					flag = find_one_buf_pos(&bp, minNode, Troot, DME_tree_map,&bm, stepv,sbb) ; 

					if (flag == 1 ) {
						putFIFO( &bm , totalnum ) ; 
						
						printf("******buffer insertion %f %f *************\n", bm.x , bm.y) ; 
						// do some thing for bn (BUF_NODE) 
						
					}
						

				}				

			}
				printf(" findex 2 is %d\n", Findex2);
			if ( !poreFIFO() ){printf("here out\n"); break;}
	//	}



	}

	Enhance_Buf(Troot, DME_tree_map,totalnum);


	num_node = buf_i -  sink.num; 
	num_buffer = (buf_i+buf_a-totalnum+1)/2; 
	num_wire = totalnum-1+(buf_i-totalnum+1)/2; 
	num_sinknode = sink.num ; 
	num_total_nodes  = totalnum ;




return ;


}




int check_DME(DME_NODE * af ) {
static int i = 0 ;
static int blk_index = 0; 
	if ( af == NULL) return ; 
	printf("%d\t", i++);
	printf("(%f, %f)\n", af->select_x, af->select_y);
	if ( af ->blockage_node == 1 ) 
		blk_index ++ ; 
	check_DME(af -> pleft) ; 
	check_DME(af -> pright) ; 
	return blk_index ; 

}


void Out_put_files_coordinate(FILE *fp, BUF_NODE ** bufnode1, DME_TREE_NODE * Troot){
BUF_NODE * b ; 
static int i; 
	if (Troot == NULL || Troot->ls == NULL )
		return ; 
	if (Troot->node_id != 0)
		fprintf(fp, "%d %d %d \n", Troot->node_id, (int)Troot->x, (int)Troot->y);
	if (Troot->ls != NULL){
		b = (*(bufnode1 + Troot->ls->node_id))->next ;
		while( b != NULL){
			fprintf(fp,"%d %d %d\n", b->buf_id, (int)b->x,(int)b->y);
			fprintf(fp,"%d %d %d\n", (b->buf_id)+1, (int)b->x, (int)b->y);
			b = b->next ; 
		}
	}
	if (Troot->rs != NULL){
		b = (*(bufnode1 + Troot->rs->node_id))->next ;
		while( b != NULL){
			fprintf(fp,"%d %d %d\n", b->buf_id,(int) b->x,(int) b->y);
			fprintf(fp,"%d %d %d\n", (b->buf_id)+1,(int) b->x,(int) b->y);
			b = b->next ; 
		}
	}
	Out_put_files_coordinate(fp,bufnode1,Troot->ls);
	Out_put_files_coordinate(fp,bufnode1,Troot->rs);


}



void Out_put_files_sink(FILE *fp, DME_TREE_NODE * Troot){
static int i; 
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
		b = (*(bufnode1 + Troot->ls->node_id))->next ;
		if ( b != NULL)
			fprintf(fp,"%d %d %d\n", Troot->node_id, b->buf_id,0);
		else 
			fprintf(fp,"%d %d %d\n", Troot->node_id, Troot->ls->node_id,0);
		
		while( b != NULL){
		//	printf("%d %d %d\n", b->buf_id, b->buf_id,0);
			if ( b->next != NULL)
				fprintf(fp,"%d %d %d\n", b->buf_id+1, b->next->buf_id,0);
			else 
				fprintf(fp,"%d %d %d\n", b->buf_id+1, Troot->ls->node_id,0);
			b = b->next ; 
		}
		
	}
	if (Troot->rs != NULL){
		b = (*(bufnode1 + Troot->rs->node_id))->next ;
		if ( b != NULL)
			fprintf(fp,"%d %d %d\n", Troot->node_id, b->buf_id,0);
		else 
			fprintf(fp,"%d %d %d\n", Troot->node_id, Troot->rs->node_id,0);

		while( b != NULL){
		//	printf("%d %d %d\n", b->buf_id, b->buf_id,0);
			if ( b->next != NULL)
				fprintf(fp,"%d %d %d\n", b->buf_id+1, b->next->buf_id,0);
			else 
				fprintf(fp,"%d %d %d\n", b->buf_id+1, Troot->rs->node_id,0);
			b = b->next ; 
		}
		
	}
	

	Out_put_files_connect(fp,bufnode1,Troot->ls);
	Out_put_files_connect(fp,bufnode1,Troot->rs);


}


void Out_put_files_buf(FILE * fp, int lnum,   BUF_NODE ** bufnode1 ,  DME_TREE_NODE ** map){
BUF_NODE * b ; 
int i ;
	for ( i = 1; i < lnum ; i++){
		b = (*(bufnode1 + i))->next ; 
		while ( b != NULL){
			fprintf(fp,"%d %d %d\n", b->buf_id, b->buf_id+1,0);
			if ( (*(map+i))->is_sink != 1 && b->next == NULL )
				fprintf(fp,"%d %d %d\n", b->buf_id, b->buf_id+1,0);
			b = b->next ;
		}

	}		


}



void insert_buffer(DME_NODE * L  , BUF_NODE *** OBUF, DME_TREE_NODE ** OT , DME_TREE_NODE *** OTmap){
int i, K;
int num_blknode ; 	
int num_totalnode ; 
DME_TREE_NODE * Tr = NULL ; 
double *sink_alt_array = (double *) malloc(sizeof(double) * sink.num); 
double max_path  = 0.0 ; 
BUF_NODE * bf; 
DME_TREE_NODE ** DME_tree_map1 ; 



		num_blknode = check_DME( L+ 2 * sink.num -2 ) ; 
		printf(" num_blk node is %d\n", num_blknode ) ; 
		num_totalnode = 2 * sink.num  + num_blknode; 

		buf_i = num_totalnode -1 ; 
		bufnode = (BUF_NODE **) malloc (sizeof (BUF_NODE *) *(num_totalnode )) ; 
		DME_tree_map1 = ( DME_TREE_NODE **) malloc (sizeof(DME_TREE_NODE *) * (num_totalnode)); 		
		for ( i = 0 ; i <num_totalnode ; i++){
			(*(bufnode+i)) = (BUF_NODE *) malloc (sizeof(BUF_NODE));
			(*(bufnode+i))-> x = (*(bufnode+i))-> y = -1; 
			(*(bufnode+i))->buf_id = -1 ; 
			(*(bufnode+i)) -> next = NULL; 
		}


		construct_DME_tree( L, sink.num  ,  &Tr) ;
		settle_DME_tree(Tr, 0.0 ); 
		construct_SINK_array( Tr , sink_alt_array); 
	
		check_DME_tree(Tr) ;
		construct_DME_map(Tr, DME_tree_map1); 
		check_sink_alt_array(sink_alt_array,sink.num);
	
		max_path = max_sink_alt(sink_alt_array,sink.num); 
	
		K = ceil( max_path / LAMBADA )+1 ; 
		if ( !(K%2) ) K += 1 ; 
		Buffer_Placement (Tr, K, sink_alt_array, sink.num, num_totalnode, DME_tree_map1) ; 
		(*OBUF) = bufnode ; 
		(*OT) = Tr ; 
		(*OTmap) = DME_tree_map1 ; 





}






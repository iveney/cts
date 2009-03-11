#include"crt_path.h"


void refine_crt_num(int * node, int * wire, int * buf, BUF_NODE ** OBUF){
int i = 0;
BUF_NODE * bf = (*(OBUF + 1))->next;
*buf = 0;
	while ( bf != NULL){
		i++;
		(*buf) += bf->units;
		bf = bf->next;
	}

//	*buf = i*6;
	*node = 2 * i;
	*wire = i ;
}


void cpy_buf_list( BUF_NODE * L1, BUF_NODE * L2){
	if ( L2 == NULL)
			return;
	if ( L1 == NULL){
		printf(" error copy , no list 1\n");
	}
	L1->x = L2->x;
	L1->y = L2->y;
	L1->buf_id = L2->buf_id;
	L1->buf_type = L2->buf_type;
	L1->units = L2->units;
	L1->wire_type = L2->wire_type;;
//	L1 = L1 -> next;
	L2 = L2 -> next;
	while ( L2 != NULL){
		L1->next = (BUF_NODE *) malloc(sizeof(BUF_NODE));
		L1 = L1->next;
		L1->x = L2->x;
		L1->y = L2->y;
		L1->buf_id = L2->buf_id;
		L1->buf_type = L2->buf_type;
		L1->units = L2->units;
		L1->wire_type = L2->wire_type;;
//		L1 = L1 -> next;
		L2 = L2 -> next;
	}
}
void refine_crt_path(BUF_NODE ** OBUF, DME_TREE_NODE * OT, DME_TREE_NODE ** OTmap, int n, int w, int bu){
DME_TREE_NODE * lookson;
DME_TREE_NODE * Vroot = OT;
DME_TREE_NODE * Rroot = OT->ls;
DME_TREE_NODE * Oroot = (*OTmap);
BUF_NODE * store;
BUF_NODE * buflist;
BUF_NODE * st;
BUF_NODE * ex;
int succ = -1 ;
int head = 0 ;
int tail = 0 ;
int segnum = 2;
int * li_path_info;
int	InterNum = Rroot->buf_num;
int i;
int bcode;
int btype;
int bnum;
	ZERO->node_id = 0;
	ZERO->x = 0;
	ZERO->y = 0;
	ZERO->sink_index = -1;
    ZERO->ls = ZERO->rs = NULL;
/*	if( Rroot->node_id != 1){
		printf(" wrong, there is blk node this test \n");
		exit(0);
	}
*/

	lookson = OT->ls;
	while(lookson->is_blk == 1)
		lookson = lookson->ls;
	Rroot = lookson;
	buflist = (*(OBUF+Rroot->node_id))->next;
	store = (BUF_NODE *) malloc(sizeof(BUF_NODE));
	cpy_buf_list(store,buflist);
	st = store;
	while ( st != NULL){
		printf("*******************************\n");
		if(st->next != NULL)
			segnum++;
		else if( ((int) st->x) == Rroot->x && ((int) st->y) == Rroot->y)
			head = 1;
		else
			segnum++;
		st = st->next;
	}
	printf(" seg num is %d\n", segnum);
	printf(" head is %d\t tail is %d\n",head,tail);
	st = store;
	while (st != NULL){
		printf("(%d %d)\t",(int)st->x,(int)st->y);
		st = st->next;
	}
//	exit(0);
	succ = critical_path(head,tail,segnum,&li_path_info,buflist,OBUF,Rroot,OTmap);
	for(i=segnum-1;i>=0;i--){
		printf("wire type is %d\n",li_path_info[2*i+1]);
		bcode = li_path_info[2*i];
		decode(bcode,&btype,&bnum);
		printf("bcode is %d bnum is %d btype is %d\n",bcode,bnum,btype);
	}


	(*OTmap) = Oroot;
	Rroot->fa = Oroot;
	(*(OBUF+Rroot->node_id))->next = store;
	st = store;
	ex = st;
	if ( succ > 0 ){
		for( i=segnum-2; i>=0; i--){
			if( i == 0){
				Rroot->wire_type = li_path_info[2*i+1];
				if (head == 0)
					continue;
			}
			bcode = li_path_info[2*i];
			decode(bcode,&btype,&bnum);
			if(btype > -1 && bnum > -1){
				st->buf_type = btype;
				st->units = bnum;
				st->wire_type = li_path_info[2*i+1];
				ex = st;
				st = st->next;
				num_node += 2;
				num_wire ++;
				total_buf_num += bnum;
			}
			else{
				ex->next = st->next;
				free(st);
				st = ex->next;
			}

		}


	}
	else {
		num_node += n;
		num_wire += w;
		total_buf_num += bu;
		printf("refine critical path by DP failed: recover\n");
	}

	return;

}

void quick_find(int head,int total_seg_num,int segnum,BUF_NODE *store, int * x, int * y,int *b){
int full = total_seg_num - 2;
	full += head;
int passed = full - segnum ;
BUF_NODE * st = store;
//	passed ++;
	if ( passed == 0){
		*x = 0;
		*y = 0;
		*b = -1;
		return;
	}
	while(passed --){
//		printf("pass is %d\n",passed);
		if ( st == NULL ){

			printf("quick found wrong, no such passed number\n");
			exit(0);
		}
		*x = (int) st->x;
		*y = (int) st->y;
		*b = st->buf_id;
		st = st->next;
	}

	return;


}

void decode(int bcode, int * btype, int *bnum){
	if ( bcode < 0){
		*btype = -1;
		*bnum = -1;
	}
	else {
		*bnum = bcode ;
//		*btype = bcode %2 ;
		*btype = 0;
	}
}

int partial_path_CLR(int total_seg_num,int segnum, int * path_info, BUF_NODE * store, int head, BUF_NODE ** OBUF, DME_TREE_NODE * Rroot, DME_TREE_NODE ** Tmap,double * delay){
int err;
int i;
int x,y;
int wtype=-1;
int btype=-1;
int bnum=-1;
int b_id=-1;
int bcode;
double tempqian;
FILE * tfp;
FILE * ifp;
DME_TREE_NODE * Vroot = (DME_TREE_NODE *) malloc(sizeof(DME_TREE_NODE));
//DME_TREE_NODE * Rroot = (DME_TREE_NODE *) malloc(sizeof(DME_TREE_NODE));
BUF_NODE * newbuf = (BUF_NODE *) malloc(sizeof(BUF_NODE));
		   newbuf->next = NULL;
BUF_NODE * bf = newbuf;
BUF_NODE * b1, *b2;

new_num_node = 0;
new_num_wire = 0;
new_total_buf_num = 0 ;
new_num_total_nodes = 0;
printf("total segment is %d\n", total_seg_num);
printf("now is %d\n",segnum);
Vroot->x = 0;
Vroot->y = 0;
Vroot->node_id = 0;
	for(i=segnum ; i>=0 ; i--){
		if ( i  == total_seg_num-1){
			Vroot->x = 0;
			Vroot->y = 0;
			Vroot->node_id = LARGESTNODE;
			Vroot->wire_type = 1;
			Vroot->sink_index = -1;
			Vroot->ls = Vroot->rs = NULL;
			new_num_node++;
			new_num_wire++;
			continue;
		}
		if ( i == 0 ){
			Rroot->wire_type = path_info[1];
			if (head == 0)
				continue;
		}
		quick_find(head,total_seg_num,i,store,&x,&y,&b_id);
		if ( i== segnum){
			Vroot->node_id = LARGESTNODE;
			Vroot->x = x;
			Vroot->y = y;
			Vroot->wire_type = 1;
			Vroot->ls = Vroot->rs = NULL;
			new_num_node ++;
			new_num_wire ++;
//			new_num_total_nodes++;
			Vroot->sink_index = -1;
//			continue;
		}

		if ( i == 0 ){
			Rroot->wire_type = path_info[1];
		}
		bcode = path_info[2*i];
		printf("i is %d\n", i);
		printf("bcode is %d\n", bcode);
		decode(bcode,&btype,&bnum);
		printf("bnum is %d\n",bnum);
	//	sleep(1);
		if ( bnum > -1 && btype > -1){
			if (bf->next != NULL){
				printf("error: why not move to the last one \n");
				exit(0);
			}
			new_num_node += 2;
			new_num_wire ++;
			new_total_buf_num+= bnum;
			bf->next = (BUF_NODE *) malloc(sizeof(BUF_NODE));
			bf = bf->next;
			bf->x = (double)x;
			bf->y = (double)y;
			if (b_id == -1){
				printf("error:this should not be buffer\n");
				exit(0);
			}
			bf->buf_id = b_id;
			bf->wire_type = path_info[2*i+1];
			bf->buf_type = btype;
			bf->units = bnum;
			bf->next = NULL;
		}



	}
	//Vroot->ls = Rroot;
	//Rroot->fa = Vroot;
	if(Vroot->node_id != 0){
		ZERO->ls = Vroot;
		ZERO->rs = NULL;
		Vroot->fa = ZERO;
		Vroot->ls = Rroot;
		Rroot->fa = Vroot;
	}
	else {
		ZERO->ls = Rroot;
		ZERO->rs = NULL;
		Rroot->fa = ZERO;

	}
	printf("root is %d\n",Rroot->node_id);
	tfp = fopen("crtoutfile","w");
	ifp = fopen("crtinfile","w");
	num_node+=new_num_node ;
	num_wire+=new_num_wire ;
	total_buf_num+=new_total_buf_num;
	num_total_nodes+=new_num_total_nodes;
	(*(OBUF+Rroot->node_id))->next = newbuf->next;
	(*Tmap) = ZERO;
	gen_inputfile(ifp,ZERO,Tmap,1);
	output_file(tfp,OBUF,ZERO,Tmap,0);
	fclose(tfp);
	fclose(ifp);

	//(*(OBUF+Rroot->node_id))->next = NULL;

	num_node-=new_num_node ;
	num_wire-=new_num_wire ;
	total_buf_num-=new_total_buf_num;
	num_total_nodes-=new_num_total_nodes;
	ZERO->ls = NULL;
	b1 = newbuf;
	while ( b1 != NULL ){
		b2 = b1->next;
		free(b1);
		b1 = b2;
	 }
		tempqian = 0.0;
		if(segnum == total_seg_num -1)
			err = crt_process(1, &tempqian);
		else
			err = crt_process(4,&tempqian);
		printf("\n");
		printf(" err is %d\n",err);
		printf(" qian %f\n",tempqian);
		printf("\n");
		*delay = tempqian;
		return err;
}

void add_buf_list(BUF_NODE * ol , BUF_NODE *ne){
BUF_NODE * bi;
	if (ne == NULL)
		return;
	bi = ol;
	if (bi == NULL){
		printf(" error: the newlist not exist\n");
		exit(0);
	}
	while ( bi->next != NULL)
		bi = bi->next;
	bi->next = ne;
}

void adjust_node_id(DME_TREE_NODE * OT, int loss){
     if ( OT == NULL )
		 return;
	 if (OT->node_id != 0)
		 OT->node_id = OT->node_id - loss;
	 adjust_node_id(OT->ls,loss);
	 adjust_node_id(OT->rs,loss);
}

void trans_crt_path(BUF_NODE ** OBUF, DME_TREE_NODE * OT, DME_TREE_NODE ** OTmap){
DME_TREE_NODE * seg = OT->ls;
BUF_NODE * buflist , * newlist;
BUF_NODE * bf,* bt;
int i;
int nodenum = 0;
	if( seg -> is_blk != 1)
		return;
	newlist = (BUF_NODE *)malloc(sizeof(BUF_NODE));
	newlist->next = NULL;
	//bf = newlist;
	while (seg != NULL){
		add_buf_list(newlist , (*(OBUF+seg->node_id))->next);


		if(seg->is_blk != 1){
			(*(OBUF+seg->node_id))->next = newlist->next;
			OT->ls = seg;
			seg->fa = OT;
			break;
		}
		nodenum ++;
		seg = seg->ls ;
	}
	num_node -= nodenum;
	num_wire -= nodenum;
	num_total_nodes -= nodenum;
	adjust_node_id(OT,nodenum);
	printf("nodenum is %d\n",nodenum);
	for(i=1;i<=num_total_nodes-1;i++){
		(*(OBUF+i))->next = (*(OBUF+i+nodenum))->next;
		(*(OTmap+i)) = (*(OTmap+i+nodenum));

	}
}


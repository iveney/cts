#include "ds.h"
#include "stdio.h"
#include "stdlib.h"
#include "connect.h"
#include "bufplace.h"
#include "segment.h"
#include<math.h>
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
#define CAP_LIMIT 40000
#define Absdist(x1,x2) ((x1)-(x2))>=0?((x1)-(x2)):((x2)-(x1))
static int blockage_count = 0;
extern DME_NODE * source_node;
extern double level_length[20];
//TODO no blockage between them

extern BOX frame;
extern LAMBADA3;
extern int HIGHWAY;
extern SOURCE source  ;
extern SINK sink;
extern double cap_left;
extern WIRELIB wirelib;
extern BUFLIB	buflib ;
extern VDDLIB	vddlib ;
extern int SlewLimit;
extern int CapLimit;
extern BLOCKAGE blockage;
//extern int which_level;
extern int g_size;
extern int free_buf_num;
extern int mid_sink_num;
extern int sink_array_num;
void segment_merged(DME_NODE *n, BOOL first, NODE A, NODE B){
	NODE s,t;
	if(n->x1 == n->x2)
		return;
	s.x = n->x1;
	s.y = n->y1;
	t.x = n->x2;
	t.y = n->y2;
	//printf("%d %d %d %d %d\n", n->x1,n->y1,n->x2,n->y2,first);
	//printf("%d %d %d %d\n", A.x,A.y,B.x,B.y);
	if(A.x > B.x)
		exit(0);
	if(first == TRUE)
		compute_segment(&s,&t,&blockage,blockage.num,A,B);
	else
		compute_segment(&t,&s,&blockage,blockage.num,A,B);
	if(s.x<t.x){
		n->x1 = s.x;
		n->y1 = s.y;
		n->x2 = t.x;
		n->y2 = t.y;
	}
	else{
		n->x1 = t.x;
		n->y1 = t.y;
		n->x2 = s.x;
		n->y2 = s.y;
	}
	n->lower = n->x1 + n->y1;
	n->left = n->x1 - n->y1;
	n->upper = n->x2 + n->y2;
	n->right = n->x2 - n->y2;
	//printf("%d %d %d %d\n", n->x1,n->y1,n->x2,n->y2);

}

BOOL overlapped(DME_NODE *n1, DME_NODE *n2){
//	return FALSE;
	int llx_1, lly_1,urx_1, ury_1;
	int llx_2, lly_2,urx_2, ury_2;
	if(n1->x1 > n1->x2){
		llx_1 = n1->x2;
		urx_1 = n1->x1;
	}else{
		llx_1 = n1->x1;
		urx_1 = n1->x2;
	}
	if(n1->y1 > n1->y2){
		lly_1 = n1->y2;
		ury_1 = n1->y1;
	}else{
		lly_1 = n1->y1;
		ury_1 = n1->y2;
	}

	if(n2->x1 > n2->x2){
		llx_2 = n2->x2;
		urx_2 = n2->x1;
	}else{
		llx_2 = n2->x1;
		urx_2 = n2->x2;
	}
	if(n2->y1 > n2->y2){
		lly_2 = n2->y2;
		ury_2 = n2->y1;
	}else{
		lly_2 = n2->y1;
		ury_2 = n2->y2;
	}
	int minx = MAX(llx_1, llx_2);
	int miny = MAX(lly_1, lly_2);
	int maxx = MIN(urx_1, urx_2);
	int maxy = MIN(ury_1, ury_2);
	if(minx > maxx || miny > maxy)
		return FALSE;
	else
//		printf("dfsadf\n");
//		exit(0);
		return TRUE;
}

void find_path(int min_i, int min_j, NODE *p, int * length){
	int index = min_j;
	int len = 0;
	p[len].x=g_node[min_j].x;
	p[len].y=g_node[min_j].y;
	//p[len].dir_to = dirs
	len++;
	DIRECTION a2b,b2a;
	int j = index;
	while((index = parents[min_i][index]) != (min_i)){
		// ll=0, lr=1, ur=2, ul=3
		int k = index;
		a2b = dirs[j][k];
		b2a = dirs[k][j];
		j=k;
		p[len] = g_node[index];
		p[len-1].dir_to = a2b;
		p[len-1].dir_from = b2a;
//   		printf("test direction: (%d %d),(%d %d), %d %d\n",p[len-1].x,p[len-1].y,p[len].x,p[len].y,a2b,b2a);
		len++;
	}
	p[len-1].dir_to = dirs[j][min_i];
	p[len-1].dir_from = dirs[min_i][j];

	p[len].x=g_node[min_i].x;
	p[len].y=g_node[min_i].y;
//   	printf("test direction: (%d %d),(%d %d), %d %d %d\n",p[len-1].x,p[len-1].y,p[len].x,p[len].y,p[len-1].dir_to ,p[len-1].dir_from,len);
	len++;
	*length = len;
// 	printf("length is  %d\n",*length);
// 	printf("\n");

//	return (int)shortest[g_size-1];
}

int cal_distance(DME_NODE * n1, DME_NODE * n2){

	int dd1, dd2, dd3, dd4;
	dd1 = Manhattan(n1->x1, n1->y1, n2->x1, n2->y1);
	dd2 = Manhattan(n1->x1, n1->y1, n2->x2, n2->y2);
	dd3 = Manhattan(n1->x2, n1->y2, n2->x1, n2->y1);
	dd4 = Manhattan(n1->x2, n1->y2, n2->x2, n2->y2);
// 	printf("%d %d %d %d\n",dd1,dd2,dd3,dd4);
	int t1, t2,t_min;
	t1 = MIN(dd1, dd2);
	t2 = MIN(dd3, dd4);
	t_min = MIN(t1,t2);
	return t_min;
}


DME_NODE * init_dme_blockage_node(NODE p, int wtype, int buf_u){
	DME_NODE *n = (DME_NODE *) malloc (sizeof(DME_NODE));
	n->blockage_node = 1;
	n->duplicate_first_buf = 0;
	n->buf_num = 0;
	n->sink_index = -1;
	n->x1 = n->x2 = p.x;
	n->y1 = n->y2 = p.y;
	n->select_x = n->x1;
	n->select_y = n->y1;
	n->buf_unit = buf_u;
	n->sx = n->lower = n->upper = n->x1 + n->y1;
	n->sy = n->left = n->right = n->x1 - n->y1;
	n->pright=NULL;
	n->wire_type = wtype;
//	printf("blockage node init. %d. (%d %d)\n",blockage_count, n->select_x, n->select_y);
//	sleep(1);
	//exit(0);
	n->node_id = 777;
	n->node_id = blockage_count++;
	return n;
}

BOOL point_in_path(int x1,int y1, NODE s, NODE t){
// 	return TRUE;
	switch(s.dir_to){
		case LEFT:
			if(y1 == s.y && (t.x<=x1 && x1<=s.x))
				return TRUE;
			break;
		case RIGHT:
			if(y1 == s.y && (s.x<=x1 && x1<=t.x))
				return TRUE;
			break;
		case DOWN:
			if(x1 == s.x && (t.y<=y1 && y1<=s.y))
				return TRUE;
			break;
		case UP:
			if(x1 == s.x && (s.y<=y1 && y1<=t.y))
				return TRUE;
			break;
	}
	switch(s.dir_from){
		case LEFT:
			if(y1 == t.y && (s.x<=x1 && x1<=t.x))
				return TRUE;
			break;
		case RIGHT:
			if(y1 == t.y && (t.x<=x1 && x1<=s.x))
				return TRUE;
			break;
		case DOWN:
			if(x1 == t.x && (s.y<=y1 && y1<=t.y))
				return TRUE;
			break;
		case UP:
			if(x1 == t.x && (t.y<=y1 && y1<=s.y))
				return TRUE;
			break;
	}
	return FALSE;
}

int ballance_point(DME_NODE * n1, DME_NODE * n2, DME_NODE * parent, int distance, double delta_delay, int wire_type, BOOL need_detour, int buf_unit){
	int total_distance = distance;
	double alpha = wirelib.lib[wire_type].r;
	double beta = wirelib.lib[wire_type].c;

	double LAMBADA;
	if(wire_type == 0)
		LAMBADA = LAMBADA_w1;
	else
		LAMBADA = LAMBADA_w2;
	double delay_buf = 5.1f;
	double buf_input_cap = buflib.lib[0].icap;
	double buf_output_cap = buflib.lib[0].ocap;
	//buf_output_cap = 0;
	double buf_output_r = buflib.lib[0].ores;
	double delay1 = n1->to_sink_delay;
	double delay2 = n2->to_sink_delay;
	int step1 = LAMBADA * n1->first_buf_fraction;
	int step2 = LAMBADA * n2->first_buf_fraction;
	double cap_1 = n1->capacitance;
	double cap_2 = n2->capacitance;
	int bb1,bb2;
	int length;
	double sigma;
	double t_delay;
	int b1,b2;
	double factor1 = 1.0;
	double factor2 = 1.0;
	int flag = 0;
//	int reduntant1=0,reduntant2 = 0;
	int detour = 0;
	int detour1 = 0;
	int detour2 = 0;
	double gamma1,gamma2;
	length = distance;
	if(ABS(delay2 - delay1-delta_delay) < -2.0f)
		bb1 = (int)(length*(cap_2 + 0.5*beta*length)/(cap_1 + cap_2 + beta*length));
	else
		bb1 = (int)(( 1000*(delay2 - delay1-delta_delay) + alpha*length*(cap_2 + 0.5*beta*length))/alpha/(cap_1 + cap_2 + beta*length));

	bb2 = (int)(length - bb1);


	if(bb1<0 || bb2<0){
//		printf("%d %d < 0!\n", bb1, bb2);
//		return -1;
	//	exit(0);
	}
 //	printf("delay is %f %f ; %f %f %d %d %d\n",delay1,delay2, cap_1, cap_2, distance, bb1,bb2);
	if(bb1>bb2)
			flag = 1;
	while(1){

		int i,j;
		int sum;

		for(sum=0;sum<50;sum++)
			for(i=0;i<=sum;i++){
				j = sum - i;
				if((i+j+n1->downstream_buf_num+n2->downstream_buf_num)%2 == 1)
			//	if(i+n1->downstream_buf_num != j+n2->downstream_buf_num)
					continue;

				length = distance + detour;
				switch(i){
					case 0:
						delay1 =  n1->to_sink_delay;
						cap_1 = n1->capacitance;
						b1=0;
						break;
					case 1:
						step1 = (int)((LAMBADA * n1->first_buf_fraction)*factor1);
						length -= step1;
						cap_1 = buf_input_cap;
						delay_buf = buf_output_r*(buf_output_cap+n1->capacitance+step1*beta)*0.001;
//						int d_num = (int)((n1->capacitance+step1*beta)/(LAMBADA*beta+buf_input_cap));

						delay1 =  n1->to_sink_delay + (alpha*beta/2*step1*step1+alpha*step1*n1->capacitance)*0.001+delay_buf;
						b1=step1;
						break;
					default:
						step1 = (int)((LAMBADA * n1->first_buf_fraction)*factor1);
						length -= step1;
						b1=step1;
 						delay_buf = buf_output_r*(buf_output_cap+n1->capacitance+step1*beta)*0.001;
						if(n1->duplicate_first_buf == 1){
							delay_buf = buf_output_r/2*(buf_output_cap*2+n1->capacitance+step1*beta)*0.001;
						}

						delay1 =  n1->to_sink_delay + (alpha*beta/2*step1*step1+alpha*step1*n1->capacitance)*0.001+delay_buf;
						step1 = (int)(LAMBADA*factor1);
						b1+=(i-2)*step1;
						length -= (i-2)*step1;
						delay_buf = buf_output_r*(buf_output_cap+buf_input_cap+step1*beta)*0.001;
						delay1 += (i-2)*(alpha*beta/2*step1*step1+alpha*step1*buf_input_cap)*0.001 + (i-2)*delay_buf;
						step1 = (int)(LAMBADA*factor1);
						b1 += step1;
						length -= step1;
						delay_buf = buf_output_r*(buf_output_cap+buf_input_cap+step1*beta)*0.001;
						delay1 += (alpha*beta/2*step1*step1+alpha*step1*buf_input_cap)*0.001+delay_buf;
						cap_1 = buf_input_cap;

						break;
				}
				switch(j){
					case 0:
						delay2 =  n2->to_sink_delay;
						cap_2 = n2->capacitance;
						b2=0;
						break;
					case 1:
						step2 = (int)((LAMBADA * n2->first_buf_fraction)*factor2);
						length -= step2;
						b2=step2;
						cap_2 = buf_input_cap;
						delay_buf = buf_output_r*(buf_output_cap+n2->capacitance+step2*beta)*0.001;
						delay2 =  n2->to_sink_delay + (alpha*beta/2*step2*step2+alpha*step2*n2->capacitance)*0.001+delay_buf;

/* 						if(n2->capacitance>CAP_LIMIT)
						if(n2->sink_index == -1 && n2->blockage_node == 0)
							cap_2 += buf_input_cap;*/
						break;
					default:
						step2 = (int)((LAMBADA * n2->first_buf_fraction)*factor2);
						length -= step2;
						b2=step2;
 						delay_buf = buf_output_r*(buf_output_cap+n2->capacitance+step2*beta)*0.001;
// 						printf("xxxxxxxxx delay is %f\n",delay_buf);
						delay2 =  n2->to_sink_delay + (alpha*beta/2*step1*step2+alpha*step2*n2->capacitance)*0.001+delay_buf;
						step2 = (int)(LAMBADA*factor2);
						length -= (j-2)*step2;
						b2+=(j-2)*step2;
						delay_buf = buf_output_r*(buf_output_cap+buf_input_cap+step2*beta)*0.001;
						cap_2 = buf_input_cap;
						delay2 += (j-2)*(alpha*beta/2*step2*step2+alpha*step2*buf_input_cap)*0.001 + (j-2)*delay_buf;
// 						if(n2->capacitance>CAP_LIMIT)
/*						if(n2->sink_index == -1 && n2->blockage_node == 0)
							delay2 += (alpha*step2*buf_input_cap)*0.001;*/
						step2 = (int)(LAMBADA*factor2);
						b2 += step2;
						length -= step2;
						delay_buf = buf_output_r*(buf_output_cap+buf_input_cap+step2*beta)*0.001;
						delay2 += (alpha*beta/2*step2*step2+alpha*step2*buf_input_cap)*0.001+delay_buf;
						break;
				}


				if(length < 0)
					continue;
				if(ABS(delay2 - delay1-delta_delay) < -2.0f)
					bb1 = (int)(length*(cap_2 + 0.5*beta*length)/(cap_1 + cap_2 + beta*length));
				else
					bb1 = (int)(( 1000*(delay2 - delay1-delta_delay) + alpha*length*(cap_2 + 0.5*beta*length))/alpha/(cap_1 + cap_2 + beta*length));
				bb2 = (int)(length - bb1);
/*				if(ABS(bb1-bb2) > 30000)
					continue;*/
				if(need_detour == TRUE && (bb1 < 100000 ||  bb2 < 100000))
					continue;
				if(bb1>=0 && ((i==0 && bb1<=(int)(LAMBADA*n1->first_buf_fraction)) || (i>0 && bb1<=LAMBADA)))
					if(bb2>=0 && ((j==0 && bb2<=(int)(LAMBADA*n2->first_buf_fraction)) || (j>0 && bb2<=LAMBADA))){
						b1 += bb1;
						b2 += bb2;
// 						printf("%d %d %d %d %d %d %d %f %f \n",bb1,bb2,b1,b2,i,j,distance,factor1,factor2);
// 						printf("delta %d %d\n",n1->delta_length_buf,n2->delta_length_buf);
// 						printf("delta %d %d\n",n1->first_buf_fraction,n2->first_buf_fraction);

						if((b2+b1) != distance+detour)
							printf("%d %d %d hhhhhhhhhhh\n",b1,b2,total_distance);

						int B1 = b1, B2 = b2;
						if(b1<distance){
							b2 = distance - b1;
							detour1 = 0;
							detour2 = B2 - b2;
						}else if(b2<distance){
							b1 = distance - b2;
							detour1 = B1 - b1;
							detour2 = 0;
						}else{
							b1 = (int)(1.0f*distance*B1/(B1+B2));
							b2 = distance - b1;
							detour1 = B1 - b1;
							detour2 = B2 - b2;
						}
						if(detour1 > 0 || detour2>0)
							printf("%d %d %d %d, B %d %d\tD1 is %f, D2 is %f\n", b1, b2, detour1, detour2,i,j,n1->to_sink_delay, n2->to_sink_delay);
						parent->left_length = b1 + detour1;
						parent->right_length = b2 + detour2;
						parent->capacitance = cap_1 + cap_2 + (bb1+bb2)*beta;
						parent->resistance = alpha*((double)bb1*bb2)/(bb1+bb2);
						parent->to_sink_delay = ((alpha*bb1*(0.5*beta*bb1+cap_1)*0.001 + delay1) + (alpha*bb2*(0.5*beta*bb2+cap_2)*0.001 + delay2))/2;
//  						printf("Parent delay is %f\n",parent->to_sink_delay);
// 						printf("delay 1 is %f, delay 2 is %f\n",delay1,delay2);
						int tmp1 = bb1,tmp2 = bb2;
// 						printf("%f %f %f\n",parent->to_sink_delay,n1->to_sink_delay,n2->to_sink_delay);

//						if(i==0)
//							tmp1 += n1->delta_length_buf;
//						if(j==0)
//							tmp2 += n2->delta_length_buf;

//						parent->delta_length_buf = MAX(tmp1,tmp2);
 						double tt = (LAMBADA * beta + buf_input_cap - parent->capacitance)/beta;
/*						double p_cap = 2*MAX(cap_1+bb1*beta, cap_2+bb2*beta);
 						int tt = (int)((LAMBADA * beta + buf_input_cap - p_cap)/beta);*/
 						if(tt<0){
 							tt = 0;
 		//					printf("NOOOO!\n");
 						}
 						parent->delta_length_buf = LAMBADA - tt;
 						parent->first_buf_fraction = tt/LAMBADA;
						parent->downstream_buf_num = MAX(n1->downstream_buf_num+i, n2->downstream_buf_num+j);
						parent->downstream_total_buf_num = n1->downstream_total_buf_num+i+n2->downstream_total_buf_num+j;

						n1->buf_num = i;
						n2->buf_num = j;
						n1->factor = factor1;
						n2->factor = factor2;
						n1->detour = detour1;
						n2->detour = detour2;
						if(b1 == 0)
							b1++;
						if(b1 == distance)
							b1--;
						return b1;
					}
			}
		if(flag == 1){
			factor1*=0.95;
			if(factor1 < 0.8){
				detour += 200000;
				factor1 = 1.0f;
// 				printf("detour 1 %d distance is %d\n",detour,distance);
			}
		}
		else{
			factor2 *= 0.95f;
			if(factor2 < 0.8){
				detour += 200000;
				factor2 = 1.0f;
// 				printf("detour 2 %d distance is %d\n",detour, distance);
			}
		}
	}

}

void merge(DME_NODE * n1, DME_NODE * n2, DME_NODE * parent, int distance, NODE *p, int len, double delta_delay, int wire_type, BOOL need_d, int buf_u){
/*	printf("%f %f %f %f\n",n1->lower,n1->upper,n1->left,n1->right);
	printf("%f %f %f %f\n",n2->lower,n2->upper,n2->left,n2->right);*/
	int b1,b2;
//	double alpha = wirelib.lib[wire_type].r;
//	double beta = wirelib.lib[wire_type].c;
	int k1, k2;
	n1->wire_type = wire_type;
	n2->wire_type = wire_type;

	if(ELMORE == 1){

		b1 = ballance_point(n1,n2,parent,distance,delta_delay,wire_type,need_d,buf_u);
		b2 = distance - b1;

	}else{
		parent->weight = (distance + n1->weight+n2->weight)/2;
		b1 = (int)((distance - (n1->weight-n2->weight))/2);
		b2 = (int)((distance + (n1->weight-n2->weight))/2);
	}

	if(n1->lower > n1->upper || n1->left > n1->right)
                printf("n1:error %d %d %d %d %d\n",n1->node_id,n1->x1,n1->y1,n1->x2,n1->y2);
        if(n2->lower > n2->upper || n2->left > n2->right)
                printf("n2:error %d %d %d %d %d\n",n2->node_id,n2->x1,n2->y1,n2->x2,n2->y2);


	if(b1<=0){
		printf("Oh No b1!\n");
		printf("%d %d %d %d %d\n", b1,b2,distance,n1->sink_index,n2->sink_index);
		b1 = 0;
		b2 = distance;
		//return;
	}
	if(b2<=0){
		printf("Oh No b1!\n");
		printf("%d %d %d %d %d\n", b1,b2,distance,n1->sink_index,n2->sink_index);
		b2 = 0;
		b1 = distance;
		//return;
	}

	if(len == 2){
		if(overlapped(n1,n2) == FALSE){
			parent->lower = MAX(n1->lower - b1, n2->lower - b2);
			parent->upper = MIN(n1->upper + b1, n2->upper + b2);
			parent->left = MAX(n1->left - b1, n2->left - b2);
			parent->right = MIN(n1->right + b1, n2->right + b2);
			parent->x1 = (parent->lower + parent->left)/2;
			parent->y1 = (parent->lower - parent->left)/2;
			parent->x2 = (parent->upper + parent->right)/2;
			parent->y2 = (parent->upper - parent->right)/2;
			n1->is_select = FALSE;
			n2->is_select = FALSE;
			//parent->is_selected = FALSE;
		}else{
       	 		int lower1 = p[0].x + p[0].y;
        		int left1 = p[0].x - p[0].y;
        		int lower2 = p[1].x + p[1].y;
        		int left2 = p[1].x - p[1].y;

        		parent->lower = MAX(lower1 - b1, lower2 - b2);
        		parent->upper = MIN(lower1 + b1, lower2 + b2);
        		parent->left = MAX(left1 - b1, left2 - b2);
        		parent->right = MIN(left1 + b1, left2 + b2);

        		parent->x1 = (parent->lower + parent->left)/2;
        		parent->y1 = (parent->lower - parent->left)/2;
       		 	parent->x2 = (parent->upper + parent->right)/2;
       		 	parent->y2 = (parent->upper - parent->right)/2;
			n1->select_x = p[0].x;
			n1->select_y = p[0].y;
			n1->sx = n1->select_x + n1->select_y;
			n1->sy = n1->select_x - n1->select_y;
			n2->select_x = p[1].x;
			n2->select_y = p[1].y;
			n2->sx = n2->select_x + n2->select_y;
			n2->sy = n2->select_x - n2->select_y;
			n1->is_select = TRUE;
			n2->is_select = TRUE;
			printf("here xiao\n");
//			exit(0);
		}
		if(point_in_path(parent->x1, parent->y1, p[0],p[1]) == TRUE){
			if(p[0].x > p[1].x)
				segment_merged(parent,TRUE,p[1],p[0]);
			else
				segment_merged(parent,TRUE,p[0],p[1]);

		}
		else{
			if(p[0].x > p[1].x)
				segment_merged(parent,FALSE,p[1],p[0]);
			else
				segment_merged(parent,FALSE,p[0],p[1]);
		}

		if((p[0].x == n1->x1 && p[0].y == n1->y1) || (p[0].x == n1->x2 && p[0].y == n1->y2)){
			parent->pleft = n1;
			parent->pright= n2;

		}else{
			if((p[1].x == n2->x1 && p[1].y == n2->y1) || (p[1].x == n2->x2 && p[1].y == n2->y2)){
				printf("fixed\n");
			}
			parent->pleft = n2;
			parent->pright= n1;
			printf("%d %d %d %d\n", p[1].x,p[1].y,p[0].x, p[0].y);
			exit(0);
		}
		parent->left_direction = p[0].dir_from;
		parent->right_direction = p[0].dir_to;
		parent->downstream_length = distance + n1->downstream_length + n2->downstream_length + n1->detour + n2->detour;
		parent->sink_index = -1;
		parent->blockage_node = 0;
//		parent->visited = 0;
//		n1->visited = 1;
//		n2->visited = 1;
//		n1->select_x = p[0].x;
//		n1->select_y = p[0].y;
//		n2->select_x = p[1].x;
//		n2->select_y = p[1].y;

		return;
	}
	int i,j,k;
	int bb;
	DME_NODE *head, *tail, *next;
	if((p[0].x == n1->x1 && p[0].y == n1->y1) || (p[0].x == n1->x2 && p[0].y == n1->y2)){
		head = n1;
		tail = n2;
	}
	else{printf("xiao xiao\n");exit(0);}
	head = n1;
	tail = n2;
	bb = b1;

	for(i=0;i<len-1;i++){
		if(bb > MHT(p[i],p[i+1]))
			bb -= MHT(p[i],p[i+1]);
		else
			break;
	}
	int bb1 = bb;
	int bb2 = MHT(p[i],p[i+1]) - bb;
	k=i;

	next = head;
	for(i=1;i<=k;i++){
		DME_NODE *n = init_dme_blockage_node(p[i], wire_type, buf_u);
		n->left_direction = p[i-1].dir_from;
		n->pleft = next;
		next = n;
	}
	parent->pleft = next;
	parent->left_direction = p[k].dir_from;
	next = tail;
	for(i=len-2;i>k;i--){
		DME_NODE *n = init_dme_blockage_node(p[i], wire_type, buf_u);
		n->left_direction = p[i].dir_to;
		n->pleft = next;
		next = n;
	}
	if(k>0){
		head->is_select=TRUE;
		head->select_x = p[0].x;
		head->select_y = p[0].y;
		head->sx = head->select_x + head->select_y;
		head->sy = head->select_x - head->select_y;
	}else
		head->is_select=FALSE;

	if(k<(len-2)){
		tail->is_select=TRUE;
		tail->select_x = p[len-1].x;
		tail->select_y = p[len-1].y;
		tail->sx = tail->select_x + tail->select_y;
		tail->sy = tail->select_x - tail->select_y;
	}else
		tail->is_select=FALSE;

	parent->pright = next;
	parent->right_direction = p[k].dir_to;


	if(overlapped(parent->pleft, parent->pright) == FALSE){
		parent->lower = MAX(parent->pleft->lower - bb1, parent->pright->lower - bb2);
		parent->upper = MIN(parent->pleft->upper + bb1, parent->pright->upper + bb2);
		parent->left = MAX(parent->pleft->left - bb1, parent->pright->left - bb2);
		parent->right = MIN(parent->pleft->right + bb1, parent->pright->right + bb2);
		parent->x1 = (parent->lower + parent->left)/2;
		parent->y1 = (parent->lower - parent->left)/2;
		parent->x2 = (parent->upper + parent->right)/2;
		parent->y2 = (parent->upper - parent->right)/2;
		//parent->is_selected = FALSE;

	}else{
        	int lower1 = p[k].x + p[k].y;
        	int left1 = p[k].x - p[k].y;
        	int lower2 = p[k+1].x + p[k+1].y;
        	int left2 = p[k+1].x - p[k+1].y;
        	parent->lower = MAX(lower1 - bb1, lower2 - bb2);
        	parent->upper = MIN(lower1 + bb1, lower2 + bb2);
      		parent->left = MAX(left1 - bb1, left2 - bb2);
        	parent->right = MIN(left1 + bb1, left2 + bb2);

        	parent->x1 = (parent->lower + parent->left)/2;
       		parent->y1 = (parent->lower - parent->left)/2;
	        parent->x2 = (parent->upper + parent->right)/2;
	        parent->y2 = (parent->upper - parent->right)/2;
		if(k==0){
			head->is_select=TRUE;
			head->select_x = p[0].x;
			head->select_y = p[0].y;
			head->sx = head->select_x + head->select_y;
			head->sy = head->select_x - head->select_y;
		}
		if(k == (len-2)){
			tail->is_select=TRUE;
			tail->select_x = p[len-1].x;
			tail->select_y = p[len-1].y;
			tail->sx = tail->select_x + tail->select_y;
			tail->sy = tail->select_x - tail->select_y;
		}
		//parent->select_x = (parent->x1 + parent->x2)/2;
		//parent->select_y = (parent->y1 + parent->y2)/2;
		//parent->is_selected = TRUE;
	}
	if(point_in_path(parent->x1, parent->y1, p[k],p[k+1]) == TRUE){
		if(p[k].x > p[k+1].x)
			segment_merged(parent,TRUE,p[k+1],p[k]);
		else
			segment_merged(parent,TRUE,p[k],p[k+1]);
	}
	else{
		if(p[k].x > p[k+1].x)
			segment_merged(parent,FALSE,p[k+1],p[k]);
		else
			segment_merged(parent,FALSE,p[k],p[k+1]);

	}

// 	printf("a: %d %d %d %d %d %d\n",parent->node_id,len,parent->x1,parent->y1,parent->x2,parent->y2);

	parent->sink_index = -1;
	parent->blockage_node = 0;
//	parent->visited = 0;
	//n1->select_x = p[0].x;
	//n1->select_y = p[0].y;
	//n2->select_x = p[len-1].x;
	//n2->select_y = p[len-1].y;
//	n1->visited = 1;
//	n2->visited = 1;
	parent->downstream_length = distance + n1->downstream_length + n2->downstream_length + n1->detour + n2->detour;
	return;

}

void output_dme_node(FILE *fp, DME_NODE *n){
	if(n==NULL)
		return;
	if(n->sink_index>0){
		draw_point(fp , n->select_x, n->select_y, 0, BLUE);
	}
	else if(n->blockage_node ==0){
		double downright_x,downright_y,upleft_x,upleft_y;
		double factor;
	      if (frame.ur.x != 0)
        	        factor = (double)9500 / frame.ur.x;

  	      if (factor > 9500 / frame.ur.y)
        	        factor = (double)9500 / frame.ur.y;

		downright_x = n->x1;
                downright_y = n->y1;
                upleft_x = n->x2;
                upleft_y = n->y2;
                upleft_x = upleft_x*factor + OFFSET;
                upleft_y = upleft_y*factor + OFFSET;
                downright_x = downright_x*factor + OFFSET;
                downright_y = downright_y*factor + OFFSET;
                fprintf(fp,"2 1 0 1 1 7 50 -1 -1 0 0 0 -1 0 0 2\n");
                fprintf(fp,"\t%.0f %.0f %.0f %.0f\n",upleft_x,upleft_y,downright_x,downright_y);

//		draw_wire(fp,n->x1,n->y1, n->x2, n->y2, 0, BLACK);
		draw_point(fp , n->select_x, n->select_y, 0, RED);
	}
	double factor , upleft_x, upleft_y;
	if (frame.ur.x != 0)
		factor = (double)9500 / frame.ur.x;

	if (factor > 9500 / frame.ur.y)
		factor = (double)9500 / frame.ur.y;
	upleft_x = n->select_x*factor + OFFSET;
	upleft_y = n->select_y*factor + OFFSET;
 	fprintf(fp,"4 0 0 50 -1 0 12 0.0000 4 150 315 %.0f %.0f %d\\001\n",upleft_x,upleft_y,n->sink_index);
// 	printf("%d %d %d\n",n->node_id,n->select_x, n->select_y);
// 	if(n->blockage_node == 1){
// 		printf("%d->(%d)\n",n->node_id,n->pleft->node_id);
// 	}else if(!(n->sink_index > 0)){
//  		printf("%d->(%d %d)\n",n->node_id,n->pleft->node_id,n->pright->node_id);
// 		printf("%d->(%d %d)\n",n->sink_index,n->pleft->sink_index,n->pright->sink_index);
//	}

// 	printf("%d %d %d %d %d\n",n, n->pleft,n->pright,n->left_direction,n->right_direction);
	double xm,ym;
	if(n->pleft != NULL){
		switch(n->left_direction){
			case LEFT:
			case RIGHT:
				ym = n->select_y;
				xm = n->pleft->select_x;
				break;
			case UP:
			case DOWN:
			default:
				xm = n->select_x;
				ym = n->pleft->select_y;
				break;
		}
		draw_wire(fp, n->select_x, n->select_y, xm, ym, 0, GREEN);
		draw_wire(fp, n->pleft->select_x, n->pleft->select_y, xm, ym, 0, GREEN);
	}
	if(n->pright != NULL){
		switch(n->right_direction){
			case LEFT:
			case RIGHT:
				ym = n->select_y;
				xm = n->pright->select_x;
				break;
			case UP:
			case DOWN:
			default:
				xm = n->select_x;
				ym = n->pright->select_y;
				break;
		}
		draw_wire(fp, n->select_x, n->select_y, xm, ym, 0, GREEN);
		draw_wire(fp, n->pright->select_x, n->pright->select_y, xm, ym, 0, GREEN) ;
	}
	output_dme_node(fp, n->pleft);
	output_dme_node(fp, n->pright);
}


void print_fig_1(DME_NODE * L, int length){
	FILE* fp;
	char * filename = "t3.fig";
	fp = (FILE *) fopen(filename,"w");
	double factor;
	double upleft_x;
	double upleft_y;
	double downright_x;
	double downright_y;
	double width;
	double height;
	double t1_x,t1_y,t2_x,t2_y;

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

	output_dme_node(fp, &L[length-1]);
	/*
	for (i=0; i < (length); i++)
	{

		upleft_x = L[i].select_x;
		upleft_y = L[i].select_y;

		upleft_x = upleft_x*factor + OFFSET;
		upleft_y = upleft_y*factor + OFFSET;
		fprintf(fp,"1 3 0 1 0 7 50 -1 -1 0 1 0 %.0f %.0f 40 40 %.0f %.0f %.0f %.0f\n",upleft_x,upleft_y,upleft_x,upleft_y,upleft_x,upleft_y+40);

		upleft_x = (L[i].lower+L[i].left)/2;
		upleft_y = (L[i].lower-L[i].left)/2;
		downright_x = (L[i].upper+L[i].right)/2;
		downright_y = (L[i].upper-L[i].right)/2;

		upleft_x = upleft_x*factor + OFFSET;
		upleft_y = upleft_y*factor + OFFSET;
		downright_x = downright_x*factor + OFFSET;
		downright_y = downright_y*factor + OFFSET;

		if(i<(length/2+1)){
			fprintf(fp,"1 3 0 1 0 7 50 -1 -1 0 1 0 %.0f %.0f 50 50 %.0f %.0f %.0f %.0f\n",upleft_x,upleft_y,upleft_x,upleft_y,upleft_x,upleft_y+50);
			continue;
		}
		fprintf(fp,"2 1 0 1 0 7 50 -1 -1 0 0 0 -1 0 0 2\n");
		fprintf(fp,"\t%.0f %.0f %.0f %.0f\n",upleft_x,upleft_y,downright_x,downright_y);
//		continue;
		downright_x = L[i].pleft->select_x;
		downright_y = L[i].pleft->select_y;
		upleft_x = L[i].select_x;
		upleft_y = L[i].select_y;
		upleft_x = upleft_x*factor + OFFSET;
		upleft_y = upleft_y*factor + OFFSET;
		downright_x = downright_x*factor + OFFSET;
		downright_y = downright_y*factor + OFFSET;
		fprintf(fp,"2 1 0 1 1 7 50 -1 -1 0 0 0 -1 0 0 2\n");
		fprintf(fp,"\t%.0f %.0f %.0f %.0f\n",upleft_x,upleft_y,downright_x,downright_y);
		downright_x = L[i].pright->select_x;
		downright_y = L[i].pright->select_y;
		downright_x = downright_x*factor + OFFSET;
		downright_y = downright_y*factor + OFFSET;
		fprintf(fp,"2 1 0 1 1 7 50 -1 -1 0 0 0 -1 0 0 2\n");
		fprintf(fp,"\t%.0f %.0f %.0f %.0f\n",upleft_x,upleft_y,downright_x,downright_y);



      //		if (i >= general_block_num)
      //			fprintf(fp,"4 0 0 0 0 0 12 0.0000 4 135 180 %.0f %.0f %d\\001\n",(upleft_x+downright_x) / 2 - ID_OFFSET,(upleft_y + downright_y) / 2 + ID_OFFSET,i-general_block_num);
	}
	*/
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

void chooes_highway(DME_NODE *n, int s_r_length){

	int i, k;
	double buf1 = (buflib.lib[0].icap + buflib.lib[0].ocap);
	double beta1 = wirelib.lib[0].c;
	double beta2 = wirelib.lib[1].c;

	double Lmax1 = LAMBADA_w1;
	double Lmax2 = LAMBADA_w2;
	double sink_cap=0;
//	double cap_left = CapLimit - n->downstream_length*beta1 - n->downstream_length/Lmax1*buf1*2;
	for(i=0;i<sink.num;i++){
		sink_cap += sink.pool[i].lc;
	}
	cap_left = CapLimit - sink_cap - n->downstream_length*beta1 - n->downstream_total_buf_num*buf1*2;

	printf("buf num is %d %f\n", n->downstream_total_buf_num, n->downstream_length/Lmax1);
	printf("cap left %f\n",cap_left);
	int try_array[7] = {6,5,4,7,6,5,4};
	int lam[7] = {400001, 400001, 400001, 1000001, 1000001, 1000001,1000001};
	for(i=0;i<7;i++){
 	       int first_buf = lam[i] - try_array[i]*(int)(buflib.lib[0].icap/wirelib.lib[0].c);
 	        if(first_buf < 0){
 	                printf("first buf shit!!\n");
 	                first_buf = 0;
 	        }
 		k = ceil(1.0f*(s_r_length-first_buf)/lam[i]);
	//      printf("%d %d %d\n",k,first_buf,min_dis);
	        k++;
		if(s_r_length < first_buf){
  	     	        k = 1;
        	}
        	if((k + n->downstream_buf_num)%2 == 0){
        	        k++;
        	}
		double cost = try_array[i]*buf1*k + s_r_length * beta2;
		if(cap_left > cost)
			break;
	}
	if(i==7){
		printf("shit!!!\n");
		exit(0);
	}
	printf("now choosing %d\n", i);
	HIGHWAY = try_array[i];
	n->buf_unit = try_array[i];
	LAMBADA3 = lam[i];
	n->buf_num = k;
	if(i<3)
		n->wire_type = 1;
	else
		n->wire_type = 0;
}

void connect_to_source(DME_NODE * n1, DME_NODE * parent){
	NODE p[100];
	int length,min_i;
//	int distance = cal_distance(n1,parent);
	NODE tmp;
	tmp.x = parent->x1;
	tmp.y = parent->y1;
	int ret = insertpt(tmp, sink_num*2);
	update_dist(&blockage,ret);
	int i,j,dis,min_dis=INFINITE;
	for(i=static_num;i<g_size;i++){
		if(g_occupy[i] == FALSE)
			continue;
		if(mapping[i] == mapping[ret])
			continue;
		dis = pairs[ret][i];
		if(min_dis>dis){
			min_dis = dis;
			min_i = i;
		}
	}
	find_path(min_i,ret,p, &length);
	removept(ret);
	removept(min_i);
        int k1,other1=-1;
        for(k1=static_num;k1<g_size;k1++){
	if( k1 != min_i &&  g_occupy[k1] == 1 && mapping[k1]==mapping[min_i])
		other1 = k1;
	}
	if(other1 != -1)
		removept(other1);
        for(i=static_num;i<g_size;i++)
                if(g_occupy[i] == TRUE){
			printf("NNNN\n");
			exit(0);
		}

//	for(i=0;i<length;i++){
//		printf("%d %d %d %d\n",p[i].x, p[i].y, p[i].dir_from, p[i].dir_to);
//	}
	chooes_highway(n1, min_dis);
	DME_NODE *next = n1;
//	int wtype = 1;
//	printf("%d %d %d %d\n",n1->x1, n1->y1, n1->x2, n1->y2);
	for(i=1;i<length-1;i++){
		DME_NODE *n = init_dme_blockage_node(p[i], n1->wire_type, n1->buf_unit);
		n->left_direction = p[i].dir_to;
		n->pleft = next;
		next = n;
	}
//   	printf("xxxxxxxxxxxxxx %d %d %d %d\n",p[0].x,p[0].y,p[0].dir_from, p[0].dir_to);
//   	printf("xxxxxxxxxxxxxx %d %d %d %d\n",p[length-1].x,p[length-1].y,p[length-1].dir_from, p[length-1].dir_to);

	parent->pleft = next;
	parent->left_direction = p[0].dir_to;
	parent->downstream_length = n1->downstream_length + min_dis;
	parent->pright = NULL;

	if(n1->buf_num!=1)
		n1->factor = 0.995f*min_dis/(n1->buf_num-1)/LAMBADA3;
	else
		n1->factor = 1;
	//n1->factor = 1.0f * (k-2) / (k);
	n1->duplicate_first_buf = 0;
	parent->left_length= min_dis;
	parent->right_length= 0;
	n1->reduntant = 0;
	n1->detour = 0;
	n1->select_x = p[length-1].x;
	n1->select_y = p[length-1].y;
	n1->sx = p[length-1].x + p[length-1].y;
	n1->sy = p[length-1].x - p[length-1].y;
	n1->is_select = FALSE;
//	n1->wire_type = wtype;
//	n1->factor = 1.0f;
	n1->delta_length_buf = LAMBADA3;
	n1->first_buf_fraction = 0;
	printf("length is %d %d %d\n",length,n1->buf_num, min_dis);
	printf("root length is %f. root to source is %d \n",n1->downstream_length,min_dis);
}

void trace_back(DME_NODE * n, DME_NODE * parent){
	if(n==NULL)
		return;
	//printf("xxxxx dd \n");

//	printf("node is %d %d %d\n",n->node_id, n->blockage_node, n->is_select);
	if(n->lower > n->upper || n->left > n->right)
		{exit(0);}
	if(n->blockage_node == 0 && parent != NULL && n->is_select == FALSE){
		int dis = cal_distance(n,parent);
//		printf("distance is %d %d %d %d %d\n", dis,parent->select_x, parent->select_y, parent->sx, parent->sy);
//		printf("%d %d %d %d\n",parent->lower, parent->left,parent->upper, parent->right);
//		printf("%d %d %d %d\n",n->lower,n->left,n->upper, n->right);
//		printf("P: %d %d %d %d\n",parent->x1, parent->y1,parent->x2,parent->y2);
//		printf("RIGHT: %d %d %d %d\n",parent->x1, parent->y1,parent->x2,parent->y2);
//		printf("%d %d %d %d\n",n->x1, n->y1,n->x2,n->y2);
/*		printf("%.0f %.0f %.0f %.0f\n",parent->x1,parent->y1,parent->x2,parent->y2);
		printf("%.0f %.0f %.0f %.0f\n",n->x1,n->y1,n->x2,n->y2);*/
		n->sx = (MAX(n->lower, parent->sx - dis) + MIN(n->upper, parent->sx + dis))/2;
		n->sy = (MAX(n->left, parent->sy - dis) + MIN(n->right, parent->sy + dis))/2;
	        n->select_x = (n->sx+n->sy)/2;
	        n->select_y = (n->sx-n->sy)/2;
		if(ABS(Manhattan(n->select_x, n->select_y, parent->select_x, parent->select_y) - dis) > 10){
			printf("shiting %d %d %d %d.\n", n->select_x, n->select_y, parent->select_x, parent->select_y, dis);
			printf("dis is %d, %d\n",dis, Manhattan(n->select_x, n->select_y, parent->select_x, parent->select_y));
			exit(0);
		}
//		if(dis ==( Manhattan(parent->select_x, parent->select_y, n->x1, n->y1))){
//		        n->select_x = n->x1;
//		        n->select_y = n->y1;
//			n->sx = n->select_x + n->select_y;
//			n->sy = n->select_x - n->select_y;
//		}else if(dis ==( Manhattan(parent->select_x, parent->select_y, n->x2, n->y2))){
//
//		        n->select_x = n->x2;
//		        n->select_y = n->y2;
//			n->sx = n->select_x + n->select_y;
//			n->sy = n->select_x - n->select_y;
//		}else{
//			printf("trace back shit!!\n");
//			printf("dis is %d, manhattan is %d\n", dis, Manhattan(parent->select_x, parent->select_y, n->x1, n->y1));
//			printf("dis is %d, manhattan is %d\n", dis, Manhattan(parent->select_x, parent->select_y, n->x2, n->y2));
//			exit(0);
//		}

//		printf("traced cood is %d %d\n", n->select_x, n->select_y);

   // 		printf("%d %d %d %d %d %d\n",n->node_id,parent->select_x, parent->select_y, n->select_x,n->select_y,dis);
	}
// 	printf("? %d %d %d\n",n->blockage_node,n->select_x,n->select_y);
//	printf("	%d %d %d %d\n",n->x1,n->y1,n->x2,n->y2);
	trace_back(n->pleft, n);
//	if(parent == NULL)
//		printf("\n\n");
	trace_back(n->pright, n);
}

void coordinate_translate(DME_NODE * n){
	if(n==NULL)
		return;
	double t1,t2;
	t1 = (n->sx+n->sy)/2;
	t2 = (n->sx-n->sy)/2;
	n->select_x = t1;
	n->select_y = t2;
	coordinate_translate(n->pleft);
	coordinate_translate(n->pright);
}

void free_tree_node(DME_TREE_NODE * OT){
	if ( OT == NULL)
		return ;
	free_tree_node(OT->ls);
	free_tree_node(OT->rs);
	free(OT);

}

void free_buf(BUF_NODE ** OBUF){
	int i = 0 ;
	BUF_NODE * b1, * b2;
	for ( i = 0 ; i < free_buf_num; i++){
		b1 = (*(OBUF+i)) ;
		while(b1 != NULL){
			b2 = b1->next;
			free(b1);
			b1 = b2;
		}

	}
	free(OBUF);
}


void eleminate(BUF_NODE ** OBUF, DME_TREE_NODE * OT, DME_TREE_NODE ** OTmap){
	free_tree_node(OT);
	free(OTmap);
	free_buf(OBUF);

}

void subtree_call_ngspice(DME_NODE *n, double *ltree, double *rtree){
	FILE * tfp , * ifp;
	BUF_NODE ** OBUF;
	DME_TREE_NODE * lt, * rt ;
	DME_TREE_NODE * OT;
	DME_TREE_NODE ** OTmap ;
	CUSINK * sink_alt_array ;
	int i,j;
	tfp = fopen("outfile","w");
	ifp = fopen("infile","w");
// 	printf(" call ngspice \n");
	mid_sink_num = 0;
	insert_buffer(n,&OBUF,&OT,&OTmap);
	gen_inputfile(ifp,OT,OTmap,0);
	output_file(tfp,OBUF,OT,OTmap);
	fclose(tfp);
	fclose(ifp);
	sink_alt_array = (CUSINK *) malloc(sizeof(CUSINK)*(mid_sink_num));
	if (mid_sink_num == 0 || sink_alt_array == NULL){
		printf("no sink alt array\n");
		exit(0);
	}
	sink_array_num = 0;
	construct_SINK_array(OT,sink_alt_array,mid_sink_num);
	preprocess(1,sink_alt_array,mid_sink_num);
	lt = OT->ls->ls;
	rt = OT->ls->rs;
	if ( lt == NULL || rt == NULL){
		printf(" wrong test node left or right son is NULL\n");
		exit(0);
	}
// 	printf(" ***********%d %d %d %d*************\n", lt->left,lt->right,rt->left,rt->right);
	(*ltree) = (*rtree) = 0.0;
	for ( i = lt->left ; i <= lt->right ; i++){
		for ( j= 0 ; j<4; j++)
			(*ltree) += sink_alt_array[i].latency[j] ;
	}
	(*ltree) /= 4*(lt->right+1) ;
	for ( i = rt->left ; i <= rt->right ; i++){
		for ( j= 0 ; j<4; j++)
			(*rtree) += sink_alt_array[i].latency[j] ;
	}
	(*rtree) /=4* (rt->right-rt->left + 1) ;

// 	printf(" ***********%f     %f*************\n",*ltree,*rtree);


// 	for ( i = 0 ; i < mid_sink_num ; i++){
// 		printf(" %d %d \t",sink_alt_array[i].node_id,sink_alt_array[i].sink_index);
// 		for ( j = 0 ; j < 4 ; j++)
// 			printf("%f ",sink_alt_array[i].latency[j]);
// 		printf("\n");
// 	}

//	sleep(2);
	eleminate(OBUF,OT,OTmap);
	free(sink_alt_array);
	OBUF = NULL;
	OT = NULL;
	OTmap = NULL;
//	exit(0);


}


BOOL ngspice_test(DME_NODE * n, double *delta_delay, BOOL flag){
double ltree,rtree;
BUF_NODE ** OBUF;
DME_TREE_NODE * OT;
DME_TREE_NODE ** OTmap ;
	source_node->pleft = n;
	source_node->left_direction = RIGHT;
	source_node->pright = NULL;
	n->duplicate_first_buf = 0;
	n->buf_num = 0;
	n->wire_type= 0;
	n->reduntant = 0;
	n->is_select = FALSE;
	n->detour = 0;
	n->blockage_node = 0;
//	n->select_x = (n->x1+n->x2)/2;
//	n->select_y = (n->y1+n->y2)/2;
//	source_node->left_length = Manhattan(0,0,n->select_x,n->select_y);
	source_node->left_length = cal_distance(n,source_node);

//	printf("0 \n");
	trace_back(source_node,NULL);
//	printf("1 \n");
//	coordinate_translate(n);
	check_DME(n);
	n->select_x = n->x1;
	n->select_y = n->y1;
	n->sx = n->select_x + n->select_y;
	n->sy = n->select_x - n->select_y;
	trace_back(n,NULL);
//	printf("2\n");
	check_DME(n);
	n->select_x = n->x2;
	n->select_y = n->y2;
	n->sx = n->select_x + n->select_y;
	n->sy = n->select_x - n->select_y;
	trace_back(n,NULL);
	check_DME(n);
//	printf("3 \n");
//	return FALSE;
	trace_back(source_node,NULL);
	subtree_call_ngspice(source_node,&ltree,&rtree);
//	printf("4 \n");
//	check_DME(n);
	*delta_delay =  ltree - rtree;
// 	printf("%f %f %f\n", ltree, rtree, *delta_delay);
	// insert_buf(n);
	if(flag == TRUE && ABS(*delta_delay) > 0.1)
		return TRUE;
//	n->to_sink_delay = (ltree + rtree)/2;
 	printf("%f %f %f\n", ltree, rtree, *delta_delay);
	return FALSE;
}


void dme_core(DME_NODE * L, int length){
	int i,j,i2;
	int count=0;
	source_node = (DME_NODE *) malloc (sizeof(DME_NODE));
	source_node->lower = 0;
	source_node->upper = 0;
	source_node->left = 0;
	source_node->right = 0;
	source_node->x1 = 0;
	source_node->y1 = 0;
	source_node->x2 = 0;
	source_node->y2 = 0;
	source_node->select_x = 0;
	source_node->select_y = 0;
	source_node->sx = 0;
	source_node->sy = 0;
	source_node->tree_level = 0;

	source_node->blockage_node = 0;
	source_node->sink_index = -1;
	NODE p[100];
	NODE min_p[100];
	int current_level = 0;
	int c_k;
	int li_length = length;
	int li_k = 0;
	int li_flexicity = log(length)/log(2) - 5;
	if(li_flexicity < 0) li_flexicity = 1;
	//li_flexicity = 0;
	while(count<(length-1)){
		int min_dis=INFINITE;
		int dis;
		int min_i,min_j;
		int len,min_len;
		double cost,min_cost=INFINITE;

		if(li_k == li_length/2){
			li_length = li_length - 2 * li_k + li_k;
			current_level++;
			li_k = 0;
		}
		for(i=static_num;i<g_size;i++){
			if(g_occupy[i] == FALSE)
				continue;
			for(j=i+1;j<g_size;j++){
				if(g_occupy[j] == FALSE)
					continue;
				if(mapping[i] == mapping[j])
					continue;
// 				printf("%d %d %d %d %d\n", i,j,L[i].level,L[j].level,current_level);
				if(L[mapping[i]].level>current_level + li_flexicity || L[mapping[j]].level>current_level + li_flexicity)
					continue;
//				dis = cal_distance(&L[i],&L[j],1, &p,&len);
				dis = pairs[i][j];
//				printf("xxxxx %d\n",dis);
//				cost = dis/100 + 10000*ABS(L[i].to_sink_delay - L[j].to_sink_delay) + 500*ABS(L[i].capacitance - L[j].capacitance);
				//dis =test_distance(&L[i],&L[j]);
				if(min_dis>dis){

//				if(min_cost>cost){
//					min_cost = cost;
					min_dis = dis;
					min_i = i;
					min_j = j;
//					min_len = len;
//					for(i2=0;i2<len;i2++)
//						min_p[i2] = p[i2];
				}
			}

		}
 //		printf("%d %d %d\n",min_i, min_j,min_dis);
		int L_min_i = mapping[min_i], L_min_j = mapping[min_j];
		if(L_min_i > L_min_j){
			int temp = L_min_i;
			L_min_i = L_min_j;
			L_min_j = temp;
			temp = min_i;
			min_i = min_j;
			min_j = temp;
		}
  //		printf("%d %d\n",L_min_i, L_min_j);
		find_path(min_i, min_j, min_p, &min_len);

		L[length+count].node_id = length+count;
		double delta_delay = 0,delta_before = 0;
		double delay_prev = 10, min_delay=999,min_before;
		int kkkk=0;
		do{
			if((delta_delay * delay_prev < 0 && ABS(ABS(delta_delay) - ABS(delay_prev)) < 0.1) || kkkk>BIG_NUM){
//			if( kkkk>BIG_NUM)
			//	kkkk = BIG_NUM + 1;
				ngspice_test(&L[length+count],&delta_delay, FALSE);
				break;
			}
        	        delay_prev = delta_delay;
               	       	delta_before = delta_before+delta_delay;
                	if(kkkk!=0 &&( (ABS(min_delay)) > (ABS(delta_delay)))){
                	        min_delay = delta_delay;
                                min_before = delta_before-delta_delay;
                       	}
			merge(&L[L_min_j],&L[L_min_i],&L[length+count],min_dis,min_p,min_len, delta_before,0, FALSE, 2);
			kkkk++;
			if(kkkk!=1)
				printf("%d: %f %f\n",kkkk, delta_before,delta_delay);
			//printf("%d\n",kkkk);
		}
		while( ngspice_test(&L[length+count],&delta_delay, TRUE) == TRUE);
/*
		if(kkkk == BIG_NUM+1){
//			merge(&L[L_min_j],&L[L_min_i],&L[length+count],min_dis,min_p,min_len, min_before,0,2);
			printf("now we have to detour someting.\n");
			delta_delay = 0,delta_before = 0;
			kkkk=0;
  	        	do{
        	                if(kkkk>BIG_NUM)
                	                break;
        	                delay_prev = delta_delay;
               	         	delta_before = delta_before+delta_delay;
                	        if(kkkk!=0 &&( (ABS(min_delay)) > (ABS(delta_delay)))){
                	                min_delay = delta_delay;
                        	        min_before = delta_before-delta_delay;
                        	}
                       	 	merge(&L[L_min_j],&L[L_min_i],&L[length+count],min_dis,min_p,min_len, delta_before,0, TRUE, 2);
                  	        kkkk++;
				if(kkkk!=0)
					printf("%d: %f %f\n",kkkk, delta_before,delta_delay);
                	}
                	while( ngspice_test(&L[length+count],&delta_delay, TRUE) == TRUE);
		}
*/
		if(kkkk == BIG_NUM + 1){
			merge(&L[L_min_j],&L[L_min_i],&L[length+count],min_dis,min_p,min_len, min_before,0,TRUE, 2);
			ngspice_test(&L[length+count],&delta_delay, FALSE);
			printf("haha here\n");
		}
		printf("haha, delta is %f\n", delta_delay);

		int k1,other1=-1,other2=-1;
		for(k1=static_num;k1<g_size;k1++){
			if( k1 != min_i &&  g_occupy[k1] == 1 && mapping[k1]==mapping[min_i])
				other1 = k1;
			if( k1 != min_j &&  g_occupy[k1] == 1 && mapping[k1]==mapping[min_j])
				other2 = k1;
		}
		removept(min_i);
		removept(min_j);
		if(other1 != -1)
			removept(other1);
		if(other2 != -1)
			removept(other2);
		NODE tmp;
		tmp.x = L[length+count].x1;
		tmp.y = L[length+count].y1;
		int q1 = insertpt(tmp, length+count);
		update_dist(&blockage,q1);
		tmp.x = L[length+count].x2;
		tmp.y = L[length+count].y2;
		q1 = insertpt(tmp, length+count);
		update_dist(&blockage,q1);
		L[length+count].level = MAX(L[L_min_i].level, L[L_min_j].level)+1;
 		printf("%d %d %d %d %d\n",count, L_min_i,L_min_j,min_dis,min_len);
		count++;
		li_k++;
	}
	connect_to_source(&L[2*length-2],source_node);
	trace_back(source_node,NULL);

}

void buf_count(DME_NODE * n, int p_num, int level){
	if(n==NULL)
		return;
	if(n->blockage_node == 0){
		n->upstream_buf_num = p_num + n->buf_num;// + n->duplicate_first_buf;
		n->tree_level = level;
		level_length[level] += n->left_length + n->right_length;
		level++;
	}else{
		n->tree_level = level;
	}
	buf_count(n->pleft, n->upstream_buf_num,level);
	buf_count(n->pright,n->upstream_buf_num,level);
/*	if(n->sink_index != -1)
		printf("sink %d buffer number is %d\n", n->sink_index, n->upstream_buf_num);*/
}

void label_tree(DME_NODE *n, int which_level){
	if(n==NULL)
		return;
	if(n->tree_level < which_level && n->pleft != NULL)
		n->visited = 0;
	else
		n->visited = 1;
	label_tree(n->pleft, which_level);
	label_tree(n->pright, which_level);
}

void reconstruct_tree(DME_NODE * n, int which_level, DME_NODE * L){
	label_tree(n, which_level);
	int i,j,count=0;
	for(i=0;i<2*sink.num-1;i++){
		if(L[i].tree_level == which_level){
			NODE tmp;
			tmp.x = L[i].x1;
			tmp.y = L[i].y1;
			int q1 = insertpt(tmp, i);
			update_dist(&blockage,q1);
			count++;
			if(L[i].pleft == NULL)
				continue;
			tmp.x = L[i].x2;
			tmp.y = L[i].y2;
			q1 = insertpt(tmp, i);
			update_dist(&blockage,q1);

		}
		if(L[i].tree_level < which_level && L[i].pleft == NULL){
			count++;
			NODE tmp;
			tmp.x = L[i].x1;
			tmp.y = L[i].y1;
			int q1 = insertpt(tmp, i);
			update_dist(&blockage,q1);
		}
	}
	NODE p[100];
	NODE min_p[100];
	printf("re-construction count is %d\n",count);
	count = 0;
	while(1){
		int min_dis=INFINITE;
		int dis;
		int min_i,min_j;
		int len,min_len;
		int merged_parent;
		for(i=0;i<2*sink.num-1;i++)
			if(L[i].visited == 0)
				break;
		if(i==2*sink.num - 1)
			break;
		else
			merged_parent = i;

		for(i=static_num;i<g_size;i++){
			if(g_occupy[i] == FALSE)
				continue;
			for(j=i+1;j<g_size;j++){
				if(g_occupy[j] == FALSE)
					continue;
				if(mapping[i] == mapping[j])
					continue;
				dis = pairs[i][j];
				if(min_dis>dis){
					min_dis = dis;
					min_i = i;
					min_j = j;
				}
			}

		}
// 		printf("%d %d %d\n",min_i, min_j,min_dis);
		int L_min_i = mapping[min_i], L_min_j = mapping[min_j];
		if(L_min_i > L_min_j){
			int temp = L_min_i;
			L_min_i = L_min_j;
			L_min_j = temp;
			temp = min_i;
			min_i = min_j;
			min_j = temp;
		}
 		printf("%d %d\n",L_min_i, L_min_j);
		find_path(min_i, min_j, min_p, &min_len);

                double delta_delay = 0,delta_before = 0;
                int kkkk=0;
                do{
//                      if((delta_delay * delay_prev < 0 && ABS(ABS(delta_delay) - ABS(delay_prev)) < 0.1) || kkkk>BIG_NUM)
                        if( kkkk>BIG_NUM)
                                break;
                        delta_before = delta_before+delta_delay;
			merge(&L[L_min_j],&L[L_min_i],&L[merged_parent],min_dis,min_p,min_len, delta_before,0, FALSE, 3);
                        kkkk++;
//                      if(kkkk!=0)
//                              printf("%d: %f %f\n",kkkk, delta_before,delta_delay);
                        //printf("%d\n",kkkk);
                }
                while( ngspice_test(&L[merged_parent],&delta_delay, TRUE) == TRUE);

                double delay_prev = 10, min_delay=999,min_before;
                if(kkkk == BIG_NUM+1){
//                      merge(&L[L_min_j],&L[L_min_i],&L[length+count],min_dis,min_p,min_len, min_before,0,2);
                        printf("now we have to detour someting.\n");
                        delta_delay = 0,delta_before = 0;
                        kkkk=0;
                        do{
                                if(kkkk>BIG_NUM+20)
                                        break;
                                delay_prev = delta_delay;
                                delta_before = delta_before+delta_delay;
                                if(kkkk!=0 &&( (ABS(min_delay)) > (ABS(delta_delay)))){
                                        min_delay = delta_delay;
                                        min_before = delta_before-delta_delay;
                                }
				merge(&L[L_min_j],&L[L_min_i],&L[merged_parent],min_dis,min_p,min_len, delta_before,0, FALSE, 3);
                                kkkk++;
                        }
			while( ngspice_test(&L[merged_parent],&delta_delay, TRUE) == TRUE);
                }
                if(kkkk == BIG_NUM + 21){
			merge(&L[L_min_j],&L[L_min_i],&L[merged_parent],min_dis,min_p,min_len, min_before,0, TRUE, 3);
                        ngspice_test(&L[merged_parent],&delta_delay, FALSE);
                        printf("haha here\n");
                }
                printf("x haha, delta is %f\n", delta_delay);


		int k1,other1=-1,other2=-1;
		for(k1=static_num;k1<g_size;k1++){
			if( k1 != min_i && g_occupy[k1] == 1 && mapping[k1]==mapping[min_i])
				other1 = k1;
			if( k1 != min_j && g_occupy[k1] == 1 && mapping[k1]==mapping[min_j])
				other2 = k1;
		}
		removept(min_i);
		removept(min_j);
		if(other1 != -1)
			removept(other1);
		if(other2 != -1)
			removept(other2);
		NODE tmp;
		tmp.x = L[merged_parent].x1;
		tmp.y = L[merged_parent].y1;
		int q1 = insertpt(tmp, merged_parent);
		update_dist(&blockage,q1);
		tmp.x = L[merged_parent].x2;
		tmp.y = L[merged_parent].y2;
		q1 = insertpt(tmp, merged_parent);
		update_dist(&blockage,q1);
		L[merged_parent].visited = 1;

 		printf("%d %d %d %d %d %d\n",count++, merged_parent, L_min_i,L_min_j,min_dis,min_len);

	}
//	printf("lxxxxxxxxxxxx\n");
	connect_to_source(&L[2*sink.num-2],source_node);
	trace_back(source_node,NULL);
}

int tree_statistic(DME_NODE * n){
	int i;
	double total = 0;
	for(i=0;i<50;i++)
		if(level_length[i] < 1)
			break;

	int num = i;
	printf("total level is %d\n", num+1);
	for(i=0;i<num;i++)
		total += level_length[i];
	printf("total length is %f. Cap limit is %d\n",total,CapLimit);
	double buf1 = 2*(buflib.lib[0].icap + buflib.lib[0].ocap);
	double beta1 = wirelib.lib[0].c;
	double beta2 = wirelib.lib[1].c;

	double Lmax1 = LAMBADA_w1;
	double Lmax2 = LAMBADA_w2;

	double tt = (CapLimit - level_length[0]/LAMBADA3*3*buf1  - total*beta1 - total/Lmax1*buf1)/(beta2 - beta1 + buf1/Lmax2 - buf1/Lmax1);
	printf("tt is %f\n",tt);
	double t_length = 0;
	for(i=0;i<num;i++){
		t_length += level_length[i];
		if(t_length > tt)
			break;
	}
	return i-1>0?i-1:0;
}

void deferred_merge_embedding(){
	int i;

	DME_NODE * L = (DME_NODE *) malloc (sizeof(DME_NODE) * (2*sink.num));

	//constructg(&blockage);
	construct_g_all(&blockage,&frame,&sink);
	int which = all_pair_shortest();

	int length = sink.num;
	for(i=0;i<sink.num; i++){
		//printf("%d %d %d %d\n", sink.pool[i].index,sink.pool[i].x, sink.pool[i].y, sink.pool[i].lc);
		L[i].lower = L[i].upper = (sink.pool[i].x) + (sink.pool[i].y);
		L[i].left = L[i].right = (sink.pool[i].x) - (sink.pool[i].y);
		L[i].x1 = L[i].x2 = (sink.pool[i].x);
		L[i].y1 = L[i].y2 = (sink.pool[i].y);
		L[i].sink_index = sink.pool[i].index;
		L[i].visited = 0;
		L[i].weight = 0;
		L[i].to_sink_delay = 0;
		L[i].delta_length_buf = 0;
		L[i].first_buf_fraction = 1;
		L[i].buf_num = 0;
		L[i].blockage_node = 0;
		L[i].downstream_buf_num = 0;
		L[i].downstream_total_buf_num = 0;
		L[i].downstream_length = 0.0f;
		L[i].factor = 1.0;
		L[i].capacitance = (double)(sink.pool[i].lc);
		L[i].pleft=NULL;
		L[i].pright=NULL;
		L[i].reduntant = 0;
		L[i].node_id=i;
		L[i].is_select=TRUE;
		L[i].level = 0;
		L[i].buf_unit = 2;
		L[i].wire_type = 0;
		L[i].duplicate_first_buf=0;
	}
//	get_init_delay(L);
//	L[0].to_sink_delay = 200;
	dme_core(L, length);
	buf_count(source_node,0,0);
//	printf("------------------------------------------------------\n");
//	int which_level = tree_statistic(source_node);
//	printf("tree level is %d--------------------------------------------------\n",which_level);
//	sleep(2);
//	if(which_level>0)
//		reconstruct_tree(source_node,which_level,L);
//	check_DME(&L[2*length-2]);
	print_fig_1(L,length*2-1);
//	free_all();

//	return L;

}

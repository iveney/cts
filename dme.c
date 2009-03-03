#include "ds.h"
#include "stdio.h"
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
//TODO no blockage between them

extern BOX frame;
extern SOURCE source  ;
extern SINK sink; 
extern WIRELIB wirelib;
extern BUFLIB	buflib ; 
extern VDDLIB	vddlib ; 
extern int SlewLimit; 
extern int CapLimit;
extern BLOCKAGE blockage;
extern int g_size;
extern int free_buf_num;
extern int mid_sink_num; 
extern int sink_array_num;
void segment_merged(DME_NODE *n, BOOL first){
	NODE s,t;
	if(n->x1 == n->x2)
		return;
	s.x = n->x1;
	s.y = n->y1;
	t.x = n->x2;
	t.y = n->y2;

	if(first == TRUE)
		compute_segment(&s,&t,&blockage,blockage.num);
	else
		compute_segment(&t,&s,&blockage,blockage.num);	
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
// 	printf("ffffff  %d\n",*length);
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


DME_NODE * init_dme_blockage_node(NODE p){
	DME_NODE *n = (DME_NODE *) malloc (sizeof(DME_NODE));
	n->blockage_node = 1;
	n->duplicate_first_buf = 0;
	n->buf_num = 0;
	n->sink_index = -1;
	n->x1 = n->x2 = p.x;
	n->y1 = n->y2 = p.y;
	n->sx = n->lower = n->upper = n->x1 + n->y1;
	n->sy = n->left = n->right = n->x1 - n->y1;
	n->pright=NULL;
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

double solve_equation(double gamma1, double gamma2, double sigma){
	return (double)(2*gamma1*gamma2/(gamma1+gamma2+gamma2/sigma - sqrt(pow(gamma1 + gamma2 + gamma2/sigma,2) - 4*gamma1*gamma2)));
}
int ballance_point(DME_NODE * n1, DME_NODE * n2, DME_NODE * parent, int distance, double delta_delay){
	int total_distance = distance;
	double alpha = wirelib.lib[0].r;
	double beta = wirelib.lib[0].c;
	double delay_buf = 5.1f;
	double buf_input_cap = buflib.lib[0].icap;
	double buf_output_cap = buflib.lib[0].ocap;
	//buf_output_cap = 0;
	double buf_output_r = buflib.lib[0].ores;
	double delay1 = n1->to_sink_delay;
	double delay2 = n2->to_sink_delay;
	int step1 = LAMBADA - n1->delta_length_buf;
	int step2 = LAMBADA - n2->delta_length_buf;
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
	int k1_from = (bb1+n1->delta_length_buf)/LAMBADA - 2;
	k1_from = k1_from>0?k1_from :0;
	int k1_to = (int)((bb1+n1->delta_length_buf)/LAMBADA + 5);
	
	int k2_from = (bb2+n2->delta_length_buf)/LAMBADA -2 ;
	k2_from = k2_from>0?k2_from :0;
	int k2_to = (int)((bb2+n2->delta_length_buf)/LAMBADA + 5);
// 	k1_to = k2_to = 100;
// 	printf("k1 from to %d %d\n",k1_from, k1_to);
// 	printf("k2 from to %d %d\n",k2_from, k2_to);
// 	printf("buffer %f %f %f\n",buf_input_cap,buf_output_r,buf_output_cap);
	if(bb1>bb2)
			flag = 1;
//	int count = 100000;
	while(1){
		
		int i,j;
		int sum;
/*		for(i=k1_from;i<k1_to;i++)
			for(j=k2_from;j<k2_to;j++){*/
		
		for(sum=0;sum<50;sum++)
			for(i=0;i<=sum;i++){
				j = sum - i;
				if((i+j+n1->downstream_buf_num+n2->downstream_buf_num)%2 == 1)
					continue;
				
				length = distance + detour;
				switch(i){
					case 0:
						delay1 =  n1->to_sink_delay;
						cap_1 = n1->capacitance;
						b1=0;
						break;
					case 1:
						step1 = (int)((LAMBADA - n1->delta_length_buf)*factor1);
						length -= step1;
						cap_1 = buf_input_cap;
						delay_buf = buf_output_r*(buf_output_cap+n1->capacitance+step1*beta)*0.001;
//						int d_num = (int)((n1->capacitance+step1*beta)/(LAMBADA*beta+buf_input_cap));

						if(n1->duplicate_first_buf == 1){
							cap_1 += buf_input_cap;
							delay_buf = buf_output_r/2*(buf_output_cap*2+n1->capacitance+step1*beta)*0.001;
						}
						delay1 =  n1->to_sink_delay + (alpha*beta/2*step1*step1+alpha*step1*n1->capacitance)*0.001+delay_buf;
						b1=step1;
						break;
					default:
						step1 = (int)((LAMBADA - n1->delta_length_buf)*factor1);
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
						if(n1->duplicate_first_buf == 1){	
							step1 = (int)((LAMBADA - buf_input_cap/beta)*factor1);
							b1 += step1;
							length -= step1;
							delay_buf = buf_output_r*(buf_output_cap+2*buf_input_cap+step1*beta)*0.001;
							delay1 += (alpha*beta/2*step1*step1+alpha*step1*2*buf_input_cap)*0.001+delay_buf;
						}else{
							step1 = (int)(LAMBADA*factor1);
							b1 += step1;
							length -= step1;
							delay_buf = buf_output_r*(buf_output_cap+buf_input_cap+step1*beta)*0.001;
							delay1 += (alpha*beta/2*step1*step1+alpha*step1*buf_input_cap)*0.001+delay_buf;
						}
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
						step2 = (int)((LAMBADA - n2->delta_length_buf)*factor2);
						length -= step2;
						b2=step2;
						cap_2 = buf_input_cap;
						delay_buf = buf_output_r*(buf_output_cap+n2->capacitance+step2*beta)*0.001;
						if(n2->duplicate_first_buf == 1){
							cap_2 += buf_input_cap;
							delay_buf = buf_output_r/2*(buf_output_cap*2+n2->capacitance+step2*beta)*0.001;
						}
						delay2 =  n2->to_sink_delay + (alpha*beta/2*step2*step2+alpha*step2*n2->capacitance)*0.001+delay_buf;
						
/* 						if(n2->capacitance>CAP_LIMIT)
						if(n2->sink_index == -1 && n2->blockage_node == 0)
							cap_2 += buf_input_cap;*/
						break;
					default:
						step2 = (int)((LAMBADA - n2->delta_length_buf)*factor2);
						length -= step2;
						b2=step2;
 						delay_buf = buf_output_r*(buf_output_cap+n2->capacitance+step2*beta)*0.001;
// 						printf("xxxxxxxxx delay is %f\n",delay_buf);
						if(n2->duplicate_first_buf == 1){
							delay_buf = buf_output_r/2*(buf_output_cap*2+n2->capacitance+step2*beta)*0.001;
						}
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
						if(n2->duplicate_first_buf == 1){
							step2 = (int)((LAMBADA - buf_input_cap/beta)*factor2);
							b2 += step2;
							length -= step2;
							delay_buf = buf_output_r*(buf_output_cap+2*buf_input_cap+step2*beta)*0.001;
							delay2 += (alpha*beta/2*step2*step2+alpha*step2*2*buf_input_cap)*0.001+delay_buf;
						}else{
							step2 = (int)(LAMBADA*factor2);
							b2 += step2;
							length -= step2;
							delay_buf = buf_output_r*(buf_output_cap+buf_input_cap+step2*beta)*0.001;
							delay2 += (alpha*beta/2*step2*step2+alpha*step2*buf_input_cap)*0.001+delay_buf;
						}
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
				if(bb1>=0 && ((i==0 && bb1<=(LAMBADA2-n1->delta_length_buf)) || (i>0 && bb1<=LAMBADA2)))
					if(bb2>=0 && ((j==0 && bb2<=(LAMBADA2-n2->delta_length_buf)) || (j>0 && bb2<=LAMBADA2))){
						b1 += bb1;
						b2 += bb2;
// 						printf("%d %d %d %d %d %d %d %f %f \n",bb1,bb2,b1,b2,i,j,distance,factor1,factor2);
// 						printf("delta %d %d\n",n1->delta_length_buf,n2->delta_length_buf);

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
						parent->capacitance = cap_1 + cap_2 + (bb1+bb2)*beta;
						parent->resistance = alpha*((double)bb1*bb2)/(bb1+bb2);
						parent->cap1 = (bb1+bb2)*beta/2;
						parent->cap2 = (bb1+bb2)*beta/2 + cap_1 + cap_2;
// 						printf("cap is %f %f %f\n",parent->capacitance,cap_1,cap_2);
						parent->to_sink_delay = ((alpha*bb1*(0.5*beta*bb1+cap_1)*0.001 + delay1) + (alpha*bb2*(0.5*beta*bb2+cap_2)*0.001 + delay2))/2;
//  						printf("Parent delay is %f\n",parent->to_sink_delay);
// 						printf("delay 1 is %f, delay 2 is %f\n",delay1,delay2);
						int tmp1 = bb1,tmp2 = bb2;
// 						printf("%f %f %f\n",parent->to_sink_delay,n1->to_sink_delay,n2->to_sink_delay);

						if(i==0)
							tmp1 += n1->delta_length_buf;
						if(j==0)
							tmp2 += n2->delta_length_buf;
						
						parent->delta_length_buf = MAX(tmp1,tmp2);
 						int tt = (LAMBADA * beta + buf_input_cap - parent->capacitance)/beta;
/*						double p_cap = 2*MAX(cap_1+bb1*beta, cap_2+bb2*beta);
 						int tt = (int)((LAMBADA * beta + buf_input_cap - p_cap)/beta);*/
 						if(tt<0){
 							tt = 0;
 							parent->duplicate_first_buf = 1;
 							printf("NOOOO!\n");
 						}
 						parent->delta_length_buf = LAMBADA - tt;
/*						if(parent->capacitance>(2*CAP_LIMIT))
							parent->delta_length_buf = LAMBADA;
						else if(parent->capacitance>CAP_LIMIT)
							parent->delta_length_buf = 2*(2*CAP_LIMIT - parent->capacitance)/beta;
						else
							parent->delta_length_buf = (CAP_LIMIT - parent->capacitance)/beta;
						if(parent->delta_length_buf>LAMBADA)*/
/* 						if(parent->capacitance > CAP_LIMIT)
							parent->delta_length_buf = LAMBADA;
						if(parent->delta_length_buf > LAMBADA)
							parent->delta_length_buf = LAMBADA;*/
						parent->downstream_buf_num = MAX(n1->downstream_buf_num+i, n2->downstream_buf_num+j);
						
// 						if(n1->capacitance > CAP_LIMIT && i>0)
//						if(i>0 && n1->capacitance > CAP_LIMIT)
/*						if(i>0 && n1->downstream_buf_num == 0)
							n1->duplicate_first_buf = 1;
						else
							n1->duplicate_first_buf = 0;
						
						if(j>0 && n2->downstream_buf_num == 0)
							n2->duplicate_first_buf = 1;
						else
							n2->duplicate_first_buf = 0;*/
//						n1->duplicate_first_buf = 0;
//						n2->duplicate_first_buf = 0;
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
/*		if(flag){
			return 0;
		}
		else{
			return distance;
		}*/
/*		if(factor1 < 0.6 && factor2 <0.6 ){
			
			printf("haha r1 %d, r2 %d %d\ndelay 1 %f, delay 2 %f\n", reduntant1, reduntant2, distance,n1->to_sink_delay,n2->to_sink_delay );
			factor1 = 1.0;
			factor2 = 1.0;
			continue;
		}*/
// 		printf("haha %f %f\n", factor1,factor2);
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
/*		if(factor2<0.1){
			printf("%f %f\n",factor1, factor2);
			exit(0);
		}*/
	}

}

void merge(DME_NODE * n1, DME_NODE * n2, DME_NODE * parent, int distance, NODE *p, int len, double delta_delay){
/*	printf("%f %f %f %f\n",n1->lower,n1->upper,n1->left,n1->right);
	printf("%f %f %f %f\n",n2->lower,n2->upper,n2->left,n2->right);*/
	int b1,b2;
	double alpha = wirelib.lib[0].r;
	double beta = wirelib.lib[0].c;
	int k1, k2;
	if(ELMORE == 1){
		
		b1 = ballance_point(n1,n2,parent,distance,delta_delay);
		b2 = distance - b1;

	}else{
		parent->weight = (distance + n1->weight+n2->weight)/2;
		b1 = (int)((distance - (n1->weight-n2->weight))/2);
		b2 = (int)((distance + (n1->weight-n2->weight))/2);
	}
	
	

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
		parent->lower = MAX(n1->lower - b1, n2->lower - b2);
		parent->upper = MIN(n1->upper + b1, n2->upper + b2);
		parent->left = MAX(n1->left - b1, n2->left - b2);
		parent->right = MIN(n1->right + b1, n2->right + b2);
		parent->x1 = (parent->lower + parent->left)/2;
		parent->y1 = (parent->lower - parent->left)/2;
		parent->x2 = (parent->upper + parent->right)/2;
		parent->y2 = (parent->upper - parent->right)/2;
		
		if(point_in_path(parent->x1, parent->y1, p[0],p[1]) == TRUE){
			segment_merged(parent,TRUE);
		}
		else{	
			segment_merged(parent,FALSE);
		}
		
		if((p[0].x == n1->x1 && p[0].y == n1->y1) || (p[0].x == n1->x2 && p[0].y == n1->y2)){
			parent->pleft = n1;
			parent->pright= n2;
			
		}else{
			parent->pleft = n2;
			parent->pright= n1;
			printf("dsfsadf\n");
			exit(0);
		}
		parent->left_direction = p[0].dir_from;
		parent->right_direction = p[0].dir_to;
		parent->sink_index = -1;
		parent->blockage_node = 0;
		parent->visited = 0;
		n1->visited = 1;
		n2->visited = 1;
		return;
	}
	int i,j,k;
	int bb;
	DME_NODE *head, *tail, *next;
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
		DME_NODE *n = init_dme_blockage_node(p[i]);
		n->left_direction = p[i-1].dir_from;
		n->pleft = next;
		next = n;
	}
	parent->pleft = next;
	parent->left_direction = p[k].dir_from;
	next = tail;
	for(i=len-2;i>k;i--){
		DME_NODE *n = init_dme_blockage_node(p[i]);
		n->left_direction = p[i].dir_to;
		n->pleft = next;
		next = n;
	}
	
	parent->pright = next;
	parent->right_direction = p[k].dir_to;
	
	parent->lower = MAX(n1->lower - b1, n2->lower - b2);
	parent->upper = MIN(n1->upper + b1, n2->upper + b2);
	parent->left = MAX(n1->left - b1, n2->left - b2);
	parent->right = MIN(n1->right + b1, n2->right + b2);
	parent->x1 = (parent->lower + parent->left)/2;
	parent->y1 = (parent->lower - parent->left)/2;
	parent->x2 = (parent->upper + parent->right)/2;
	parent->y2 = (parent->upper - parent->right)/2;

	if(point_in_path(parent->x1, parent->y1, p[k],p[k+1]) == TRUE){
		segment_merged(parent,TRUE);
	}
	else{	
		segment_merged(parent,FALSE);
	}

// 	printf("a: %d %d %d %d %d %d\n",parent->node_id,len,parent->x1,parent->y1,parent->x2,parent->y2);
	
	parent->sink_index = -1;
	parent->blockage_node = 0;
	parent->visited = 0;
	n1->visited = 1;
	n2->visited = 1;
	return;
	
}

void output_dme_node(FILE *fp, DME_NODE *n){
	if(n==NULL)
		return;
	if(n->sink_index>0){
		draw_point(fp , n->select_x, n->select_y, 0, BLUE);
	}
	else
		draw_point(fp , n->select_x, n->select_y, 0, RED);
	double factor , upleft_x, upleft_y;
	if (frame.ur.x != 0)
		factor = (double)9500 / frame.ur.x;

	if (factor > 9500 / frame.ur.y)
		factor = (double)9500 / frame.ur.y;	
	upleft_x = n->select_x*factor + OFFSET;
	upleft_y = n->select_y*factor + OFFSET;
// 	fprintf(fp,"4 0 0 50 -1 0 12 0.0000 4 150 315 %.0f %.0f %d\\001\n",upleft_x,upleft_y,n->node_id);
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
	
/*	for (i=0; i < (length); i++)
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
		continue;
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
	}*/
	
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

void connect_to_source(DME_NODE * n1, DME_NODE * parent){
	NODE p[100];
	int length,min_i;
	int distance = cal_distance(n1,parent);
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
	find_path(min_i, ret,&p, &length);
// 	DME_NODE *head = n1;
	DME_NODE *next = n1;
	
	for(i=1;i<length-1;i++){
		DME_NODE *n = init_dme_blockage_node(p[i]);
		n->left_direction = p[i].dir_to;
		n->pleft = next;
		next = n;
	}
//   	printf("xxxxxxxxxxxxxx %d %d %d %d\n",p[0].x,p[0].y,p[0].dir_from, p[0].dir_to);
//   	printf("xxxxxxxxxxxxxx %d %d %d %d\n",p[length-1].x,p[length-1].y,p[length-1].dir_from, p[length-1].dir_to);

	parent->pleft = next;
// 	parent->left_direction = p[length-1].dir_from;
	parent->left_direction = p[0].dir_to;
	parent->pright = NULL;
	int first_buf = LAMBADA3 - HIGHWAY*(int)(buflib.lib[0].icap/wirelib.lib[0].c);
	if(first_buf < 0){
		printf("shit!!\n");
		first_buf = 0;
	}
	int k = ceil(1.0f*(distance-first_buf)/LAMBADA3);
	
	printf("%d %d %d\n",k,first_buf,distance);
	k++;
//	k++;
	if(distance < first_buf){
		k = 1;
	}
	if((k + n1->downstream_buf_num)%2 == 0){
		k++;
	}
	if(k!=1)
		n1->factor = 1.0f*distance/(k-1)/LAMBADA3;
	else
		n1->factor = 1;
	//n1->factor = 1.0f * (k-2) / (k);
	n1->duplicate_first_buf = 0;
	n1->buf_num = k;
	n1->reduntant = 0;
//	n1->factor = 1.0f;
	n1->delta_length_buf = LAMBADA3;
	printf("length is %d %d %d\n",length,n1->buf_num, first_buf);
}

void trace_back(DME_NODE * n, DME_NODE * parent){
	if(n==NULL)
		return;
// 	printf("? %d %f %f\n",n->blockage_node,n->select_x,n->select_y);
	//printf("xxxxx %f\n", dis);
	if(n->blockage_node == 0 && parent != NULL){
		double dis = cal_distance(n,parent);
/*		printf("%.0f %.0f %.0f %.0f\n",parent->left,parent->right,parent->lower,parent->upper);
		printf("%.0f %.0f %.0f %.0f\n",n->left,n->right,n->lower,n->upper);
		printf("%.0f %.0f %.0f %.0f\n",parent->x1,parent->y1,parent->x2,parent->y2);
		printf("%.0f %.0f %.0f %.0f\n",n->x1,n->y1,n->x2,n->y2);*/
		n->sx = (MAX(n->lower, parent->sx - dis) + MIN(n->upper, parent->sx + dis))/2;
		n->sy = (MAX(n->left, parent->sy - dis) + MIN(n->right, parent->sy + dis))/2;
//    		printf("%d %.0f %.0f %.0f %.0f %.0f\n",n->node_id,parent->select_x, parent->select_y, n->select_x,n->select_y,dis);
	}
	trace_back(n->pleft, n);
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

BOOL ngspice_test(DME_NODE * n, double *delta_delay){
double ltree,rtree;
BUF_NODE ** OBUF;
DME_TREE_NODE * OT;
DME_TREE_NODE ** OTmap ; 	
	source_node->pleft = n;
	source_node->left_direction = RIGHT;
	source_node->pright = NULL;
	n->duplicate_first_buf = 0;
	n->buf_num = 0;
	n->reduntant = 0;
	trace_back(n,source_node);
	coordinate_translate(n);
	subtree_call_ngspice(source_node,&ltree,&rtree);
	*delta_delay =  ltree - rtree;
// 	printf("%f %f %f\n", ltree, rtree, *delta_delay);
	// insert_buf(n);
	if(ABS(*delta_delay) > 0.1)
		return TRUE;

	return FALSE;
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
	gen_inputfile(ifp,OT,OTmap);
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
	
	source_node->blockage_node = 0;
	source_node->sink_index = -1;
	NODE p[100];
	NODE min_p[100];
	int current_level = 0;
	int c_k;
	while(count<(length-1)){
		int min_dis=INFINITE;
		int dis;
		int min_i,min_j;
		int len,min_len;
		double cost,min_cost=INFINITE;
		
		for(i=static_num;i<g_size;i++){
			if(g_occupy[i] == FALSE)
				continue;
			for(j=i+1;j<g_size;j++){
				if(g_occupy[j] == FALSE)
					continue;
				if(mapping[i] == mapping[j])
					continue;
// 				printf("%d %d %d %d %d\n", i,j,L[i].level,L[j].level,current_level);
//				if(L[i].level>current_level || L[j].level>current_level)
//					continue;

//				dis = cal_distance(&L[i],&L[j],1, &p,&len);
				dis = pairs[i][j];
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
		int L_min_i = mapping[min_i], L_min_j = mapping[min_j];
		if(L_min_i > L_min_j){
			int temp = L_min_i;
			L_min_i = L_min_j;
			L_min_j = temp;
			temp = min_i;
			min_i = min_j;
			min_j = temp;
		}
//  		printf("%d %d\n",L_min_i, L_min_j);
		find_path(min_i, min_j, &min_p, &min_len);
//		int kk=length,k_total=0,prev=0;
//		for(c_k=0;c_k<=current_level;c_k++){
//			int tmp= (kk+prev)/2;
//			prev = (kk+prev)%2;
//			kk = tmp;
//			k_total += tmp;
//		}
//		if((k_total-1)<=count)
//			current_level++;
// 		printf("x  %d %d %d\n",k_total,current_level,count);
//		L[length+count].node_id = length+count;
		
		double delta_delay = 0,delta_before = 0, delay_prev = 10, min_delay=999,min_before;
		int kkkk=0;
		do{		
			if((delta_delay * delay_prev < 0 && ABS(ABS(delta_delay) - ABS(delay_prev)) < 0.1) || kkkk>BIG_NUM)
				break;
			delay_prev = delta_delay;
			delta_before = delta_before+delta_delay;
			if(kkkk!=0 &&( (ABS(min_delay)) > (ABS(delta_delay)))){
				min_delay = delta_delay;
				min_before = delta_before-delta_delay;
			}
			merge(&L[L_min_j],&L[L_min_i],&L[length+count],min_dis,&min_p,min_len, delta_before);
			if(kkkk++!=0)
				printf("%d: %f %f\n",kkkk, delta_before,delta_delay);
		}
		while(ngspice_test(&L[length+count],&delta_delay) == TRUE);
//			merge(&L[L_min_j],&L[L_min_i],&L[length+count],min_dis,&min_p,min_len, delta_before);
		if(kkkk == BIG_NUM+1){
			merge(&L[L_min_j],&L[L_min_i],&L[length+count],min_dis,&min_p,min_len, min_before);
		}

		L[length+count].node_id = length+count;

		int k1,other1=-1,other2=-1;
		for(k1=static_num;k1<g_size;k1++){
			if( k1 != min_i && mapping[k1]==mapping[min_i])
				other1 = k1;
			if( k1 != min_j && mapping[k1]==mapping[min_j])
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
//		L[length+count].level = MAX(L[L_min_i].level, L[L_min_j].level)+1;
// 		printf("%d %d %d %d %d\n",count, L_min_i,L_min_j,min_dis,min_len);
		count++;
// 		printf("\n");
	}
	connect_to_source(&L[2*length-2],source_node);
/*	for(i=0;i<2*length-1;i++){
		printf("%d %.0f %.0f %.0f %.0f\n",L[i].node_id,L[i].lower,L[i].left, L[i].upper,L[i].right);
		printf("%d %.0f %.0f %.0f %.0f\n",L[i].node_id,L[i].x1,L[i].y1, L[i].x2,L[i].y2);
	}*/
// 	trace_back(&L[2*length-2], source_node);
	trace_back(source_node,NULL);

	coordinate_translate(source_node);
}

void buf_count(DME_NODE * n, int p_num){
	if(n==NULL)
		return;
	if(n->blockage_node == 0)
		n->upstream_buf_num = p_num + n->buf_num;// + n->duplicate_first_buf;
	buf_count(n->pleft, n->upstream_buf_num);
	buf_count(n->pright,n->upstream_buf_num);
/*	if(n->sink_index != -1)
		printf("sink %d buffer number is %d\n", n->sink_index, n->upstream_buf_num);*/
}
DME_NODE* deferred_merge_embedding(){
	int i;

	DME_NODE * L = (DME_NODE *) malloc (sizeof(DME_NODE) * (2*sink.num));
	
	//constructg(&blockage);
	construct_g_all(&blockage,&sink);
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
		L[i].buf_num = 0;
		L[i].blockage_node = 0;
		L[i].downstream_buf_num = 0;
		L[i].factor = 1.0;
		L[i].capacitance = (double)(sink.pool[i].lc);
		L[i].pleft=NULL;
		L[i].pright=NULL;
		L[i].reduntant = 0;
		L[i].node_id=i;
		L[i].level = 0;
		L[i].duplicate_first_buf=0;
	}
//	get_init_delay(L);
//	L[2].to_sink_delay = 200;
	dme_core(L, length);
	buf_count(&L[length*2-2],0);
//	check_DME(&L[2*length-2]);
//	print_fig_1(L,length*2-1);
	free_all();

	return L;

}

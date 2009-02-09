#include "ds.h"
#include "stdio.h"
#include<math.h>




extern BOX frame	; 
extern SOURCE source  ;
extern SINK sink	; 
extern WIRELIB wirelib;
extern BUFLIB	buflib ; 
extern VDDLIB	vddlib ; 
extern int SlewLimit; 
extern int CapLimit ;
extern BLOCKAGE blockage;




double blockageManhattan(double x1, double y1, double x2, double y2, int *blockage_index, int *direction){
	int i;
	double max_detour=0,detour;
	int t_direction;
	int flag = 0;
	*blockage_index=-1;
	for(i=0;i<blockage.num;i++){
		detour = -1;
		double width = (double) blockage.pool[i].ur.x - blockage.pool[i].ll.x;
		double height = (double) blockage.pool[i].ur.y - blockage.pool[i].ll.y;
		if(width > LAMBADA && MIN(x1,x2) <= (double)blockage.pool[i].ll.x && MAX(x1,x2) >= (double) blockage.pool[i].ur.x && MIN(y1,y2) > (double)blockage.pool[i].ll.y && MAX(y1,y2) < (double)blockage.pool[i].ur.y){
			detour = 2 * MIN(((double)blockage.pool[i].ur.y - MAX(y1,y2)), (MIN(y1,y2) - (double)blockage.pool[i].ll.y));
			if(((double)blockage.pool[i].ur.y - MAX(y1,y2)) > (MIN(y1,y2) - (double)blockage.pool[i].ll.y))
				t_direction = 1;
			else
				t_direction = 3;
		}
		else if(height > LAMBADA && MIN(y1,y2) <= (double)blockage.pool[i].ll.y && MAX(y1,y2) >= (double)blockage.pool[i].ur.y && MIN(x1,x2) > (double)blockage.pool[i].ll.x && MAX(x1,x2) < (double)blockage.pool[i].ur.x){
			detour = 2 * MIN(((double)blockage.pool[i].ur.x - MAX(x1,x2)), (MIN(x1,x2) - (double)blockage.pool[i].ll.x));
			if(((double)blockage.pool[i].ur.x - MAX(x1,x2)) > (MIN(x1,x2) - (double)blockage.pool[i].ll.x))
				t_direction = 2;
			else
				t_direction = 4;
		}
		if((detour != -1) && max_detour < detour){
			flag = 1;
			max_detour = detour;
			*blockage_index = i;
			*direction = t_direction;
		}
	}
	//printf("xxxxx %.0f %.0f %.0f %.0f %.0f %d\n",flag==1?min_detour:0,x1,y1,x2,y2, *blockage_index);
	return Manhattan(x1,y1,x2,y2) + (flag==1?max_detour:0);
}


double cal_distance(DME_NODE * n1, DME_NODE * n2, int blockage_aware, int *blockage_index, int *direction){
	int blockage_index1, blockage_index2, blockage_index3, blockage_index4;
	int direction1, direction2, direction3, direction4;
	if(blockage_aware && (MIN(n1->x1,n1->x2) > MAX(n2->x1,n2->x2) || MIN(n2->x1,n2->x2) > MAX(n1->x1,n1->x2) || MIN(n2->y1,n2->y2) > MAX(n1->y1,n1->y2) || MIN(n1->y1,n1->y2) > MAX(n2->y1,n2->y2)))
	{
		*blockage_index = -1;
		double d1 = blockageManhattan(n1->x1, n1->y1, n2->x1, n2->y1,&blockage_index1, &direction1);
		double d2 = blockageManhattan(n1->x1, n1->y1, n2->x2, n2->y2,&blockage_index2, &direction2);
		double d3 = blockageManhattan(n1->x2, n1->y2, n2->x1, n2->y1,&blockage_index3, &direction3);
		double d4 = blockageManhattan(n1->x2, n1->y2, n2->x2, n2->y2,&blockage_index4, &direction4);
		double min1 = MIN(d1,d2);
		double min2 = MIN(d3,d4);
		double min = MIN(min1, min2);
		//printf("%f %f %f %f %f\n",d1,d2,d3,d4,min);
		if(min == d1){
			*blockage_index = blockage_index1;
			*direction = direction1;
		}
		else if(min == d2){
			*blockage_index = blockage_index2;
			*direction = direction2;
		}
		else if(min == d2){
			*blockage_index = blockage_index3;
			*direction = direction3;
		}else{
			*blockage_index = blockage_index4;
			*direction = direction4;
		}

		double t1 = MAX(n1->lower,n2->lower) - MIN(n1->upper, n2->upper);
		double t2 = MAX(n1->left,n2->left) - MIN(n1->right, n2->right);
		double max = MAX(t1,t2);

		if(min != max){
		//	printf(" XXXXXXXXXXXX %f %f\n", min, max);
		}
		return min;
	}
	else{
		double t1 = MAX(n1->lower,n2->lower) - MIN(n1->upper, n2->upper);
		double t2 = MAX(n1->left,n2->left) - MIN(n1->right, n2->right);
		return MAX(t1,t2);
	}
/*	printf("%f %f %f %f\n",n1->lower,n1->upper,n1->left,n1->right);
	printf("%f %f %f %f\n",n2->lower,n2->upper,n2->left,n2->right);
	printf("%f %f %f %f\n",(n1->lower+n1->left)/2,(n1->lower-n1->left)/2,(n1->upper+n1->right)/2,(n1->upper-n1->right)/2);
	printf("%f %f %f %f\n",(n2->lower+n2->left)/2,(n2->lower-n2->left)/2,(n2->upper+n2->right)/2,(n2->upper-n2->right)/2);
	
	printf("distance is %f\n", MAX(t1,t2));*/
	
// 	double d1 = point_distance(n1->x1, n1->y1, n2->x1, n2->y1);
// 	double d2 = point_distance(n1->x1, n1->y1, n2->x2, n2->y2);
// 	double d3 = point_distance(n1->x2, n1->y2, n2->x1, n2->y1);
// 	double d4 = point_distance(n1->x2, n1->y2, n2->x2, n2->y2);
/*	double max = MAX(t1,t2);
	if(min != max){
		printf("bug XXXXXXXXXXXX %f %f\n", min, max);
	//	exit(0);
	}
	printf("\n");
	return MAX(t1,t2);*/
	
}

void handle_blockage_dme_node(DME_NODE * n1, DME_NODE * n2, DME_NODE * parent, DME_NODE * nb1, DME_NODE * nb2, double b1, double b2, int blockage_index, double middle){
	double dd1 = cal_distance(n1,nb1,0,NULL,NULL);
	double dd2 = cal_distance(n2,nb2,0,NULL,NULL);
	//printf("%f %f %f %f %f\n",dd1,dd2,middle,nb1->x1,nb2->x1);
	if(b1<=dd1){
// 		printf("here 1\n");

		parent->lower = MAX(n1->lower - b1, nb1->lower - dd1 + b1);
		parent->upper = MIN(n1->upper + b1, nb1->upper + dd1 - b1);
		parent->left = MAX(n1->left - b1, nb1->left - dd1 + b1);
		parent->right = MIN(n1->right + b1, nb1->right + dd1 - b1);
		parent->x1 = (parent->lower + parent->left)/2;
		parent->y1 = (parent->lower - parent->left)/2;
		parent->x2 = (parent->upper + parent->right)/2;
		parent->y2 = (parent->upper - parent->right)/2;
		parent->pleft = n1;
		parent->pright = nb1;
		nb1->pleft = nb2;
		nb2->pleft = n2;
	}else if(b1 <= (dd1+middle)){
// 		printf("here 2\n");
		if(blockage.pool[blockage_index].ur.y - blockage.pool[blockage_index].ll.y == middle){
			parent->x1 = parent->x2 = nb1->x1;
			parent->y1 = parent->y2 = b1 -dd1 + nb1->y1;
		}else{
			parent->x1 = parent->x2 = b1 -dd1 + nb1->x1;
			parent->y1 = parent->y2 = nb1->y1;
		}
		parent->lower = parent->upper = parent->x1 + parent->y1;
		parent->left = parent->right = parent->x1 - parent->y1;
		parent->pleft = nb1;
		parent->pright = nb2;
		nb1->pleft = n1;
		nb2->pleft = n2;
	}
	else{
// 		printf("here 3\n");
		parent->lower = MAX(n2->lower - b2, nb2->lower - dd2 + b2);
		parent->upper = MIN(n2->upper + b2, nb2->upper + dd2 - b2);
		parent->left = MAX(n2->left - b2, nb2->left - dd2 + b2);
		parent->right = MIN(n2->right + b2, nb2->right + dd2 - b2);
		parent->x1 = (parent->lower + parent->left)/2;
		parent->y1 = (parent->lower - parent->left)/2;
		parent->x2 = (parent->upper + parent->right)/2;
		parent->y2 = (parent->upper - parent->right)/2;
		parent->pleft = n2;
		parent->pright = nb2;
		nb2->pleft = nb1;
		nb1->pleft = n1;
	}


}

void remove_blocked(DME_NODE * n){
	int i;
	for(i=0;i<blockage.num;i++){
 	//	printf("%d %d %d %d\n",blockage.pool[i].ll.x,blockage.pool[i].ll.y,blockage.pool[i].ur.x,blockage.pool[i].ur.y);
	}

}

void merge(DME_NODE * n1, DME_NODE * n2, DME_NODE * parent, double distance, int blockage_index, int direction){
/*	printf("%f %f %f %f\n",n1->lower,n1->upper,n1->left,n1->right);
	printf("%f %f %f %f\n",n2->lower,n2->upper,n2->left,n2->right);*/
	double b1,b2;
	double alpha = wirelib.lib[0].r;
	double beta = wirelib.lib[0].c;
	if(ELMORE == 1){
		b1 = ((n2->to_sink_delay - n1->to_sink_delay) + alpha*distance*(n2->capacitance + 0.5*beta*distance))/alpha/(n1->capacitance + n2->capacitance + beta*distance);
		b2 = distance - b1;
	}else{
		b1 = (distance - (n1->weight-n2->weight))/2;
		b2 = (distance + (n1->weight-n2->weight))/2;
	}
	//cal_distance();
	//printf("%d %d %f %f %f\n",blockage_index,direction,b1,b2,distance);
		
	if(b1<0 || b2<0){
		printf("Oh No!\n");
		printf("%f %f %f %f %f\n", b1,b2,distance,n1->weight,n2->weight);
		//return;
	}
	
	if(blockage_index == -1){
		parent->lower = MAX(n1->lower - b1, n2->lower - b2);
		parent->upper = MIN(n1->upper + b1, n2->upper + b2);
		parent->left = MAX(n1->left - b1, n2->left - b2);
		parent->right = MIN(n1->right + b1, n2->right + b2);
		parent->x1 = (parent->lower + parent->left)/2;
		parent->y1 = (parent->lower - parent->left)/2;
		parent->x2 = (parent->upper + parent->right)/2;
		parent->y2 = (parent->upper - parent->right)/2;
		// 	
		parent->pleft = n1;
		parent->pright= n2;
/*		parent->visited = 0;
		parent->weight = (distance + n1->weight+n2->weight)/2;
		parent->to_sink_delay = alpha*b1*(0.5*beta*b1+n1->capacitance) + n1->to_sink_delay;
		parent->capacitance = n1->capacitance + n2->capacitance + distance*beta;
		n1->visited = 1;
		n2->visited = 1;*/
	}
	else{
		DME_NODE * nb1 = (DME_NODE *) malloc (sizeof(DME_NODE));
		DME_NODE * nb2 = (DME_NODE *) malloc (sizeof(DME_NODE));
		nb1->pright = NULL;
		nb2->pright = NULL;
		nb1->blockage_node = 1;
		nb2->blockage_node = 1;
		nb1->sink_index = -1;
		nb2->sink_index = -1;

		if(direction == 1){
			nb1->x1 = nb1->x2 = (double)blockage.pool[blockage_index].ll.x;
			nb1->y1 = nb1->y2 = (double)blockage.pool[blockage_index].ll.y;
			nb1->select_x = nb1->lower = nb1->upper = nb1->x1 + nb1->y1;
			nb1->select_y = nb1->left = nb1->right = nb1->x1 - nb1->y1;
			nb2->x1 = nb2->x2 = (double)blockage.pool[blockage_index].ur.x;
			nb2->y1 = nb2->y2 = (double)blockage.pool[blockage_index].ll.y;
			nb2->select_x = nb2->lower = nb2->upper = nb2->x1 + nb2->y1;
			nb2->select_y = nb2->left = nb2->right = nb2->x1 - nb2->y1;
			//printf("%f %f\n",n1->x1,n2->x1);
			if(n1->x1 < n2->x1)
				handle_blockage_dme_node(n1,n2,parent,nb1,nb2,b1,b2,blockage_index,(double)blockage.pool[blockage_index].ur.x-(double)blockage.pool[blockage_index].ll.x);
			else
				handle_blockage_dme_node(n2,n1,parent,nb1,nb2,b2,b1,blockage_index,(double)blockage.pool[blockage_index].ur.x-(double)blockage.pool[blockage_index].ll.x);
				
		}
		else if(direction == 3){
			nb1->x1 = nb1->x2 = (double)blockage.pool[blockage_index].ll.x;
			nb1->y1 = nb1->y2 = (double)blockage.pool[blockage_index].ur.y;
			nb1->select_x = nb1->lower = nb1->upper = nb1->x1 + nb1->y1;
			nb1->select_y = nb1->left = nb1->right = nb1->x1 - nb1->y1;
			nb2->x1 = nb2->x2 = (double)blockage.pool[blockage_index].ur.x;
			nb2->y1 = nb2->y2 = (double)blockage.pool[blockage_index].ur.y;
			nb2->select_x = nb2->lower = nb2->upper = nb2->x1 + nb2->y1;
			nb2->select_y = nb2->left = nb2->right = nb2->x1 - nb2->y1;
			if(n1->x1 < n2->x1)
				handle_blockage_dme_node(n1,n2,parent,nb1,nb2,b1,b2,blockage_index,(double)blockage.pool[blockage_index].ur.x-(double)blockage.pool[blockage_index].ll.x);
			else
				handle_blockage_dme_node(n2,n1,parent,nb1,nb2,b2,b1,blockage_index,(double)blockage.pool[blockage_index].ur.x-(double)blockage.pool[blockage_index].ll.x);
			
		}
		else if(direction == 2){
			nb1->x1 = nb1->x2 = (double)blockage.pool[blockage_index].ll.x;
			nb1->y1 = nb1->y2 = (double)blockage.pool[blockage_index].ll.y;
			nb1->select_x = nb1->lower = nb1->upper = nb1->x1 + nb1->y1;
			nb1->select_y = nb1->left = nb1->right = nb1->x1 - nb1->y1;
			nb2->x1 = nb2->x2 = (double)blockage.pool[blockage_index].ll.x;
			nb2->y1 = nb2->y2 = (double)blockage.pool[blockage_index].ur.y;
			nb2->select_x = nb2->lower = nb2->upper = nb2->x1 + nb2->y1;
			nb2->select_y = nb2->left = nb2->right = nb2->x1 - nb2->y1;
			if(n1->y1 < n2->y1)
				handle_blockage_dme_node(n1,n2,parent,nb1,nb2,b1,b2,blockage_index,(double)blockage.pool[blockage_index].ur.y-(double)blockage.pool[blockage_index].ll.y);
			else
				handle_blockage_dme_node(n2,n1,parent,nb1,nb2,b2,b1,blockage_index,(double)blockage.pool[blockage_index].ur.y-(double)blockage.pool[blockage_index].ll.y);
			
		}else if(direction == 4){
			nb1->x1 = nb1->x2 = (double)blockage.pool[blockage_index].ur.x;
			nb1->y1 = nb1->y2 = (double)blockage.pool[blockage_index].ll.y;
			nb1->select_x = nb1->lower = nb1->upper = nb1->x1 + nb1->y1;
			nb1->select_y = nb1->left = nb1->right = nb1->x1 - nb1->y1;
			nb2->x1 = nb2->x2 = (double)blockage.pool[blockage_index].ur.x;
			nb2->y1 = nb2->y2 = (double)blockage.pool[blockage_index].ur.y;
			nb2->select_x = nb2->lower = nb2->upper = nb2->x1 + nb2->y1;
			nb2->select_y = nb2->left = nb2->right = nb2->x1 - nb2->y1;
			if(n1->y1 < n2->y1)
				handle_blockage_dme_node(n1,n2,parent,nb1,nb2,b1,b2,blockage_index,(double)blockage.pool[blockage_index].ur.y-(double)blockage.pool[blockage_index].ll.y);
			else
				handle_blockage_dme_node(n2,n1,parent,nb1,nb2,b2,b1,blockage_index,(double)blockage.pool[blockage_index].ur.y-(double)blockage.pool[blockage_index].ll.y);
			
		}else{
			printf("%d %d error\n", blockage_index, direction);
			//exit(0);
		}

	}
	parent->sink_index = -1;
	parent->blockage_node = 0;
	parent->visited = 0;
	parent->weight = (distance + n1->weight+n2->weight)/2;
	parent->to_sink_delay = alpha*b1*(0.5*beta*b1+n1->capacitance) + n1->to_sink_delay;
	parent->capacitance = n1->capacitance + n2->capacitance + distance*beta;
	n1->visited = 1;
	n2->visited = 1;
	remove_blocked(parent);
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
/*	
	for ( i = 0 ; i < blockage.num ; i++){
		upleft_x = (double) (blockage.pool[i].ll.x);
		upleft_y = (double) (blockage.pool[i].ll.y);
		downright_x = (double) (blockage.pool[i].ur.x);
		downright_y = (double) (blockage.pool[i].ur.y);
		draw_wire(fp,upleft_x,upleft_y,downright_x,downright_y, Dashed, RED);
		draw_wire(fp,downright_x,downright_y,upleft_x,upleft_y,Dashed, RED);
	}
*/
	fclose(fp);
}

void print_fig_2(DME_NODE * L, int length){
	FILE* fp;
	char * filename = "t2.fig";
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
		
		t1_x = (L[i].pleft->lower+L[i].pleft->left)/2;
		t1_y = (L[i].pleft->lower-L[i].pleft->left)/2;
		t2_x = (L[i].pleft->upper+L[i].pleft->right)/2;
		t2_y = (L[i].pleft->upper-L[i].pleft->right)/2;
		upleft_x = (t1_x+t2_x)/2;
		upleft_y = (t1_y+t2_y)/2;
		t1_x = (L[i].pright->lower+L[i].pright->left)/2;
		t1_y = (L[i].pright->lower-L[i].pright->left)/2;
		t2_x = (L[i].pright->upper+L[i].pright->right)/2;
		t2_y = (L[i].pright->upper-L[i].pright->right)/2;
		downright_x = (t1_x+t2_x)/2;
		downright_y = (t1_y+t2_y)/2;
		
		upleft_x = upleft_x*factor + OFFSET;
		upleft_y = upleft_y*factor + OFFSET;
		downright_x = downright_x*factor + OFFSET;
		downright_y = downright_y*factor + OFFSET;
		fprintf(fp,"2 1 0 1 0 7 50 -1 -1 0 0 0 -1 0 0 2\n");
		fprintf(fp,"\t%.0f %.0f %.0f %.0f\n",upleft_x,upleft_y,downright_x,downright_y);
		
		
      //		if (i >= general_block_num)
      //			fprintf(fp,"4 0 0 0 0 0 12 0.0000 4 135 180 %.0f %.0f %d\\001\n",(upleft_x+downright_x) / 2 - ID_OFFSET,(upleft_y + downright_y) / 2 + ID_OFFSET,i-general_block_num);
	}
	fclose(fp);
}



double test_distance(DME_NODE * n1, DME_NODE * n2){
		double t1 = MAX(n1->lower,n2->lower) - MIN(n1->upper, n2->upper);
		double t2 = MAX(n1->left,n2->left) - MIN(n1->right, n2->right);
		return MAX(t1,t2);
}

void trace_back(DME_NODE * n, DME_NODE * parent){
	if(n==NULL)
		return;
// 	printf("? %d %f %f\n",n->blockage_node,n->select_x,n->select_y);
	//printf("xxxxx %f\n", dis);
	if(n->blockage_node == 0){
		double dis = cal_distance(n,parent,0,NULL,NULL);
/*		printf("%.0f %.0f %.0f %.0f\n",parent->left,parent->right,parent->lower,parent->upper);
		printf("%.0f %.0f %.0f %.0f\n",n->left,n->right,n->lower,n->upper);
		printf("%.0f %.0f %.0f %.0f\n",parent->x1,parent->y1,parent->x2,parent->y2);
		printf("%.0f %.0f %.0f %.0f\n",n->x1,n->y1,n->x2,n->y2);*/
		n->select_x = (MAX(n->lower, parent->select_x - dis) + MIN(n->upper, parent->select_x + dis))/2;
		n->select_y = (MAX(n->left, parent->select_y - dis) + MIN(n->right, parent->select_y + dis))/2;
//  		printf("%d %.0f %.0f %.0f %.0f %.0f\n",n->node_id,parent->select_x, parent->select_y, n->select_x,n->select_y,dis);
	}
	trace_back(n->pleft, n);
	trace_back(n->pright, n);
}

void coordinate_translate(DME_NODE * n){	
	if(n==NULL)		
		return;	
	double t1,t2;	
	t1 = (n->select_x+n->select_y)/2;	
	t2 = (n->select_x-n->select_y)/2;	
	n->select_x = t1;	
	n->select_y = t2;	
	coordinate_translate(n->pleft);	
	coordinate_translate(n->pright);
}



void dme_core(DME_NODE * L, int length){
	int i,j;
	int count=0;
	DME_NODE * source_node = (DME_NODE *) malloc (sizeof(DME_NODE));
	source_node->lower = 0;
	source_node->upper = 0;
	source_node->left = 0;
	source_node->right = 0;
	source_node->select_x = 0;
	source_node->select_y = 0;
	
	while(count<(length-1)){
		double min_dis=1e20;
		double dis;
		int min_i,min_j;
		int blockage_index=-1, direction;
		int min_blockage_index, min_direction;
		for(i=0;i<(count+length);i++){
			if(L[i].visited == 1)
				continue;
			for(j=i+1;j<(count+length);j++){
				if(L[j].visited == 1)
					continue;
				dis = cal_distance(&L[i],&L[j],1,&blockage_index,&direction);
				//dis =test_distance(&L[i],&L[j]);
				if(min_dis>dis){
					min_dis = dis;
					min_i = i;
					min_j = j;
					min_blockage_index = blockage_index;
					min_direction = direction;
				}
			}
			
		}
		//printf("%d %d %f\n", min_i,min_j,min_dis);
		L[length+count].node_id = length+count;
		merge(&L[min_i],&L[min_j],&L[length+count],min_dis,min_blockage_index,min_direction);
		
		count++;
// 		printf("\n");
	}
/*	for(i=0;i<2*length-1;i++){
		printf("%d %.0f %.0f %.0f %.0f\n",L[i].node_id,L[i].lower,L[i].left, L[i].upper,L[i].right);
		printf("%d %.0f %.0f %.0f %.0f\n",L[i].node_id,L[i].x1,L[i].y1, L[i].x2,L[i].y2);
	}*/
	trace_back(&L[2*length-2], source_node);

	coordinate_translate(&L[2*length-2]);
}



DME_NODE* deferred_merge_embedding(){
	int i;


	DME_NODE * L = (DME_NODE *) malloc (sizeof(DME_NODE) * (2*sink.num));
/*	DME_TREE_NODE * Tr = NULL ; 

	double *sink_alt_array = (double *) malloc(sizeof(double) * sink.num); 
	double max_path  = 0.0 ; 
	BUF_NODE * bf; 
	buf_i = 2*sink.num -1   ; 
	bufnode = (BUF_NODE **) malloc (sizeof (BUF_NODE *) *(2*sink.num)) ; 

	for ( i = 0 ; i < 2*sink.num  ; i++){
		(*(bufnode+i)) = (BUF_NODE *) malloc (sizeof(BUF_NODE));
		(*(bufnode+i))-> x = (*(bufnode+i))-> y = -1; 
		(*(bufnode+i))->buf_id = -1 ; 
		(*(bufnode+i)) -> next = NULL; 
	}
*/
	
	int length = sink.num;
	for(i=0;i<sink.num; i++){
		//printf("%d %d %d %d\n", sink.pool[i].index,sink.pool[i].x, sink.pool[i].y, sink.pool[i].lc);
		L[i].lower = L[i].upper = (double)(sink.pool[i].x) + (double)(sink.pool[i].y);
		L[i].left = L[i].right = (double)(sink.pool[i].x) - (double)(sink.pool[i].y);
		L[i].x1 = L[i].x2 = (double)(sink.pool[i].x);
		L[i].y1 = L[i].y2 = (double)(sink.pool[i].y);
		L[i].sink_index = sink.pool[i].index;
		L[i].visited = 0;
		L[i].weight = 0;
		L[i].to_sink_delay = 0;
		L[i].blockage_node = 0;
		L[i].capacitance = (double)(sink.pool[i].lc);
		L[i].pleft=NULL;
		L[i].pright=NULL;
		L[i].node_id=i;
	}
	
	dme_core(L, length);
	
	print_fig_1(L,length*2-1);
	return L;

/*	check_DME( L+ 2 * sink.num -2 ) ; 
	

	construct_DME_tree( L, sink.num ,  &Tr) ;
	settle_DME_tree(Tr, 0.0 ); 
	construct_SINK_array( Tr , sink_alt_array); 

	check_DME_tree(Tr) ;
//	construct_DME_map(Tr, DME_tree_map); 
	check_sink_alt_array(sink_alt_array,sink.num);

	max_path = max_sink_alt(sink_alt_array,sink.num); 

	K = ceil( max_path / LAMBADA )+1 ; 
	if ( !(K%2) ) K += 1 ; 
	Buffer_Placement (Tr, K, sink_alt_array, sink.num) ; 

*/	

	// check the DME_tree_map  right ?
	/*
	for ( i = 0 ; i < 2 * sink.num ; i ++) {
		printf(" %d \t %f \n", i , (DME_tree_map[i])->altitude) ; 

	}
	*/


/*

	for ( i = 0 ; i < 2*sink.num  ; i++){
		bf = (*(bufnode+i))->next ; 
		printf(" node %d  ",i);
		while(bf != NULL){
			printf("buffer (%f,%f) ", bf->x, bf->y);
			bf = bf->next ; 
		}
		printf("\n");

	}

*/


	//TODO
	//print_fig_2(L,length*2-1);

}






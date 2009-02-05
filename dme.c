#include "ds.h"
#include "stdio.h"
#define OFFSET 100

extern BOX frame	; 
extern SOURCE source  ;
extern SINK sink	; 
extern WIRELIB wirelib;
extern BUFLIB	buflib ; 
extern VDDLIB	vddlib ; 
extern int SlewLimit; 
extern int CapLimit ; 
extern BLOCKAGE blockage;

typedef struct dme_node{
	double lower;
	double upper; 
	double left;
	double right;
	double weight;
	double select_x;
	double select_y;
	
	int visited;
	struct dme_node * pleft;
	struct dme_node * pright;
}DME_NODE;

double distance(DME_NODE * n1, DME_NODE * n2){
	double t1 = MAX(n1->lower,n2->lower) - MIN(n1->upper, n2->upper);
	double t2 = MAX(n1->left,n2->left) - MIN(n1->right, n2->right);
/*	printf("%f %f %f %f\n",n1->lower,n1->upper,n1->left,n1->right);
	printf("%f %f %f %f\n",n2->lower,n2->upper,n2->left,n2->right);
	printf("%f %f %f %f\n",(n1->lower+n1->left)/2,(n1->lower-n1->left)/2,(n1->upper+n1->right)/2,(n1->upper-n1->right)/2);
	printf("%f %f %f %f\n",(n2->lower+n2->left)/2,(n2->lower-n2->left)/2,(n2->upper+n2->right)/2,(n2->upper-n2->right)/2);
	
	printf("distance is %f\n", MAX(t1,t2));*/
	return MAX(t1,t2);
}

void merge(DME_NODE * n1, DME_NODE * n2, DME_NODE * parent, double distance){
/*	printf("%f %f %f %f\n",n1->lower,n1->upper,n1->left,n1->right);
	printf("%f %f %f %f\n",n2->lower,n2->upper,n2->left,n2->right);*/
	double b1 = (distance - (n1->weight-n2->weight))/2;
	double b2 = (distance + (n1->weight-n2->weight))/2;
	if(b1<0 || b2<0){
		printf("Oh No!\n");
		printf("%f %f %f %f %f\n", b1,b2,distance,n1->weight,n2->weight);
		//return;
	}
/*	Interval ix1,iy1,ix2,iy2;
	ix1->lower = n1->lower - b1;
	ix1->upper = n1->upper + b1;	
	iy1->lower = n1->left - b1;
	iy1->upper = n1->right + b1;
	ix2->lower = n2->lower - b2;
	ix2->upper = n2->upper + b2;
	iy2->lower = n2->left - b2;
	iy2->upper = n2->right + b2;*/
	
	parent->lower = MAX(n1->lower - b1, n2->lower - b2);
	parent->upper = MIN(n1->upper + b1, n2->upper + b2);
	parent->left = MAX(n1->left - b1, n2->left - b2);
	parent->right = MIN(n1->right + b1, n2->right + b2);
	// 	
// 	if((n2->upper + b2) > (n1->upper + b1)){
// 		parent->lower = n2->lower - b2;
// 		parent->upper = n1->upper + b1;
// 	}else{
// 		parent->lower = n1->lower - b1;
// 		parent->upper = n2->upper + b2;
// 	}
// 	
// 	if((n2->right + b2) > (n1->right + b1)){
// 		parent->left = n2->left - b2;
// 		parent->right = n1->right + b1;
// 	}else{
// 		parent->left = n1->left - b1;
// 		parent->right = n2->right + b2;
// 	}
// 	
	parent->pleft = n1;
	parent->pright= n2;
	parent->visited = 0;
	parent->weight = (distance + n1->weight+n2->weight)/2;
	n1->visited = 1;
	n2->visited = 1;
	
}

void print_fig_1(DME_NODE * L, int length){
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

void trace_back(DME_NODE * n, DME_NODE * parent){
	if(n==NULL)
		return;
	double dis = distance(n,parent);
	
	n->select_x = (MAX(n->lower, parent->select_x - dis) + MIN(n->upper, parent->select_x + dis))/2;
	n->select_y = (MAX(n->left, parent->select_y - dis) + MIN(n->right, parent->select_y + dis))/2;
// 	printf("%f %f %f\n",n->select_x,n->select_y,dis);
	
	trace_back(n->pleft, n);
	trace_back(n->pright, n);
}

void coordianate_translate(DME_NODE * L, int length){
	int i;
	double t1,t2;
	for(i=0;i<length;i++){
		t1 = (L[i].select_x+L[i].select_y)/2;
		t2 = (L[i].select_x-L[i].select_y)/2;
		L[i].select_x = t1;
		L[i].select_y = t2;
	}
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
		for(i=0;i<(count+length);i++){
			if(L[i].visited == 1)
				continue;
			for(j=i+1;j<(count+length);j++){
				if(L[j].visited == 1)
					continue;
				dis = distance(&L[i],&L[j]);
				if(min_dis>dis){
					min_dis = dis;
					min_i = i;
					min_j = j;
				}
			}
			
		}
		merge(&L[min_i],&L[min_j],&L[length+count],min_dis);
		count++;
// 		printf("\n");
	}
	trace_back(&L[2*length-2], &source_node);
	coordianate_translate(L, 2*length-1);
}



void deferred_merge_embedding(){
	int i;
	DME_NODE * L = (DME_NODE *) malloc (sizeof(DME_NODE) * (2*sink.num-1));
	int length = sink.num;
	for(i=0;i<sink.num; i++){
		//printf("%d %d %d %d\n", sink.pool[i].index,sink.pool[i].x, sink.pool[i].y, sink.pool[i].lc);
		L[i].lower = L[i].upper = (double)(sink.pool[i].x) + (double)(sink.pool[i].y);
		L[i].left = L[i].right = (double)(sink.pool[i].x) - (double)(sink.pool[i].y);
		L[i].visited = 0;
		L[i].weight = 0;
		L[i].pleft=NULL;
		L[i].pright=NULL;
	}
	
	dme_core(L, length);
	
	print_fig_1(L,length*2-1);
	//TODO
	//print_fig_2(L,length*2-1);

}

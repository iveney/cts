#include <stdio.h>
#include <stdlib.h>
#include "segment.h"
#define Length_Limit 0

// typedef struct NODE{
// 	int x;
// 	int y;
// }NODE;

// typedef struct blockage{
// 	NODE ll;
// 	NODE ur;
// }blockage;


// int main(){
// 	NODE p1;
// 	NODE p2;
// 	int blockage_number =2;
// 	blockage blockage_list->pool[2];
// 
// 	p1.x = 1.0f;
// 	p1.y = 1.0f;
// 	p2.x = 5.0f;
// 	p2.y = 5.0f;
// 
// 	blockage_list->pool[0].ll.x = 0.0f;
// 	blockage_list->pool[0].ll.y = 0.0f;
// 	blockage_list->pool[0].ur.x = 2.0f;
// 	blockage_list->pool[0].ur.y = 2.0f;
// 
// 	blockage_list->pool[1].ll.x = 3.0f;
// 	blockage_list->pool[1].ll.y = 3.0f;
// 	blockage_list->pool[1].ur.x = 4.0f;
// 	blockage_list->pool[1].ur.y = 4.0f;
// 
// 	printf("%f, %f, %f, %f\n",p1.x,p1.y,p2.x,p2.y);
// 
// 	compute_segment(&p1,&p2,blockage_list->pool,blockage_number);
// 
// 	printf("%f, %f, %f, %f\n",p1.x,p1.y,p2.x,p2.y);
// 
// 
// 	return 0;
// }

int compute_distance(NODE *p1, NODE *p2){
	return (p2->x - p1->x); 
}

void compute_segment(NODE *p1, NODE *p2, BLOCKAGE *blockage_list, int blockage_number){
	NODE *p3;
	NODE *adjacent_list;
	int intersection_blockage;
	int distance, temp;
	int i;
	int j;
	int k;
	int flag = 0;
	int direction;
	int first_not_in = 0;

	//make sure that p1 is on the left of p2
	if (p1->x > p2->x){
		p3 = (NODE *) malloc (sizeof(NODE));
		(*p3) = (*p1);
		(*p1) = (*p2);
		(*p2) = (*p3);
		free(p3);
		first_not_in = 1;
	}

	if(p2 ->y > p1->y ) direction =1;
	else direction = -1;
    p2->y = (p2->x - p1->x) * direction + (p1->y);	
	//printf("before %d %d %d %d\n", p1->x, p1->y, p2->x, p2->y);

	for (i = 0; i < blockage_number; i++){
		
		if((blockage_list->pool[i].ur.x - blockage_list->pool[i].ll.x <= Length_Limit) ||
			(blockage_list->pool[i].ur.x - blockage_list->pool[i].ll.x <= Length_Limit))
			continue;

		if (blockage_list->pool[i].ll.x >= (p2->x))
			continue;
		if (blockage_list->pool[i].ur.x  <= (p1->x))
			continue;
		if(direction == 1){
			if(blockage_list->pool[i].ll.y >= (p2->y))
				continue;
			if(blockage_list->pool[i].ur.y <= (p1->y))
				continue;
		}
		else if (direction == -1){
			if(blockage_list->pool[i].ll.y >= (p1->y))
				continue;
			if(blockage_list->pool[i].ur.y <= (p2->y))
				continue;
		}
		//p1 is in the blockage
		if ((blockage_list->pool[i].ll.x < (p1->x)) &&
			(blockage_list->pool[i].ur.x > (p1->x))  &&
			(blockage_list->pool[i].ll.y < (p1->y)) &&
			(blockage_list->pool[i].ur.y > (p1->y))){

			//p2 is in the blockage
			if ((blockage_list->pool[i].ll.x < (p2->x)) &&
				(blockage_list->pool[i].ur.x > (p2->x))  &&
				(blockage_list->pool[i].ll.y < (p2->y)) &&
				(blockage_list->pool[i].ur.y > (p2->y))){
				flag = 1;
				break;
			}
			//p2 isn't in the blockage
			//compute the new p1 NODE
			else{
				p3 = (NODE *) malloc(sizeof(NODE));
				if(direction == -1){
					p3->y = blockage_list->pool[i].ll.y;				
					p3->x = (p3->y - p1->y)*direction + p1->x;
// 					printf("haha %d %d %d %d %d \n",p3->x,p1->x,p1->y,p2->x,p2->y);
				}
				else{
					p3->y = blockage_list->pool[i].ur.y;
					p3->x = (p3->y - p1->y)*direction + p1->x;
				}
				if(p3->x > blockage_list->pool[i].ur.x){
					p3->x = blockage_list->pool[i].ur.x;
					p3->y = (p3->x - p1->x) * direction + p1->y;
				}
				
				(*p1) = (*p3);
				free(p3);
//				p1 = p3;
				
			}
		}

		//p2 is in the blockage
		if ((blockage_list->pool[i].ll.x < (p2->x)) &&
			(blockage_list->pool[i].ur.x > (p2->x))  &&
			(blockage_list->pool[i].ll.y < (p2->y)) &&
			(blockage_list->pool[i].ur.y > (p2->y))){
			
// 			printf("haha p2 is in the blockage\n");

			//p1 is in the blockage
			if ((blockage_list->pool[i].ll.x < (p1->x)) &&
				(blockage_list->pool[i].ur.x > (p1->x))  &&
				(blockage_list->pool[i].ll.y < (p1->y)) &&
				(blockage_list->pool[i].ur.y > (p1->y))){
				flag = 1;
				break;
			}
			//p1 isn't in the blockage
			//compute the new p2 NODE
			else{
/*				printf("haha p1 is not in the blockage\n");
				printf("direction is %d\n",direction);*/
				p3 = (NODE *) malloc(sizeof(NODE));
				if(direction == -1){
					if(p1->x >= blockage_list->pool[i].ll.x){
						if((p1->y <= blockage_list->pool[i].ur.y)){
							(*p3) = (*p1);
						}
						else{
							p3->y = blockage_list->pool[i].ur.y;
							p3->x = (p3->y - p1->y) * direction + p1->x;
						}
					}
					else if(p1->y <= blockage_list->pool[i].ur.y){
						if(p1->x == blockage_list->pool[i].ll.x){
							(*p3) = (*p1);
						}
						else{
							p3->x = blockage_list->pool[i].ll.x;
							p3->y = (p3->x - p1->x) * direction + p1->y;
						}
					}
					else{
						p3->x = blockage_list->pool[i].ll.x;
						p3->y = (p3->x - p1->x) * direction + p1->y;
		
	
						if (p3->y > blockage_list->pool[i].ur.y){
				
					
							p3->y = blockage_list->pool[i].ur.y;
							p3->x = (p3->y - p1->y) * direction + p1->x;
						}
					}

				}
				else{
					if(p1->y >= blockage_list->pool[i].ll.y){
						if((p1->x >= blockage_list->pool[i].ll.x)){
							(*p3) = (*p1);
						}
						else{
							p3->x = blockage_list->pool[i].ll.x;
							p3->y = (p3->x - p1->x) * direction + p1->y;
						}
		
					}
					else if(p1->x >= blockage_list->pool[i].ll.x){
						if(p1->y == blockage_list->pool[i].ll.y){
							(*p3) = (*p1);
						}
						else{
							p3->y = blockage_list->pool[i].ll.y;
							p3->x = (p3->y - p1->y) * direction + p1->x;
						}
		
					}
					else{
						p3->x = blockage_list->pool[i].ll.x;
						p3->y = (p3->x - p1->x) * direction + p1->y;
		
						if (p3->y < blockage_list->pool[i].ll.y)
						{
							p3->y = blockage_list->pool[i].ll.y;
							p3->x = (p3->y - p1->y) * direction + p1->x;
		
						}
					}

				}
				(*p2) = (*p3);
				free(p3);
//				p2 = p3;
			}
		}
	}

	

	//p1 and p2 are in the same blockage
	if(flag == 1){
		(*p2) = (*p1);
//		return;
	}
	
	if(((*p2).x == (*p1).x) && ((*p2).y == (*p1).y)) return;
	

	//printf("huhu %d, %d, %d, %d\n",p1->x,p1->y,p2->x,p2->y);

	//after getting the new p1 and p2, we should compute the adjacent NODE with blockage
	adjacent_list = (NODE *) malloc (sizeof(NODE)*2*blockage_number);
	intersection_blockage=0;
	p3 = (NODE *) malloc (sizeof(NODE));

	for(i = 0 ; i < blockage_number; i++){

		if((blockage_list->pool[i].ur.x - blockage_list->pool[i].ll.x <= Length_Limit) ||
			(blockage_list->pool[i].ur.x - blockage_list->pool[i].ll.x <= Length_Limit))
			continue;

		if (blockage_list->pool[i].ll.x >= (p2->x))
			continue;
		if (blockage_list->pool[i].ur.x  <= (p1->x))
			continue;
		if(direction == 1){
			if(blockage_list->pool[i].ll.y >= (p2->y))
				continue;
			if(blockage_list->pool[i].ur.y <= (p1->y))
				continue;
		}
		else if (direction == -1){
			if(blockage_list->pool[i].ll.y >= (p1->y))
				continue;
			if(blockage_list->pool[i].ur.y <= (p2->y))
				continue;
		}
		//printf("haha\n");
		
		//printf("li %d %d %d %d",p1->x, p1->y, p2->x, p2->y);

		
		if(direction == 1){
			if(p1->y >= blockage_list->pool[i].ll.y){
				if(p1->x >= blockage_list->pool[i].ll.x){
					(*p3) = (*p1);
					adjacent_list[2*intersection_blockage] = (*p3);
				}
				else{
					p3->x = blockage_list->pool[i].ll.x;
					p3->y = (p3->x - p1->x) * direction + p1->y;
	
					if(p3->y >= blockage_list->pool[i].ur.y)
						continue;
					adjacent_list[2*intersection_blockage] = (*p3);
				}
			}
			else if(p1->x >= blockage_list->pool[i].ll.x){
				if(p1->y == blockage_list->pool[i].ll.y){
					(*p3) = (*p1);
					adjacent_list[2*intersection_blockage] = (*p3);
				}
				else{
					p3->y = blockage_list->pool[i].ll.y;
					p3->x = (p3->y - p1->y) * direction + p1->x;
	
					if(p3->x >= blockage_list->pool[i].ur.x)
						continue;
					adjacent_list[2*intersection_blockage] = (*p3);
				}
			}
			else{
				p3->x = blockage_list->pool[i].ll.x;
				p3->y = (p3->x - p1->x) * direction + p1->y;

				if(p3->y >= blockage_list->pool[i].ur.y)
					continue;
				else if (p3->y >= blockage_list->pool[i].ll.y)
					adjacent_list[2*intersection_blockage] = (*p3);
				else{
					p3->y = blockage_list->pool[i].ll.y;
					p3->x = (p3->y - p1->y) * direction + p1->x;

					if(p3->x >= blockage_list->pool[i].ur.x)
						continue;
					else{
						adjacent_list[2*intersection_blockage] = (*p3);
					}
				}
			}
			//finish finding the first intersection NODE

			p3->x = blockage_list->pool[i].ur.x;
			p3->y = (p3->x - p1->x) * direction  + p1->y;
			
			if((p3->y >= blockage_list->pool[i].ll.y) &&
				(p3->y <= blockage_list->pool[i].ur.y)){
				adjacent_list[2*intersection_blockage+1] = (*p3);
			}
			else{
				p3->y = blockage_list->pool[i].ur.y;
				p3->x = (p3->y - p1->y) * direction + p1->x; 
				adjacent_list[2*intersection_blockage+1] = (*p3);
			}

		}

		else if (direction == -1){
			if(p1->x >= blockage_list->pool[i].ll.x){
				if(p1->y <= blockage_list->pool[i].ur.y){
					(*p3) = (*p1);
					adjacent_list[2*intersection_blockage] = (*p3);
				}
				else{
					p3->y = blockage_list->pool[i].ur.y;
					p3->x = (p3->y - p1->y) * direction + p1->x;
	
					if(p3->x >= blockage_list->pool[i].ur.x)
						continue;
					adjacent_list[2*intersection_blockage] = (*p3);
				}	
			}
			else if(p1->y <= blockage_list->pool[i].ur.y){
				if(p1->x == blockage_list->pool[i].ll.x){
					(*p3) = (*p1);
					adjacent_list[2*intersection_blockage] = (*p3);
				}
				else{
					p3->x = blockage_list->pool[i].ll.x;
					p3->y = (p3->x - p1->x) * direction + p1->y;
	
					if(p3->y <= blockage_list->pool[i].ll.y)
						continue;
					adjacent_list[2*intersection_blockage] = (*p3);
				}
			}
			else{
				p3->x = blockage_list->pool[i].ll.x;
				p3->y = (p3->x - p1->x) * direction + p1->y;

				if(p3->y <= blockage_list->pool[i].ll.y)
					continue;
				else if (p3->y <= blockage_list->pool[i].ur.y)
					adjacent_list[2*intersection_blockage] = (*p3);
				else{
					p3->y = blockage_list->pool[i].ur.y;
					p3->x = (p3->y - p1->y) * direction  + p1->x;

					if(p3->x >= blockage_list->pool[i].ur.x)
						continue;
					else{
						adjacent_list[2*intersection_blockage] = (*p3);
					}
				}
			}
			//finish finding the first intersection NODE

			p3->x = blockage_list->pool[i].ur.x;
			p3->y = (p3->x - p1->x) * direction  + p1->y;
			
			if((p3->y >= blockage_list->pool[i].ll.y) &&
				(p3->y <= blockage_list->pool[i].ur.y)){
				adjacent_list[2*intersection_blockage+1] = (*p3);
			}
			else{
				p3->y = blockage_list->pool[i].ll.y;
				p3->x = (p3->y - p1->y) * direction + p1->x; 
				adjacent_list[2*intersection_blockage+1] = (*p3);
			}

		}
		intersection_blockage++;
	}

	//printf("huhu %f, %f, %f, %f\n",p1->x,p1->y,p2->x,p2->y);

	if(intersection_blockage>0){

		//finish finding the intersection NODE list
		//sorting
		for(i=0; i<intersection_blockage-1; i++){
			k=i;
			for(j=i+1; j<intersection_blockage; j++){
				if(adjacent_list[k*2].x > adjacent_list[j*2].x)
					k=j;
			}
			if(k!=i){
				(*p3) = adjacent_list[i*2];
				adjacent_list[i*2] = adjacent_list[k*2];
				adjacent_list[k*2] = (*p3);
				(*p3) = adjacent_list[i*2+1];
				adjacent_list[i*2+1] = adjacent_list[k*2+1];
				adjacent_list[k*2+1] = (*p3);
			}
		}

		//compute the longest distance
		if(first_not_in==0){
			(*p2) = adjacent_list[0];
		}
		else if (first_not_in == 1){
			(*p1) = adjacent_list[2*intersection_blockage-1];
		}
/*
		i = 0;
		j = 0;
		while(i <= intersection_blockage){
			if(i == 0){
				distance = compute_distance(p1, &(adjacent_list[2*i]));
			}
			else if (i == intersection_blockage){
				temp = compute_distance(&(adjacent_list[2*i-1]), p2);
				if(temp > distance) j = i;
			}
			else{
				temp = compute_distance(&(adjacent_list[2*i-1]),&(adjacent_list[2*i]));
				if(temp > distance) j = i;
			}
			i++;
		}

		if(j==0){
			(*p2) = adjacent_list[0];
		}
		else if (j == intersection_blockage){
			(*p1) = adjacent_list[2*intersection_blockage-1];
		}
		else{
			(*p1) = adjacent_list[2*j-1];
			(*p2) = adjacent_list[2*j];
		}*/
	}

	free(p3);
	free(adjacent_list);
	//printf("after %d %d %d %d\n", p1->x, p1->y, p2->x, p2->y);


}

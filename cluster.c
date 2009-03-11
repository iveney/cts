// ----------------------------------------------------------------//
// Filename : cluster.c
// try to cluster some near sink nodes
//
// Author : Xiao Zigang
// Modifed: < Thu Mar  5 14:42:55 HKT 2009 >
// ----------------------------------------------------------------//
// Useage:
// Use (preprocess_block) and (preprocess_sinks) first
// and (construct_g_all)
// then use (cluster_sink)s to generate polylines
// Use (link_info[i]) to access the link, (link_num) is the total number
// where (link_list) is the parent pointer (from tail to head)
// At last should call (free_clusters) to free resources

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "util.h"
#include "ds.h"
#include "cluster.h"
#include "connect.h"
#include "bufplace.h"

int *  link_list=NULL;
BOOL * isHead=NULL;
BOOL * isTail=NULL;
LINK_INFO* link_info=NULL;
int link_num=0;

// REQUIRE: all pair shortest path has been computed
//          if not, calculate it first
// We need a data structure to store the list
void cluster_sinks(BLOCKAGE * blockage,SINK * sink){
	if( dirty )
		report_exit("You must calculate all pair shortest path first!");
	allocate_init_clusters(sink);
	int i,j;
	int head,tail;
	int length = 0;
	int cap = 0;
	int num = 0;
	BOOL * used = (BOOL*) malloc(sink->num * sizeof(BOOL));
	memset(used,FALSE,sizeof(sink->num * sizeof(int)));
	for(i=0;i<sink->num;i++){
		// for each sink, try to form a poly line GREEDILY
		// note that in g, sink node starts from static_num
		if(used[i]) continue;
		used[i]=TRUE;
		head = tail = i;
		length = 0;
		cap = sink->pool[i].lc;
		num=1;
		do{
			int min_dist = INFINITE;
			int min_idx = -1;
			char which = 'h';// h=head, t=tail
			int s,t;

			// find a nearest neighbour in head side
			for(j=0;j<sink_num;j++){
				s=head+static_num;
				t=j+static_num;
				if( !used[j] && head!=j &&
				     pairs[s][t] < min_dist ){
					min_dist = pairs[s][t];
					min_idx = j;
					which = 'h';
				}
			}

			// find a nearest neighbour in tail side
			for(j=0;j<sink_num;j++){
				s=tail+static_num;
				t=j+static_num;
				if( !used[j] && tail!=j &&
				     pairs[s][t] < min_dist ) {
					min_dist = pairs[s][t];
					min_idx = j;
					which = 't';
				}
			}

			// check if adding the selected node violates
			int new_len,new_cap;
			if( min_idx != -1 ){
				new_len	= length+min_dist;
				new_cap = (cap + sink->pool[min_idx].lc);
			}
			if( min_idx != -1 &&
			    new_len <= MAX_LEN &&
			    new_cap <= MAX_CAP &&
			    num +1  <= MAX_SINK ){
				// update length cap num
				length = new_len;
				cap = new_cap;
				num++;
				used[min_idx] = TRUE;
			}
			else{ // finish this polyline
				link_info[link_num].l = length;
				link_info[link_num].c = cap;
				link_info[link_num].n = num;
				link_info[link_num].t = tail;
				link_info[link_num].h = head;
				link_num++;
				break;
			}

			// now EXPAND(update head/tail and pointer)
			switch( which ){
			case 'h':
				link_list[head] = min_idx;
				isHead[head] = FALSE;
				isTail[min_idx] = FALSE;
				isHead[min_idx] = TRUE;
				head = min_idx;
				break;
			case 't':
				link_list[min_idx] = tail;
				isTail[tail] = FALSE;
				isTail[min_idx] = TRUE;
				isHead[min_idx] = FALSE;
				tail = min_idx;
				break;
			}
		}while(1);
	}
	free(used);
	return;
}

void allocate_init_clusters(SINK * sink){
	int i;
	link_list = (int*) malloc(sink->num * sizeof(int));
	isHead = (int*) malloc(sink->num * sizeof(int));
	isTail = (int*) malloc(sink->num * sizeof(int));
	link_info = (LINK_INFO*) malloc(sink->num * sizeof(LINK_INFO));
	for(i=0;i<(sink->num);i++){
		link_list[i] = -1;
		isHead[i]    = TRUE;
		isTail[i]    = TRUE;
	}
}

void free_clusters(){
	free(link_list);
	free(isHead);
	free(isTail);
	free(link_info);
	link_list=NULL;
	isHead=isTail=NULL;
	link_info=NULL;
}

void store_sinks(SINK * sink,SINK * linked_sink){
	int i;
	/*
	for(i=0;i<sink->num;i++){
		printf("%d %d %d\n",i, sink->pool[i].x, sink->pool[i].y);
	}
	*/
	SNODE * ns = (SNODE*) malloc(sizeof(SNODE)*sink->num);
	int count=0;
	for(i=0;i<link_num;i++){
		int p = link_info[i].t;
		ns[count] = sink->pool[p];
		ns[count].lc = link_info[i].c;
		//printf("%d %d %d\n",count, ns[count].x, ns[count].y);
		count++;
	}

	linked_sink->pool = (SNODE*) malloc(sizeof(SNODE)*sink->num);
	memcpy(linked_sink->pool,sink->pool,sizeof(SNODE)*sink->num);
	linked_sink->num = sink->num;

	memcpy(sink->pool,ns,sizeof(SNODE)*sink->num);
	sink->num = count;
	/*
	for(i=0;i<sink->num;i++){
		printf("%d %d %d\n",i, sink->pool[i].x, sink->pool[i].y);
	}
	*/
}


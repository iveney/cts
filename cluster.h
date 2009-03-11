#ifndef __CLUSTER_H__
#define __CLUSTER_H__

#define MAX_LEN  0
#define MAX_SINK 8
#define MAX_CAP  300

/* cluster.c */
typedef struct{
	int l;    // length
	int n;    // node number
	int c;    // total cap
	int t;    // tail index;
	int h;    // head index;
}LINK_INFO;

int *  link_list;
BOOL * isHead;
BOOL * isTail;
LINK_INFO* link_info;
int link_num;

void cluster_sinks(BLOCKAGE *blockage, SINK *sink);
void allocate_init_clusters(SINK *sink);
void free_clusters(void);
void store_sinks(SINK * sink,SINK *);
#endif

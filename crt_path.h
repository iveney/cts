#ifndef CRT_PATH_H
#define CRT_PATH_H
#include"ds.h"
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
extern int num_node;
extern int num_buffer;
extern int num_wire;
extern int num_total_nodes;
extern int total_buf_num;
extern int new_num_node;
extern int new_num_wire;
extern int new_total_buf_num;
extern int new_num_total_nodes;
extern DME_TREE_NODE * ZERO;

void refine_crt_num(int *, int  *, int *, BUF_NODE **);
void cpy_buf_list(BUF_NODE *,BUF_NODE *);
void refine_crt_path(BUF_NODE **, DME_TREE_NODE *, DME_TREE_NODE **, int , int ,int );
void quick_find(int,int,int,BUF_NODE *,int *, int *,int *);
void decode(int , int * , int *);
int partial_path_CLR(int,int,int *,BUF_NODE *,int,BUF_NODE **,DME_TREE_NODE *, DME_TREE_NODE **, double *);
void add_buf_list(BUF_NODE *, BUF_NODE *);
void adjust_node_id(DME_TREE_NODE *, int );
void trans_crt_path(BUF_NODE **, DME_TREE_NODE *, DME_TREE_NODE **);
#endif



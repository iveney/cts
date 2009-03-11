#ifndef __BUFPLACE_H__
#define __BUFPLACE_H__

#include "ds.h"
#include <stdio.h>
#include<math.h>


void draw_point( FILE *fp , double x1, double y1, int dash, int colour) ;
void draw_wire(FILE *fp, double x1, double y1, double x2, double y2, int dash, int colour) ;
void draw_wire_node(FILE *fp,NODE s,NODE t,int dash, int color);
void draw_line(FILE *fp, double x1, double y1, double x2, double y2, int dash, int colour);
void draw_line_node(FILE *fp,NODE s,NODE t, int dash, int colour);
void draw_block(FILE *fp,BOX b,int dash,int color);
void draw_rect(FILE *fp, double llx, double lly, double urx, double ury, int dash, int color);
void draw_block(FILE *fp, BOX b, int dash, int color);
void draw_blockages(FILE *fp);
void draw_sinks(FILE *fp);
void draw_wire_node(FILE *fp, NODE s, NODE t, int dash, int color);
void draw_line(FILE *fp, double x1, double y1, double x2, double y2, int dash, int colour);
void draw_line_node(FILE *fp, NODE s, NODE t, int dash, int color);
void draw_single_source_tree(FILE *pFig, int *back, int src_idx);
void draw_single_source_rectilinear(FILE *pFig, int *back, int src_idx);

#endif

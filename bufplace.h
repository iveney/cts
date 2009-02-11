#ifndef __BUFPLACE_H__
#define __BUFPLACE_H__

#include "ds.h"
#include <stdio.h>
#include <math.h>

void draw_point(FILE *fp , double x1, double y1, int dash, int colour) ;
void draw_wire(FILE *fp, double x1, double y1, double x2, double y2, int dash, int colour) ;
void draw_block(FILE *fp,BOX b,int dash,int color);
void draw_rect(FILE *fp,double llx,double lly,double urx,double ury,int dash,int color);

#endif

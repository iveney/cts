#ifndef __IO_H__
#define __IO_H__
/* io.c */
int InputFile(FILE *ifp);
int check_input(void);
void output_file(FILE *fp, BUF_NODE **OBUF, DME_TREE_NODE *OT, DME_TREE_NODE **OTmap);
#endif

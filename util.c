// ----------------------------------------------------------------//
// Filename : util.c
// Source code for some utility functions
//
// Author : Xiao Zigang
// Modifed: <Fri Feb 13 16:46:54 HKT 2009> 
// ----------------------------------------------------------------//

#include <stdlib.h>
#include <stdio.h>
#include "util.h"

// report an error string and exit the program
// Note that it is wrapped by a macro `report_exit'
// Do NOT use this directly.
void _report_exit(const char *location, const char *msg){
	  fprintf(stderr,"Error at %s: %s\n", location, msg);
	  exit(1);
}

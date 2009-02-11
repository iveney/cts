#include <stdio.h>
#include "util.h"

void _report_exit(const char *location, const char *msg){
	  fprintf(stderr,"Error at %s: %s\n", location, msg);
	  exit(1);
}

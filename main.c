#include "ds.h"

#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<string.h>

BOX frame	; 
SOURCE source  ;
SINK sink	; 
WIRELIB wirelib;
BUFLIB	buflib ; 
VDDLIB	vddlib ; 
int SlewLimit; 
int CapLimit ; 
BLOCKAGE blockage;

int main(int argc, char **argv){
	FILE *ifp; 
	
	if(argc > 2){
		printf("error: command inputfile\n");
		exit(1);
	}
	ifp = fopen( argv[1], "r") ; 
	printf("%d\n",InputFile(ifp));
	//check_input();
	deferred_merge_embedding();

	return 1;
}

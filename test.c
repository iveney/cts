#include <stdio.h>
#include "connect.h"

int main(){
	int n=10;
	BLOCKAGE block;
	block.num = 10;
	constructg(&block);
	outputg();
	return 0;
}

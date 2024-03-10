#include <stdio.h>
#include "lib.h"

extern int a;

int main()
{
	foo(1);
	printf("a=%d\n", a);
	return 0;	
}

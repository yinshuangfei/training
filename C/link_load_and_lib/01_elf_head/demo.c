#include<stdio.h>
extern char __executable_start[];
int a = 0;

void main()
{
	printf("executable_start %x\n", __executable_start);
	printf("hello %d\n", a);

}

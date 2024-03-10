char* mystr = "hello world!!!\n";

void print()
{
	asm("movl $15,%%edx \n\t"
	    "movl %0,%%ecx \n\t"
	    "movl $0,%%ebx \n\t"
	    "movl $4,%%eax \n\t"
	    "int $0x80 \n\t"
	    ::"r"(mystr):"edx","ecx","ebx");
}

void exit()
{
	asm("movl $42,%ebx \n\t"
	    "movl $1,%eax \n\t"
	    "int $0x80 \n\t");
}

void nomain()
{
	print();
	exit();
}

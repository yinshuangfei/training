int share = 1;

void myswap(int *a, int *b)
{
	*a ^= *b ^= *a ^= *b;
	//int c = *a;
	//*a = *b;
	//*b = c;
}

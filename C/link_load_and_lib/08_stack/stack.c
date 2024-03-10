#include <stdio.h>
#include <elf.h>

int main(int argc, char* argv[])
{
	long int* p = (int*)argv;
	int i;
	Elf64_auxv_t* aux;

	printf("argv at address: %p\n", p);

	printf("\narg count: %d\n", *(p - 1));
	for(i = 0; i < *(p - 1); ++i) {
		printf("args %d: %s\n", i, *(p + i));
	}

	p += i;
	p++;

	printf("\nEnvironment:\n");
	while(*p) {
		printf("%s\n", *p);
		p++;
	}

	p++;

	printf("\nAuxiliary Vectors:\n");
	aux = (Elf64_auxv_t*)p;
	while(aux->a_type != AT_NULL) {
		printf("Type: %02d, \tValue: 0x%x\n", aux->a_type, aux->a_un.a_val);
		aux++;
	}

	return 0;
}

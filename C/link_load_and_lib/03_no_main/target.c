/**
# apt install binutils-dev
*/
#include <stdio.h>
#include "bfd.h"

void main()
{
	const char** t = bfd_target_list();
	while (*t) {
		printf("%s\n", *t);
		t++;
	}

}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

int 
main (int argc, char * argv[]) {
	DIR *dp;
	struct dirent *dirp;

	printf("%s\n", strerror(1));

	if (argc != 2) {
		exit(-1);
	}

	if ((dp = opendir(argv[1])) == NULL)
		exit(-1);
	
	while ((dirp = readdir(dp)) != NULL) {
		printf("%s\n", dirp->d_name);
	}

	closedir(dp);

	exit(0);

}

#include <stdio.h>
#include <stdlib.h>

unsigned long int max_num = 0;

int main()
{
	unsigned blocksize[] = {1024 * 1024, 1024, 1};
	int i, count;
	
	for(i = 0; i < 3; i++) {
                for (count = 1;; count++) {
                        // printf("current max_num is %lu\n", max_num);

                        void *block = malloc(max_num + blocksize[i] * count);
                        if (block) {
                                max_num = max_num + blocksize[i] * count;
                                free(block);
                        } else {
                                break;
                        }
                }
		
	}
	
	if (max_num / 1024 <= 0)
		printf("max malloc size = %lu bytes\n", max_num);
	else if (max_num / 1024 / 1024 <= 0)
		printf("max malloc size = %lu KBs\n", max_num/1024);
	else if (max_num / 1024 / 1024 / 1024<= 0)
		printf("max malloc size = %lu MBs\n", max_num/1024/1024);
	else if (max_num / 1024 / 1024 / 1024 / 1024 <= 0)
		printf("max malloc size = %lu GBs\n", max_num/1024/1024/1024);
	else if (max_num / 1024 / 1024 / 1024 / 1024 / 1024 <= 0)
		printf("max malloc size = %lu TBs\n", max_num/1024/1024/1024/1024);
}

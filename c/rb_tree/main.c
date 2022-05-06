#include <stdio.h>

#include "rb_tree.h"

#define MATRIX_LEN 16

int matrix[MATRIX_LEN] = {452, 53, 8234, 1, 
                          55, 234, 232, 567, 
                          34, 132, 234, 3, 
                          456, 23, 13, 56};

int main(int argc, int * argv[])
{
    rb_root * root = create_rbtree();

    for (int i = 0; i < MATRIX_LEN; i++) {
        printf("i=%d, num=%d\n", i, matrix[i]);
    }

    printf("hello world\n");
    return 0;
}
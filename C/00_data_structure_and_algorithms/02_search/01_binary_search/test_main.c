#include <stdio.h>
#include <stdlib.h>

int b_search(int list[], int left, int right, int target){
    if ( left > right )
        return -1;

    int mid = (left + right) / 2;

    if (target == list[mid] )
        return mid;
    else if ( target < list[mid] )
        return b_search(list, left, mid - 1, target);
    else
        return b_search(list, mid + 1, right, target);
}

int search(int list[], int num, int target){
    printf("target=%d, num=%d\n", target, num);
    if (num == 0)
        return -1;
    else
        return b_search(list, 0, num - 1, target);
}

void print_x(int list[], int num){
    int i;
    for(i=0; i<num; i++)
        printf("index=%d, %d\n", i, list[i]);
}

int main(int argc, char * argv[]){
    printf("start ... \n");
    int data[]= {1, 3, 4, 6, 7, 9, 10, 44, 45}; 
    int num = sizeof(data) / sizeof(int);


    int target;
    if (argc > 1)
        target = atoi(argv[1]);
    else
        target = 0;

    printf("ret=%d\n", search(data, num, target));
    print_x(data, num);

    return 0;

}
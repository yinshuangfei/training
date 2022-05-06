#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdlib.h>


const int Length = 30;


void update_bar(int percent_done) {
   int num_chars = percent_done * Length / 100;

   printf("\r[");

   for (int i = 0; i < num_chars; i++) {
        printf("#");
    } 

   for (int i = 0; i< Length - num_chars; i++){
        printf(" ");
    }

    printf("] %d%% Done ", percent_done);
    fflush(stdout);
}

int main(){

    for ( int i=0; i<= 100; i++ ) {
        update_bar(i);
        usleep(20000);
    }

    printf("\n");
    return 0;

}

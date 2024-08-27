#include "command.c"
#include <stdio.h>

int main(void){

    pipeline pipe = pipeline_new();
    printf("created \n");
    pipe = pipeline_destroy(pipe);
    printf("deleted \n");
    return 0;
}
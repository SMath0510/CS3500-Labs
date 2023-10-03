#include<stdio.h>
#include<stdlib.h>

int main(){
    int *data = (int *) malloc(100*sizeof(int));
    // Accessing an illegal element
    data[100] = 0;

    printf("Modified this value %d\n", data[108]);
    return 0;
}
#include<stdio.h>
#include<stdlib.h>

int main(){
    int *data = (int *) malloc(100*sizeof(int));
    // Setting data[0] as 5
    data[0] = 5;
    free(data);

    printf("Accessing the element %d\n", data[0]);
}
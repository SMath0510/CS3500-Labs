#include<stdio.h>

int main(){
    int data[100];
    // Accessing an illegal element
    data[108] = 0;

    printf("Modified this value %d\n", data[108]);
    return 0;
}
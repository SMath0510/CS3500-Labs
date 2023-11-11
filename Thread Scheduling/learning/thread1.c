#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>


int max;
int counter = 0; // shared global variable

pthread_mutex_t lock;



void *mythread(void *arg) {
    char *letter = arg;
    int i; // stack (private per thread) 
    printf("%s: begin [addr of i: %p]\n", letter, &i);
    printf("%d: counter address: %p\n", counter, &counter);
    pthread_mutex_lock(&lock);
    for (i = 0; i < max; i++) {
    counter = counter + 1; // shared: only one
    }
    pthread_mutex_unlock(&lock);
    printf("%s: done\n", letter);
    return NULL;
}
                                                                             
int main(int argc, char *argv[]) {                    
    if (argc != 2) {
    fprintf(stderr, "usage: main-first <loopcount>\n");
    exit(1);
    }
    max = atoi(argv[1]);

    pthread_mutex_init(&lock, NULL); // initialize the lock

    pthread_t p1, p2;
    printf("main: begin [counter = %d] [%p]\n", counter, (int *) &counter);
    pthread_create(&p1, NULL, mythread, "A"); 
    pthread_create(&p2, NULL, mythread, "B");
    // join waits for the threads to finish
    pthread_join(p1, NULL); 
    pthread_join(p2, NULL); 
    printf("main: done\n [counter: %d]\n [should: %d]\n", 
       counter, max*2);
    return 0;
}
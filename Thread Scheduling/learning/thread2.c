#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>


int max;
int counter = 0; // shared global variable

typedef struct __lock_t{
    int flag;
} lock_t;

int test_set(int * old_val, int new){
    int old = *old_val;
    *old_val = new;
    return old;
}

int compare_swap(int * old_val, int expected, int new){
    int old = *old_val;
    if(old == expected){
        *old_val = new;
    }
    return old;
}

int load_linked(int * val_ptr){
    return *val_ptr;
}

int store_conditional(int * val_ptr, int update_to){
    int cond = 1; // cond is that no one has updated val_ptr since the last load_linked on val_ptr
    if(cond == 1){
        *val_ptr = update_to;
        return 1; // success
    }
    return 0; // failed
}

lock_t lock;

void thread_init(lock_t * lock){
    lock->flag = 0;
}

void thread_lock(lock_t * lock){
    while(test_set((&lock->flag), 1) == 1);
    while(compare_swap((&lock->flag), 0, 1) == 1);
    while(1){
        while(load_linked(&lock->flag) == 1);
        // last accessed value is flag = 0
        if(store_conditional((&lock->flag), 1) == 1) return; // update the flag to 1 and use it // success
        // else retry all over
    }
}

void thread_unlock(lock_t * lock){
    lock->flag = 0;
}


void *mythread(void *arg) {
    char *letter = arg;
    int i; // stack (private per thread) 
    printf("%s: begin [addr of i: %p]\n", letter, &i);
    printf("%d: counter address: %p\n", counter, &counter);
    thread_lock(&lock);
    for (i = 0; i < max; i++) {
    counter = counter + 1; // shared: only one
    }
    thread_unlock(&lock);
    printf("%s: done\n", letter);
    return NULL;
}
                                                                             
int main(int argc, char *argv[]) {                    
    if (argc != 2) {
    fprintf(stderr, "usage: main-first <loopcount>\n");
    exit(1);
    }
    max = atoi(argv[1]);

    thread_init(&lock); // initialize the lock

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
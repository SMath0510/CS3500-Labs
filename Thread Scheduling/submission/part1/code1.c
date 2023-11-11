#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<stdbool.h>
#include<time.h>
#include<math.h>
#include<limits.h>
#include<pthread.h>

// Global variables 

int num_producers = 0;
int num_consumers = 0;
int buffer_size = 0;
int production_limit = 0;
int consumption_limit = 0;

int in_idx = 0;
int out_idx = 0;
int counter = 0;

int items_to_consume = 0;
int buffer_overflow = 0;
int no_more_items = 0;

pthread_mutex_t lock;

int * buffer;

// Defines

#define CONSUMER_CONSTANT 0.7
#define RAND_1000_9999 (rand() % 9000 + 1000)
#define RAND_0_3 rand() % 4
#define RAND_0_4 rand() % 5

void set_consumption_limit(){
    consumption_limit = CONSUMER_CONSTANT * production_limit * num_producers / num_consumers;
}

void *runner_p(void *param){
    int thread_idx = * (int *) param;
    int value_held = 0;
    for(int i = 1; i <= production_limit; i++){
        value_held = RAND_1000_9999; // value that is to be stored 

        while(counter == buffer_size){
            // WAIT TILL THE BUFFER IS FULL
            int flag = (items_to_consume == 0) || (buffer_overflow == 1); 
            if(flag == 1){
                // if no more items to consume (OR) buffer is full without an overflow EXIT the thread
                buffer_overflow = 1;
                pthread_exit(0);
            }
        }

        // CRITICAL SECTION

        pthread_mutex_lock(&lock);
        if(counter == buffer_size){   // just for sanity check
            i --;  // re-run this iteration
            pthread_mutex_unlock(&lock);
            continue;
        }

        buffer[in_idx] = value_held; // stored  in the buffer
        in_idx ++;

        counter ++; // produced one more item

        fprintf(stdout, "Producer Thread #: %d; Item #: %d; in value: %d; Added Number: %d\n", thread_idx, i, in_idx, value_held);
        pthread_mutex_unlock(&lock);
        sleep(RAND_0_3);
    }
    pthread_exit(0);
}

void *runner_c(void *param){
    int thread_idx = * (int *) param;
    int value_held = 0;
    for(int i = 1; i <= consumption_limit; i++){

        while(counter == 0 && (no_more_items == 1)){
            // Just wait (let producers finish) !! my solution to the case where the consumer might not be able to exit
        }

        // CRITICAL SECTION

        if ((no_more_items == 1) && items_to_consume == 0){
            pthread_exit(0);
        }

        pthread_mutex_lock(&lock);
        if(counter == 0){   // just for sanity check
            i --;  // re-run this iteration
            pthread_mutex_unlock(&lock);
            continue;
        }

        value_held = buffer[out_idx]; // took out from the buffer
        out_idx ++;

        counter --;   // consumed one counter
        items_to_consume --; // consumed one item

        fprintf(stdout, "Consumer Thread #: %d; Item #: %d; out value: %d; Read Number: %d\n", thread_idx, i, out_idx, value_held);
        pthread_mutex_unlock(&lock);
        sleep(RAND_0_4);
    }
    pthread_exit(0);
}

int main(int argc, char * argv[]){
    if(argc != 9)
    {
        // ERROR
        fprintf(stderr, "Invalid Number of Inputs\n");
        exit(-1);
    }

    // READING
    num_producers = atoi(argv[2]);
    num_consumers = atoi(argv[4]);
    buffer_size = atoi(argv[6]);
    production_limit = atoi(argv[8]);

    // INITIALIZE
    set_consumption_limit();
    items_to_consume = consumption_limit * num_consumers;

    buffer = (int *) malloc(buffer_size * sizeof(int));

    pthread_t producer_thread[num_producers];
    pthread_t consumer_thread[num_consumers];

    pthread_attr_t attribute;

    // INITIALIZE PTHREAD
    pthread_mutex_init(&lock, NULL);
    pthread_attr_init(&attribute);

    int producer_id[num_producers];
    int consumer_id[num_consumers];

    for(int i = 0; i < num_producers; i++){
        producer_id[i] = i + 1;
        int STATUS = pthread_create(&producer_thread[i], &attribute, runner_p, &producer_id[i]);
        if (STATUS < 0){
            // ERROR
            perror("\nUnable to create thread\n");
            exit(-1);
        }
    }

    for(int i = 0; i < num_consumers; i++){
        consumer_id[i] = i + 1;
        int STATUS = pthread_create(&consumer_thread[i], &attribute, runner_c, &consumer_id[i]);
        if (STATUS < 0){
            // ERROR
            perror("\nUnable to create thread\n");
            exit(-1);
        }
    }

    // JOIN PRODUCERS
    for(int i = 0; i < num_producers; i++){
        pthread_join(producer_thread[i], NULL);
    }

    no_more_items = 1;  // producers are done producing

    // JOIN CONSUMERS
    for(int i = 0; i < num_consumers; i++){
        pthread_join(consumer_thread[i], NULL);
    }

    if(buffer_overflow == 1){
        // BUFFER OVERFLOWED
        printf("Buffer Overflow\n");
    }
    
    // PROGRAM DONE
    printf("Program over...\n");
    return 0;
}
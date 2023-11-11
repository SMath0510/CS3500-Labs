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
pthread_cond_t empty, fill;

int * buffer;

#define CONSUMER_CONSTANT 0.7
#define RAND_1000_9999 (rand() % 9000 + 1000)
#define RAND_0_3 rand() % 4
#define RAND_0_4 rand() % 5

void set_consumption_limit(){
    consumption_limit = CONSUMER_CONSTANT * production_limit * num_producers / num_consumers;
}

void put(int value) {
    buffer[in_idx] = value;
    in_idx = (in_idx + 1) % buffer_size;
    counter++;
}

int get() {
    int tmp = buffer[out_idx];
    out_idx = (out_idx + 1) % buffer_size;
    counter--;
    return tmp;
}

void *producer(void *param){
    int thread_idx = * (int *) param;
    int value_held = 0;
    for(int i = 1; i <= production_limit; i++){
        value_held = RAND_1000_9999; // value that is to be stored 

        while(counter == buffer_size){
            int flag = (items_to_consume == 0) || (buffer_overflow == 1); 
            if(flag == 1){
                buffer_overflow = 1;
                pthread_exit(0);
            }
        }

        pthread_mutex_lock(&lock);
        if(counter == buffer_size){
            i --;
            pthread_mutex_unlock(&lock);
            continue;
        }

        put(value_held);
        fprintf(stdout, "Producer Thread #: %d; Item #: %d; in value: %d; Added Number: %d\n", thread_idx, i, in_idx, value_held);
        pthread_cond_signal(&fill);
        pthread_mutex_unlock(&lock);
        sleep(RAND_0_3);
    }
    pthread_exit(0);
}

void *consumer(void *param){
    int thread_idx = * (int *) param;
    int value_held = 0;
    for(int i = 1; i <= consumption_limit; i++){
        while(counter == 0 && (no_more_items == 1)){
            // Just wait (let producers finish)
        }

        if ((no_more_items == 1) && items_to_consume == 0){
            pthread_exit(0);
        }

        pthread_mutex_lock(&lock);
        if(counter == 0){
            i --;
            pthread_mutex_unlock(&lock);
            continue;
        }

        value_held = get();
        fprintf(stdout, "Consumer Thread #: %d; Item #: %d; out value: %d; Read Number: %d\n", thread_idx, i, out_idx, value_held);
        pthread_cond_signal(&empty);
        pthread_mutex_unlock(&lock);
        sleep(RAND_0_4);
    }
    pthread_exit(0);
}

int main(int argc, char * argv[]){
    if(argc != 9)
    {
        fprintf(stderr, "Invalid Number of Inputs\n");
        exit(-1);
    }

    num_producers = atoi(argv[2]);
    num_consumers = atoi(argv[4]);
    buffer_size = atoi(argv[6]);
    production_limit = atoi(argv[8]);

    set_consumption_limit();
    items_to_consume = consumption_limit * num_consumers;

    buffer = (int *) malloc(buffer_size * sizeof(int));

    pthread_t producer_thread[num_producers];
    pthread_t consumer_thread[num_consumers];

    pthread_attr_t attribute;

    pthread_mutex_init(&lock, NULL);
    pthread_cond_init(&empty, NULL);
    pthread_cond_init(&fill, NULL);
    pthread_attr_init(&attribute);

    int producer_id[num_producers];
    int consumer_id[num_consumers];

    for(int i = 0; i < num_producers; i++){
        producer_id[i] = i + 1;
        int STATUS = pthread_create(&producer_thread[i], &attribute, producer, &producer_id[i]);
        if (STATUS < 0){
            perror("\nUnable to create thread\n");
            exit(-1);
        }
    }

    for(int i = 0; i < num_consumers; i++){
        consumer_id[i] = i + 1;
        int STATUS = pthread_create(&consumer_thread[i], &attribute, consumer, &consumer_id[i]);
        if (STATUS < 0){
            perror("\nUnable to create thread\n");
            exit(-1);
        }
    }

    for(int i = 0; i < num_producers; i++){
        pthread_join(producer_thread[i], NULL);
    }

    no_more_items = 1;

    for(int i = 0; i < num_consumers; i++){
        pthread_join(consumer_thread[i], NULL);
    }

    if(buffer_overflow == 1){
        printf("Buffer Overflow\n");
    }
    
    printf("Program over...\n");

    // Destroy mutex and condition variables
    pthread_mutex_destroy(&lock);
    pthread_cond_destroy(&empty);
    pthread_cond_destroy(&fill);

    return 0;
}

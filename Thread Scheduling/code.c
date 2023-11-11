#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define BUFFER_SIZE 100

int *buffer;
int in = 0, out = 0, count = 0;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t full = PTHREAD_COND_INITIALIZER;
pthread_cond_t empty = PTHREAD_COND_INITIALIZER;

int num_producers, num_consumers, buffer_size, max_items;

void produce(int item, int thread_id, int item_number) {
    printf("Producer Thread #: %d; Item #: %d; Added Number: %d\n", thread_id, item_number, item);
}

int consume(int thread_id, int item_number) {
    int item = buffer[out];
    printf("Consumer Thread #: %d; Item #: %d; Read Number: %d\n", thread_id, item_number, item);
    return item;
}

void *producer(void *arg) {
    int thread_id = *((int *)arg);

    for (int i = 0; i < max_items; ++i) {
        int item = rand() % 9000 + 1000; // random number between 1000 and 9999

        pthread_mutex_lock(&mutex);

        while (count == buffer_size) {
            pthread_cond_wait(&full, &mutex);
        }

        produce(item, thread_id, i);

        buffer[in] = item;
        in = (in + 1) % buffer_size;
        count++;

        pthread_cond_signal(&empty);

        pthread_mutex_unlock(&mutex);

        usleep(rand() % 3000000); // sleep for 0 to 3 seconds
    }

    return NULL;
}

void *consumer(void *arg) {
    int thread_id = *((int *)arg);
    int items_to_consume = (int)(0.7 * num_producers * max_items);

    for (int i = 0; i < items_to_consume; ++i) {
        pthread_mutex_lock(&mutex);

        while (count == 0) {
            pthread_cond_wait(&empty, &mutex);
        }

        int item = consume(thread_id, i);

        out = (out + 1) % buffer_size;
        count--;

        pthread_cond_signal(&full);

        pthread_mutex_unlock(&mutex);

        usleep(rand() % 4000000); // sleep for 0 to 4 seconds
    }

    return NULL;
}

int main(int argc, char *argv[]) {\
    printf("%d: Inputs\n", argc);
    if (argc != 7) {
        fprintf(stderr, "Usage: %s -p <int> -c <int> -s <int> -n <int>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Parse command-line arguments
    for (int i = 1; i < argc; ++i) {
        if (argv[i][0] == '-' && i + 1 < argc) {
            switch (argv[i][1]) {
            case 'p':
                num_producers = atoi(argv[++i]);
                break;
            case 'c':
                num_consumers = atoi(argv[++i]);
                break;
            case 's':
                buffer_size = atoi(argv[++i]);
                break;
            case 'n':
                max_items = atoi(argv[++i]);
                break;
            default:
                fprintf(stderr, "Invalid option: %s\n", argv[i]);
                exit(EXIT_FAILURE);
            }
        }
    }
    // return 0;

    // Allocate memory for the buffer
    buffer = (int *)malloc(buffer_size * sizeof(int));

    // Create producer and consumer threads
    pthread_t producer_threads[num_producers];
    pthread_t consumer_threads[num_consumers];

    int producer_ids[num_producers];
    int consumer_ids[num_consumers];

    printf("Here 1\n");
    for (int i = 0; i < num_producers; ++i) {
        producer_ids[i] = i + 1;
        pthread_create(&producer_threads[i], NULL, producer, (void *)&producer_ids[i]);
    }
    printf("Creation Producers done\n");

    for (int i = 0; i < num_consumers; ++i) {
        consumer_ids[i] = i + 1;
        pthread_create(&consumer_threads[i], NULL, consumer, (void *)&consumer_ids[i]);
    }
    printf("Creation Consumers done\n");

    // Wait for producer threads to finish
    for (int i = 0; i < num_producers; ++i) {
        printf("Joining the thread producer %d\n", i);
        pthread_join(producer_threads[i], NULL);
    }
    printf("Joining Producers done\n");

    // Wait for consumer threads to finish
    for (int i = 0; i < num_consumers; ++i) {
        pthread_join(consumer_threads[i], NULL);
    }
    printf("Joining Consumers done\n");


    // Cleanup
    free(buffer);

    // Print final statement
    printf("Program over...\n");

    return 0;
}

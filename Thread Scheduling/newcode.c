#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

int num_producers, num_consumers, buffer_size, production_limit, consumption_limit, in, out, counter, items_to_consume; /* this data is shared by the thread(buffer_size) */
int *buffer;
bool buffer_overflow;
bool no_more_items = false;

pthread_mutex_t lock;

/* The thread will begin control in this function */

void *runner_p(void *param)
{
  int thread_no = *(int *)param, val;
  for (int i = 1; i <= production_limit; i++)
  {
    val = rand() % 9000 + 1000;
    while (counter == buffer_size)
    {
      if ((items_to_consume == 0 || !buffer_overflow) && counter == buffer_size)
      {
        buffer_overflow = false;
        pthread_exit(0);
      }
    }

    pthread_mutex_lock(&lock);

    if (counter == buffer_size)
      i--;
    else
    {
      counter++;
      buffer[in] = val;
      fprintf(stdout, "Producer Thread #: %d; Item #: %d; in value: %d; Added Number: %d\n", thread_no, i, in, val);
      in = (in + 1) % buffer_size;
    }

    pthread_mutex_unlock(&lock);
    sleep(rand() % 4);
  }
  pthread_exit(0);
}
void *runner_c(void *param)
{
  int thread_no = *(int *)param, val;
  for (int i = 1; i <= consumption_limit; i++)
  {
    while (counter == 0 && !no_more_items)
      ; // Check termination condition

    if (no_more_items)
      break; // Exit if no more items will be produced

    pthread_mutex_lock(&lock);

    if (counter == 0)
      i--;
    else
    {
      counter--;
      val = buffer[out];
      fprintf(stdout, "Consumer Thread #: %d; Item #: %d; out value: %d; Read Number: %d\n", thread_no, i, out, val);
      out = (out + 1) % buffer_size;
      items_to_consume--;
    }

    pthread_mutex_unlock(&lock);

    sleep(rand() % 5);
  }

  pthread_exit(0);
}

int main(int argc, char *argv[])
{
  if (argc != 9)
  {
    fprintf(stderr, "Invalid Number of Inputs\n");
    exit(-1);
  }

  srand(time(0));
  int i;
  num_producers = atoi(argv[2]);
  num_consumers = atoi(argv[4]);
  buffer_size = atoi(argv[6]);
  production_limit = atoi(argv[8]);
  consumption_limit = 0.7 * production_limit * num_producers / num_consumers;
  in = out = counter = 0;
  items_to_consume = consumption_limit * num_consumers;
  buffer_overflow = true;

  buffer = (int *)malloc(buffer_size * sizeof(int));

  pthread_t producer_thread[num_producers], consumer_thread[num_consumers];
  pthread_attr_t attr;

  pthread_attr_init(&attr);
  pthread_mutex_init(&lock, NULL); /* create the mutex lock */

  int producer_id[num_producers];
  for (i = 0; i < num_producers; i++)
  {
    producer_id[i] = i + 1;
    if (pthread_create(&producer_thread[i], &attr, runner_p, &producer_id[i]) < 0)
    {
      perror("\nUnable to create thread\n");
      exit(-1);
    }
  }

  int consumer_id[num_consumers];
  for (i = 0; i < num_consumers; i++)
  {
    consumer_id[i] = i + 1;
    if (pthread_create(&consumer_thread[i], &attr, runner_c, &consumer_id[i]) < 0)
    {
      perror("\nUnable to create thread\n");
      exit(-1);
    }
  }

  for (i = 0; i < num_producers; i++)
    pthread_join(producer_thread[i], NULL);

  no_more_items = true; // Set the termination condition

  for (i = 0; i < num_consumers; i++)
    pthread_join(consumer_thread[i], NULL);

  if (!buffer_overflow)
    printf("Buffer Overflow\n");
  printf("Program over...\n");
  return 0;
}


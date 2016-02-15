#include "gtthread.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include "steque.h"

/* Thread Function Prototypes */
void *producer_routine(void *arg);
void *consumer_routine(void *arg);


/* Global Data */
long g_num_prod = 0; /* number of producer threads */
gtthread_mutex_t g_num_prod_lock;
gtthread_mutex_t g_queue_lock;

/* Main - entry point */
int main(int argc, char **argv) {
  gtthread_mutex_init(&g_num_prod_lock);
  gtthread_mutex_init(&g_queue_lock);
  gtthread_init(10);
  steque_t queue;
  int N = 5;
  gtthread_t producer_thread[N];
  gtthread_t consumer_thread[N];
  void *thread_return = NULL;
  int result = 0;

  /* Initialization */

  printf("Main thread started with thread id %d\n", (gtthread_self()).id);

  steque_init(&queue);

  g_num_prod = N; /* there will be N producer thread */
int i =0;
  /* Create producer and consumer threads */
  for(; i < N; i++){
  result = gtthread_create(&producer_thread[i], producer_routine, &queue);
  if (0 != result) {
    fprintf(stderr, "Failed to create producer thread: %s\n", strerror(result));
    exit(1);
  }

  result = gtthread_create(&consumer_thread[i], consumer_routine, &queue);
  if (0 != result) {
    fprintf(stderr, "Failed to create consumer thread: %s\n", strerror(result));
    exit(1);
  }
}
i =0;
  /* Join threads, handle return values where appropriate */
for(; i < N; i++){
  result = gtthread_join(consumer_thread[i], &thread_return);
  if (0 != result) {
    fprintf(stderr, "Failed to join consumer thread: %s\n", strerror(result));
    gtthread_exit(NULL);
  }

  result = gtthread_join(producer_thread[i], NULL);
  if (0 != result) {
    fprintf(stderr, "Failed to join producer thread: %s\n", strerror(result));
    gtthread_exit(NULL);
  }
}
  
  //printf("\nPrinted %"PRIdPTR" characters.\n", (intptr_t) thread_return);

  gtthread_mutex_destroy(&g_queue_lock);
  gtthread_mutex_destroy(&g_num_prod_lock);
  gtthread_exit(0);
  return 0;
}


/* Function Definitions */

/* producer_routine - thread that adds the letters 'a'-'z' to the queue */
void *producer_routine(void *arg) {
  steque_t *queue_p = arg;
  gtthread_t consumer_thread;
  int i, result = 0;
  intptr_t c;

  for (i = 0; i < 100; i++){
    for (c = 'a'; c <= 'z'; ++c) {
      /* Add the node to the queue */
      gtthread_mutex_lock(&g_queue_lock);

      steque_enqueue(queue_p, (void*) c);

      gtthread_mutex_unlock(&g_queue_lock);

      gtthread_yield();
    }
  }

  gtthread_mutex_lock(&g_num_prod_lock);
  --g_num_prod;
  gtthread_mutex_unlock(&g_num_prod_lock);
  return (void*) 0;
}


/* consumer_routine - thread that prints characters off the queue */
void *consumer_routine(void *arg) {
  steque_t *queue_p = arg;
  intptr_t c;
  long count = 0; /* number of nodes this thread printed */

  printf("Consumer thread started with thread id %d\n", (gtthread_self()).id);

  /* terminate the loop only when there are no more items in the queue
   * AND the producer threads are all done */

  gtthread_mutex_lock(&g_queue_lock);
  gtthread_mutex_lock(&g_num_prod_lock);

  while(!steque_isempty(queue_p) || g_num_prod > 0) {
    gtthread_mutex_unlock(&g_num_prod_lock);

    if (!steque_isempty(queue_p)) {
      c = (intptr_t) steque_pop(queue_p);
      //printf("%c", (char) c);
      ++count;
      gtthread_mutex_unlock(&g_queue_lock);
    }
    else { /* Queue is empty, so let some other thread run */
      gtthread_mutex_unlock(&g_queue_lock);
      gtthread_yield();
    }
    gtthread_mutex_lock(&g_queue_lock);
  gtthread_mutex_lock(&g_num_prod_lock);
  }
  gtthread_mutex_unlock(&g_num_prod_lock);
  gtthread_mutex_unlock(&g_queue_lock);
  printf("Consumer thread end with thread id %d\n", (gtthread_self()).id);
  return (void*) count;
}

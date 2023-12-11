// Name: Akshita Akumalla ID: 59965772
// Name: Gabriel Jordan Gibb ID: 55979751

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

#define MAX_BUFFER_SIZE 10
#define MAX_THREADS 16

pthread_mutex_t buffer_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t buffer_not_full = PTHREAD_COND_INITIALIZER;
pthread_cond_t buffer_not_empty = PTHREAD_COND_INITIALIZER;

int total_producers;
int total_consumers;
int items_per_producer;
int buffer_size, delay;

int buffer[MAX_BUFFER_SIZE];
int buffer_head = 0;
int buffer_tail = 0;

void* producer(void* arg) {
    int producer_id = *(int*)arg;
    for (int i = 0; i < items_per_producer; ++i) {
        //produce item
        int item = producer_id * items_per_producer + i;

        pthread_mutex_lock(&buffer_mutex);

        //wait if buffer is full
        while ((buffer_tail + 1) % buffer_size == buffer_head) {
            //mutex is unlocked since we need a consumer to consume the item since the buffer is full
            pthread_cond_wait(&buffer_not_full, &buffer_mutex);
        }

        buffer[buffer_tail] = item;
        //add the tail to move onto the next slot to produced
        buffer_tail = (buffer_tail + 1) % buffer_size;

        printf("producer_%d produced item %d\n", producer_id, item);

        pthread_cond_signal(&buffer_not_empty);
        pthread_mutex_unlock(&buffer_mutex);

        //delay based on user input
        if (delay == 1) {
            usleep(500000); //delay of 500,000 microseconds
        }
    }

    return NULL;
}

void* consumer(void* arg) {
    int consumer_id = *(int*)arg;
    for (int i = 0; i < items_per_producer * total_producers / total_consumers; ++i) {
        pthread_mutex_lock(&buffer_mutex);

        //wait if buffer is empty
        while (buffer_head == buffer_tail) {
            //mutex is unlocked since we need a producer thread to produce an item in order to consume
            pthread_cond_wait(&buffer_not_empty, &buffer_mutex);
        }

        //consume item
        int item = buffer[buffer_head];
        //add the head to move onto the next slot to be consumed
        buffer_head = (buffer_head + 1) % buffer_size;

        printf("consumer_%d consumed item %d\n", consumer_id, item);

        pthread_cond_signal(&buffer_not_full);
        pthread_mutex_unlock(&buffer_mutex);

        //delay based on user input
        if (delay == 0) {
            usleep(500000); //delay of 500,000 microseconds
        }
    }
    //generally we return the item but in this case we are not using it for anything
    return NULL;
}

int main(int argc, char* argv[]) {
    total_producers = atoi(argv[1]);
    total_consumers = atoi(argv[2]);
    items_per_producer = atoi(argv[3]);
    buffer_size = atoi(argv[4]);
    delay = atoi(argv[5]);

    if (total_consumers >= total_producers * items_per_producer) {
        fprintf(stderr, "error: number of consumers should be smaller than the total items being produced.\n");
        exit(1);
    }

    //although initialized to MAX_THREADS we will only use the arguments for each of the data structures
    pthread_t producers[MAX_THREADS];
    pthread_t consumers[MAX_THREADS];
    int producer_ids[MAX_THREADS];
    int consumer_ids[MAX_THREADS]; 

    //make producers
    for (int i = 0; i < total_producers; ++i) {
        producer_ids[i] = i;
        pthread_create(&producers[i], NULL, producer, &producer_ids[i]);
    }
    //make consumers
    for (int i = 0; i < total_consumers; ++i) {
        consumer_ids[i] = i;
        pthread_create(&consumers[i], NULL, consumer, &consumer_ids[i]);
    }
    //wait for all producer threads to finish
    for (int i = 0; i < total_producers; ++i) {
        pthread_join(producers[i], NULL);
    }
    //wait for all consumer threads to finish
    for (int i = 0; i < total_consumers; ++i) {
        pthread_join(consumers[i], NULL);
    }

    return 0;
}
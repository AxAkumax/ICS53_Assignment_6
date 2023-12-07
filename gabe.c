#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

#define MAX_BUFFER_SIZE 10
#define MAX_THREADS 16

pthread_mutex_t buffer_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t buffer_not_full = PTHREAD_COND_INITIALIZER;
pthread_cond_t buffer_not_empty = PTHREAD_COND_INITIALIZER;

int buffer[MAX_BUFFER_SIZE];
int buffer_index = 0;

int total_producers, total_consumers, items_per_producer, buffer_size, delay;

void* producer(void* arg) {
    int producer_id = *(int*)arg;

    for (int i = 0; i < items_per_producer; ++i) {
        // Produce item
        int item = producer_id * items_per_producer + i;

        pthread_mutex_lock(&buffer_mutex);

        // Wait if buffer is full
        while (buffer_index == buffer_size) {
            pthread_cond_wait(&buffer_not_full, &buffer_mutex);
        }

        buffer[buffer_index++] = item;

        // Print produced item
        printf("producer_%d produced item %d\n", producer_id, item);

        pthread_cond_signal(&buffer_not_empty);
        pthread_mutex_unlock(&buffer_mutex);

        // Introduce delay if needed
        if (delay == 1) {
            usleep(500000);
        }
    }

    return NULL;
}

void* consumer(void* arg) {
    int consumer_id = *(int*)arg;

    for (int i = 0; i < items_per_producer * total_producers / total_consumers; ++i) {
        pthread_mutex_lock(&buffer_mutex);

        // Wait if buffer is empty
        while (buffer_index == 0) {
            pthread_cond_wait(&buffer_not_empty, &buffer_mutex);
        }

        // Consume item
        int item = buffer[--buffer_index];

        // Print consumed item
        printf("consumer_%d consumed item %d\n", consumer_id, item);

        pthread_cond_signal(&buffer_not_full);
        pthread_mutex_unlock(&buffer_mutex);

        // Introduce delay if needed
        if (delay == 0) {
            usleep(500000);
        }
    }

    return NULL;
}

int main(int argc, char* argv[]) {
    if (argc != 6) {
        fprintf(stderr, "Usage: %s p c i b d\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    total_producers = atoi(argv[1]);
    total_consumers = atoi(argv[2]);
    items_per_producer = atoi(argv[3]);
    buffer_size = atoi(argv[4]);
    delay = atoi(argv[5]);

    if (total_consumers >= total_producers * items_per_producer) {
        fprintf(stderr, "Error: Number of consumers should be smaller than the total items being produced.\n");
        exit(EXIT_FAILURE);
    }

    pthread_t producers[MAX_THREADS], consumers[MAX_THREADS];
    int producer_ids[MAX_THREADS], consumer_ids[MAX_THREADS];

    // Create producer threads
    for (int i = 0; i < total_producers; ++i) {
        producer_ids[i] = i;
        pthread_create(&producers[i], NULL, producer, &producer_ids[i]);
    }

    // Create consumer threads
    for (int i = 0; i < total_consumers; ++i) {
        consumer_ids[i] = i;
        pthread_create(&consumers[i], NULL, consumer, &consumer_ids[i]);
    }

    // Wait for all producer threads to finish
    for (int i = 0; i < total_producers; ++i) {
        pthread_join(producers[i], NULL);
    }

    // Wait for all consumer threads to finish
    for (int i = 0; i < total_consumers; ++i) {
        pthread_join(consumers[i], NULL);
    }

    return 0;
}
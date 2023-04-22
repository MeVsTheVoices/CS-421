#include "main.h"

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include <semaphore.h>

#include <sys/types.h>
#include <unistd.h>
#include <sys/syscall.h>

typedef int buffer_item;

const int BUFFER_CEIL = 5;

int main(int argc, char **argv) {
    // organize CLI arguments

    // reject any attempt to run with invalid arguments
    // no greater than 10 seconds or 10 of each type of thread
    if (argc != 4 || atoi(argv[1]) < 1 || atoi(argv[2]) < 1 || atoi(argv[3]) < 1
                  || atoi(argv[1]) > 9 || atoi(argv[2]) > 9 || atoi(argv[3]) > 9)
        printf("Usage: %s <time_to_run> <num_producers> <num_consumers>\n", argv[0]);

    
    int time_to_run =   atoi(argv[1]);
    int num_producers = atoi(argv[2]);
    int num_consumers = atoi(argv[3]);

    pthread_t producer_threads[num_producers];
    pthread_t consumer_threads[num_consumers];

    // create a buffer for producer/consumer

    int buffer[BUFFER_CEIL];

    
    // create produce threads
    pthread_attr_t thread_attributes;

    for (int i = 0; i < num_producers; i++)
        pthread_create(&producer_threads[i], &thread_attributes, 
                        produce, &buffer);

    // create consumer threads

    for (int i = 0; i < num_consumers; i++)
        pthread_create(&consumer_threads[i], &thread_attributes,
                        consume, &buffer);

    // sleep

    void* status;
    pthread_join(consumer_threads[num_consumers - 1], status); 
    pthread_join(producer_threads[num_producers - 1], status);
    return 0;
}

void produce(int* buffer) {
    printf("Creating producer thread with id %d\n", 1); 
}

void consume(int* buffer) {
    printf("Creating consumer thread with id %d\n", 2); 
}

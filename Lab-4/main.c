#include "main.h"

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include <sys/semaphore.h>

typedef int buffer_item;

 int main(int argc, char **argv) {
    // organize CLI arguments

    // reject any attempt to run with invalid arguments
    // no greater than 10 seconds or 10 of each type of thread
    if (argc != 4 || atoi(argv[1]) < 1 || atoi(argv[2]) < 1 || atoi(argv[3]) < 1
                  || atoi(argv[1]) > 9 || atoi(argv[2]) > 9 || atoi(argv[3]) > 9)
        printf("Usage: %s <time_to_run> <num_producers> <num_consumers>", argv[0]);

    
    int time_to_run =   atoi(argv[1]);
    int num_producers = atoi(argv[2]);
    int num_consumers = atoi(argv[3]);

    pthread_t producer_threads[num_producers];
    pthread_t consumer_threads[num_consumers];

    // create a buffer for producer/consumer

    int buffer_ceil = 5;
    sem_t mutex;
    sem_t empty;
    sem_t full;

    if (sem_init(&mutex, 0, 1) != 0) {
        perror("sem_init");
        exit(1);
    } else if (sem_init(&empty, 0, buffer_ceil) != 0) {
        perror("sem_init");
        exit(1);
    } else if (sem_init(&full, 0, 0) != 0) {
        perror("sem_init");
        exit(1);
    }
    /*buffer_item buffer[buffer_ceil];

    pthread_attr_t thread_attributes;
    
    // create produce threads

    for (int i = 0; i < num_producers; i++)
        pthread_create(&producer_threads[i], &thread_attributes, produce, (void *) &producer_threads[i]);

    // create consumer threads

    // sleep*/

    return 0;
}

void produce(void* arg) {

}

void consume(void* arg) {

}
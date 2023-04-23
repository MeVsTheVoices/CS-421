#include "main.h"

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include <semaphore.h>

//for system calls
#include <sys/types.h>
#include <unistd.h>
#include <sys/syscall.h>

//for random integers
#include <time.h>

typedef int buffer_item;

const int BUFFER_CEIL = 5;
const int SLEEP_FLOOR = 1;
const int SLEEP_CEIL = 3;
const int DATA_FLOOR = 1;
const int DATA_CEIL = 100;

int currentBufferIndex = 0;

// be sure to correlate with BUFFER_CEIL
int buffer[5];

semaphore mutex = 1;
semaphore empty = BUFFER_CEIL;
semaphore full  = 0;


int main(int argc, char **argv) {
    srand(time(NULL));
    // organize CLI arguments

    // reject any attempt to run with invalid arguments
    // no greater than 10 seconds or 10 of each type of thread
    if (argc != 4 || atoi(argv[1]) < 1 || atoi(argv[2]) < 1 || atoi(argv[3]) < 1
                  || atoi(argv[1]) > 9 || atoi(argv[2]) > 9 || atoi(argv[3]) > 9) {
        printf("Usage: %s <time_to_run> <num_producers> <num_consumers>\n", argv[0]);
        exit(-1);
    }

    
    int time_to_run =   atoi(argv[1]);
    int num_producers = atoi(argv[2]);
    int num_consumers = atoi(argv[3]);

    pthread_t producer_threads[num_producers];
    pthread_t consumer_threads[num_consumers];

    // create a buffer for producer/consumer

    int buffer[BUFFER_CEIL];
    
    //create necessary synchronization tools
    
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

void produce(void* status) {
    // produce and consume are mostly alike
    pid_t thread_id = syscall(__NR_gettid);
    // print thread id
    printf("Creating producer thread with id %d\n", thread_id); 
    
    buffer_item item;

    while (1) {
        // keep going through, we're either sleeping or waiting
        int time_to_sleep = (rand() % SLEEP_CEIL) + SLEEP_FLOOR;
        sleep(time_to_sleep);
        
        int data = (rand() % DATA_CEIL) + DATA_FLOOR;
        // printf("produced a %d having slept %d seconds in thread id %d\n", 
        //         data, time_to_sleep, thread_id);

        sem_wait(empty);
        sem_wait(mutex);

        // critical section
        
        buffer[currentBufferIndex] = data;
        printf("Producer thread %d inserted value %d\n", thread_id, data);
        currentBufferIndex++;

        sem_post(mutex);
        sem_post(full);
    }
}

void consume(void* status) {
    pid_t thread_id = syscall(__NR_gettid);
    // print thread id
    printf("Creating consumer thread with id %d\n", thread_id); 

    buffer_item item;

    while (1) {
        int time_to_sleep = (rand() % SLEEP_CEIL) + SLEEP_FLOOR;
        sleep(time_to_sleep);
        
        // printf("consumed having slept %d seconds in thread id %d\n", 
        //         time_to_sleep, thread_id); 
   
        sem_wait(full);
        sem_wait(mutex);

        // critical section
        
        printf("Consumer thread %d removed value %d\n",
                thread_id, buffer[currentBufferIndex - 1]);
        currentBufferIndex--;

        sem_post(mutex);
        sem_post(empty);
    }
}

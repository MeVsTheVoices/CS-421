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

// correlate with buffer
const int BUFFER_CEIL = 10;

const int SLEEP_FLOOR = 1;
const int SLEEP_CEIL = 3;
const int DATA_FLOOR = 1;
const int DATA_CEIL = 100;

int currentBufferIndex = 0;

// be sure to correlate with BUFFER_CEIL
int buffer[10];

sem_t mutex;
sem_t empty;
sem_t full;


int main(int argc, char **argv) {
    srand(time(NULL));
    printf("Main thread beginning\n");
    // organize CLI arguments

    // reject any attempt to run with invalid arguments
    // no greater than 10 seconds or 10 of each type of thread
    if (argc != 4 || atoi(argv[1]) < 1 || atoi(argv[2]) < 1 || atoi(argv[3]) < 1
                  || atoi(argv[1]) >90 || atoi(argv[2]) > 9 || atoi(argv[3]) > 9) {
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
    
    if (sem_init(&mutex, 0, 1) != 0) {
        perror("error creating semaphore\n");
        exit(-1);
    } else if (sem_init(&empty, 0, BUFFER_CEIL) != 0) {
        perror("error creating semaphore\n");
        exit(-1);
    } else if (sem_init(&full, 0, 0) != 0) {
        perror("error creating semaphore\n");
        exit(-1);
    }

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

    printf("Main thread sleeping for %d seconds\n", time_to_run);
    sleep(time_to_run);
    return 0;
}

// my insert function
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

        sem_wait(&empty);
        sem_wait(&mutex);

        // critical section
        
        buffer[currentBufferIndex] = data;
        printf("Producer thread %d inserted value %d\n", thread_id, data);
        for (int i = 0; i < BUFFER_CEIL; i++) {
            printf("[%d]", buffer[i]);
        }
        printf("\n");
        currentBufferIndex++;

        sem_post(&mutex);
        sem_post(&full);
    }
}

// my remove function
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
   
        sem_wait(&full);
        sem_wait(&mutex);

        // critical section
        
        printf("Consumer thread %d removed value %d\n",
                thread_id, buffer[currentBufferIndex - 1]);
        buffer[currentBufferIndex - 1] = 0;
        for (int i = 0; i < BUFFER_CEIL; i++) {
            printf("[%d]", buffer[i]);
        }
        printf("\n");
        currentBufferIndex--;

        sem_post(&mutex);
        sem_post(&empty);
    }
}

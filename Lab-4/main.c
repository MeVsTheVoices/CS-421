#include "main.h"

#include <stdlib.h>
#include <pthread.h>

 int main(int argc, char **argv) {
    // organize CLI arguments

    // reject any attempt to run with invalid arguments
    // no greater than 10 seconds or 10 of each type of thread
    if (argc != 3 || atoi(argv[1]) < 1 || atoi(argv[2]) < 1 || atoi(argv[3]) < 1
                  || atoi(argv[1]) > 9 || atoi(argv[2]) > 9 || atoi(argv[3]) > 9)
        printf("Usage: %s <time_to_run> <num_producers> <num_consumers>", argv[0]);

    
    int time_to_run =   atoi(argv[1]);
    int num_producers = atoi(argv[2]);
    int num_consumers = atoi(argv[3]);



    // create a buffer for producer/consumer
    
    // create produce threads

    // create consumer threads

    // sleep

    return 0;
}
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Must supply two integers as arguments\n");
    }
    int x = atoi(argv[1]);
    int y = atoi(argv[2]);
    printf("First parameter is %d\n", x);
    //printf("First parameter is %s\n", argv[1]);
    printf("Second parameter is %d\n", y);
    //printf("Second parameter is %s\n", argv[2]);
    printf("Product is %d\n", x * y);

    return 0;
}

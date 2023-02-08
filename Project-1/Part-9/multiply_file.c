#include <stdio.h>
#include <stdlib.h>

void main() {
    FILE *fp;
    int a, b;

    fp = fopen("inputfile", "r");
    fscanf(fp, "%d", &a);
    fscanf(fp, "%d", &b);

    printf("Reading first parameter %d\n", a);
    printf("Reading second parameter %d\n", b);
    printf("Product is %d\n", a * b);
}

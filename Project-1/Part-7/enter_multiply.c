#include <stdio.h>
#include <stdlib.h>

void main() {
    int first, second;
    printf("Please enter the first parameter: ");
    scanf("%d", &first);
    printf("Please enter the second paramter: ");
    scanf("%d", &second);
    printf("First parameter is %d\n", first);
    printf("Second parameter is %d\n", second);
    printf("Product is %d\n", first * second);
}


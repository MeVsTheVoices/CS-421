#include <tell_time.h>
#include <return_numbers.h>


void main() {
    int a[100];

    tell_time();

    return_numbers(a, 100);
    for (int i = 0; i < 10; i++) {
        printf("%d\t", a[i]);
    }

    printf("\n");
	return;
}


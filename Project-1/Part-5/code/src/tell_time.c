
#include <tell_time.h>

void tell_time() {
    time_t seconds;

    seconds = time(NULL);
    printf("%ld\n", seconds);

    return;
}

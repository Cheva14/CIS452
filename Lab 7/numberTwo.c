#include <stdio.h>
#include <unistd.h>
#include <limits.h>
#include <errno.h>

int main() {
    long max_sem_value = sysconf(_SC_SEM_VALUE_MAX);

    if (max_sem_value == -1 && errno == EINVAL) {
        printf("Static maximum value of a counting semaphore is not supported or defined on this system.\n");
    } else {
        printf("Static maximum value of a counting semaphore: %ld\n", max_sem_value);
    }

    return 0;
}

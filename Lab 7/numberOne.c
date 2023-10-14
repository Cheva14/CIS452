#include <stdio.h>
#include <unistd.h>
#include <limits.h>
#include <errno.h>

int main()
{
    long max_semaphores = sysconf(_SC_SEM_NSEMS_MAX);

    if (max_semaphores == -1 && errno == EINVAL)
    {
        printf("Static maximum number of semaphores per process is not supported or defined on this system.\n");
    }
    else
    {
        printf("Static maximum number of semaphores per process: %ld\n", max_semaphores);
    }

    return 0;
}

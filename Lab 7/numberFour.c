#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>

int main()
{
  int shmid;
  size_t size = 1; // Start with a small size

  while (1)
  {
    shmid = shmget(IPC_PRIVATE, size, IPC_CREAT | 0666);
    if (shmid == -1)
    {
      printf("Maximum size of shared memory segment: %zu bytes\n", size);
      break;
    }
    size *= 2; // Double the size for the next iteration
  }

  return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <signal.h>
#include <unistd.h>

#define FOO 4096

void sigint_handler(int);

int shmId;
char *sharedMemoryPtr;

int main()
{
  signal(SIGINT, sigint_handler);

  key_t key = ftok(".", 's');
  if (key < 0)
  {
    perror("Error with ftok.\n");
    exit(1);
  }

  if ((shmId = shmget(key, FOO, 0666)) < 0)
  {
    perror("Unable to get shared memory.\n");
    exit(1);
  }
  if ((sharedMemoryPtr = shmat(shmId, 0, 0)) == (void *)-1)
  {
    perror("Unable to attach.\n");
    exit(1);
  }

  while (1)
  {
    sleep(2);
    char *newString = sharedMemoryPtr + sizeof(int);
    printf("Reader: %s\n", newString);
  }

  if (shmdt(sharedMemoryPtr) < 0)
  {
    perror("Unable to detach.\n");
    exit(1);
  }

  return 0;
}

void sigint_handler(int signum)
{
  if (shmdt(sharedMemoryPtr) < 0)
  {
    perror("Unable to detach.\n");
    exit(1);
  }

  exit(0);
}

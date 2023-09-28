#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <signal.h>

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

  if ((shmId = shmget(key, FOO, IPC_CREAT | 0666)) < 0)
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
    printf("Enter a string (or 'quit' to exit): ");
    char input[256];
    fgets(input, 256, stdin);
    input[strlen(input) - 1] = '\0';
    if (strcmp(input, "quit") == 0)
      break;
    strcpy(sharedMemoryPtr + sizeof(int), input);
  }

  if (shmdt(sharedMemoryPtr) < 0)
  {
    perror("Unable to detach.\n");
    exit(1);
  }
  if (shmctl(shmId, IPC_RMID, 0) < 0)
  {
    perror("Unable to deallocate.\n");
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
  if (shmctl(shmId, IPC_RMID, 0) < 0)
  {
    perror("Unable to deallocate.\n");
    exit(1);
  }

  exit(0);
}

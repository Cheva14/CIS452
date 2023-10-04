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
void *sharedMemoryPtr;

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

  // Pointer to the flag indicating if a new string is available
  int *newStringFlag = (int *)sharedMemoryPtr;

  while (1)
  {
    while (*newStringFlag > 0)
    {
      usleep(10000);
    }

    printf("Enter a string (or 'quit' to exit): ");
    char input[100];
    fgets(input, 100, stdin);
    input[strlen(input) - 1] = '\0';
    if (strcmp(input, "quit") == 0)
      break;
    *newStringFlag = 2;
    strcpy((char *)sharedMemoryPtr + sizeof(int), input);
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

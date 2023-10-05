#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/sem.h>

#define SIZE 16
struct sembuf p = {0, -1, SEM_UNDO};
struct sembuf v = {0, +1, SEM_UNDO};

int main(int argc, char *argv[])
{
  int status;
  long int i, loop, temp, *sharedMemoryPointer;
  int sharedMemoryID;
  pid_t pid;
  int semId;

  loop = atoi(argv[1]);

  sharedMemoryID = shmget(IPC_PRIVATE, SIZE, IPC_CREAT | S_IRUSR | S_IWUSR);
  if (sharedMemoryID < 0)
  {
    perror("Unable to obtain shared memory\n");
    exit(1);
  }

  sharedMemoryPointer = shmat(sharedMemoryID, 0, 0);
  if (sharedMemoryPointer == (void *)-1)
  {
    perror("Unable to attach\n");
    exit(1);
  }

  sharedMemoryPointer[0] = 0;
  sharedMemoryPointer[1] = 1;

  // Create a semaphore
  if ((semId = semget(IPC_PRIVATE, 1, S_IRUSR | S_IWUSR)) == -1)
  {
    perror("semget: semget failed");
    exit(1);
  }

  // Initialize the semaphore
  if (semop(semId, &v, 1) == -1)
  {
    perror("semop: semop initialization failed");
    exit(1);
  }

  pid = fork();
  if (pid < 0)
  {
    printf("Fork failed\n");
  }

  if (pid == 0)
  {
    // Child

    for (i = 0; i < loop; i++)
    {
      semop(semId, &p, 1); // Wait (lock) the semaphore

      temp = sharedMemoryPointer[0];
      sharedMemoryPointer[0] = sharedMemoryPointer[1];
      sharedMemoryPointer[1] = temp;

      semop(semId, &v, 1); // Release (unlock) the semaphore
    }
    if (shmdt(sharedMemoryPointer) < 0)
    {
      perror("Unable to detach\n");
      exit(1);
    }
    exit(0);
  }
  else
  {

    for (i = 0; i < loop; i++)
    {
      semop(semId, &p, 1); // Wait (lock) the semaphore

      temp = sharedMemoryPointer[1];
      sharedMemoryPointer[1] = sharedMemoryPointer[0];
      sharedMemoryPointer[0] = temp;

      semop(semId, &v, 1); // Release (unlock) the semaphore
    }
  }

  wait(&status);
  printf("Values: %li\t%li\n", sharedMemoryPointer[0], sharedMemoryPointer[1]);

  if (shmdt(sharedMemoryPointer) < 0)
  {
    perror("Unable to detach\n");
    exit(1);
  }
  if (shmctl(sharedMemoryID, IPC_RMID, 0) < 0)
  {
    perror("Unable to deallocate\n");
    exit(1);
  }

  if (semctl(semId, 0, IPC_RMID) == -1)
  {
    perror("Unable to remove semaphore\n");
    exit(1);
  }

  return 0;
}
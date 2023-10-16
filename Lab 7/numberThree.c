#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <sys/sem.h>

struct sembuf v = {0, +1, SEM_UNDO};

int main()
{
  int semId;
  if ((semId = semget(IPC_PRIVATE, 1, S_IRUSR | S_IWUSR)) == -1)
  {
    perror("semget: semget failed");
    exit(1);
  }

  int maxValue = -1;
  while (1)
  {
    if (maxValue > semctl(semId, 0, GETVAL))
    {
      printf("Maximum value of semaphore: %d\n", maxValue);
      if (semctl(semId, 0, IPC_RMID) == -1)
      {
        perror("Unable to remove semaphore\n");
        exit(1);
      }
      return 0;
    }
    maxValue = semctl(semId, 0, GETVAL);
    semop(semId, &v, 1);
  }
}

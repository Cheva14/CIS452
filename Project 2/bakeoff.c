#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/sem.h>
#include <pthread.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <time.h>

struct sembuf p = {0, -1, SEM_UNDO};
struct sembuf v = {0, +1, SEM_UNDO};

// Define semaphores for resources
int mixerSem, pantrySem, refrigeratorSem, bowlSem, spoonSem, ovenSem;

// Define shared memory for resources
int mixer = 2;
int pantry = 1;
int refrigerator = 2;
int bowl = 3;
int spoon = 5;
int oven = 1;

// Define a data structure to track each baker's progress
typedef struct
{
  int cookies;
  int pancakes;
  int pizza;
  int pretzels;
  int rolls;
} BakerProgress;

BakerProgress progress[3];

void *baker_thread(void *args)
{
  int baker_id = *((int *)args);
  printf("baker %d created\n", baker_id);

  while (progress[baker_id].cookies < 1)
  {
    // Baker is making cookies
    // Acquire ingredients from the pantry
    if (pantry > 0)
    {
      pantry -= 1;

      sleep(1); // Mix ingredients
      printf("baker %d is mixing ingredients\n", baker_id);

      sleep(2); // Bake cookies
      printf("baker %d is baking cookies\n", baker_id);

      progress[baker_id].cookies++;
      printf("Baker %d made cookies\n", baker_id);
      pantry += 1;
    }
    else
    {
    }
  }

  // Similar logic for making pancakes and pretzels
  // ...

  // Baker has completed all recipes
  printf("Baker %d finished baking\n", baker_id);
  return NULL;
}

int main(int argc, char *argv[])
{

  // long int numberOfBakers = atoi(argv[1]);
  long int numberOfBakers = 3;
  BakerProgress progress[numberOfBakers];

  pthread_t threads[numberOfBakers];
  int baker_ids[numberOfBakers];

  // Initialize semaphores
  // mixerSem = semget(IPC_PRIVATE, 2, S_IRUSR | S_IWUSR);
  // pantrySem = semget(IPC_PRIVATE, 1, S_IRUSR | S_IWUSR);
  // refrigeratorSem = semget(IPC_PRIVATE, 2, S_IRUSR | S_IWUSR);
  // bowlSem = semget(IPC_PRIVATE, 3, S_IRUSR | S_IWUSR);
  // spoonSem = semget(IPC_PRIVATE, 5, S_IRUSR | S_IWUSR);
  // ovenSem = semget(IPC_PRIVATE, 1, S_IRUSR | S_IWUSR);

  // Initialize baker progress
  for (int i = 0; i < numberOfBakers; i++)
  {
    progress[i].cookies = 0;
    progress[i].pancakes = 0;
    progress[i].pretzels = 0;
    baker_ids[i] = i;
    pthread_create(&threads[i], NULL, baker_thread, &baker_ids[i]);
  }
  printf("done initializing baker progress\n");
  // Wait for all baker threads to finish
  for (int i = 0; i < numberOfBakers; i++)
  {
    pthread_join(threads[i], NULL);
  }
  printf("done waiting for all baker threads to finish\n");

  return 0;
}

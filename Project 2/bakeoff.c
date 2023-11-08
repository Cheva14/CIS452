#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/sem.h>
#include <pthread.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <time.h>
#include <string.h>

#define ANSI_COLOR_RESET "\x1B[0m"
#define ANSI_COLOR_FORMAT "\x1B[38;2;%d;%d;%dm"
#define RED "\x1B[31m"
#define GREEN "\x1B[32m"
#define YELLOW "\x1B[33m"
#define BLUE "\x1B[34m"
#define MAGENTA "\x1B[35m"
#define CYAN "\x1B[36m"
#define RESET "\x1B[0m"

char *colors[] = {RED, GREEN, YELLOW, BLUE, MAGENTA, CYAN};
int colorsrgb[256][256][256] = {0}; // Store generated colors

struct sembuf p = {0, -1, SEM_UNDO};
struct sembuf v = {0, +1, SEM_UNDO};

// Define semaphores for resources
int mixerSem, pantrySem, refrigeratorSem, bowlSem, spoonSem, ovenSem;

// Define shared memory for resources
int mixer = 2;
int pantry = 10;
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

BakerProgress progress[0];

struct ThreadArgs
{
  int id;
  char *color;
};

// Function to generate a random color
char *getColor()
{
  int r, g, b;

  srand(time(NULL));

  do
  {
    r = rand() % 256;
    g = rand() % 256;
    b = rand() % 256;
  } while (colorsrgb[r][g][b]); // Check if color already exists

  colorsrgb[r][g][b] = 1; // Mark color as used

  char *colorString = (char *)malloc(20 * sizeof(char));
  sprintf(colorString, ANSI_COLOR_FORMAT, r, g, b);

  return colorString;
}

void *baker_thread(void *args)
{
  struct ThreadArgs *thread_args = (struct ThreadArgs *)args;
  int baker_id = thread_args->id;
  char *color = thread_args->color;
  int baker = baker_id + 1;

  // Baker is making cookies
  while (1)
  { // Acquire ingredients from the pantry

    semop(pantrySem, &p, 1);
    printf("%sbaker %d is in the pantry\n", color, baker);
    // grabs Flour and Sugar
    semop(pantrySem, &v, 1);
    printf("%sbaker %d left the pantry\n", color, baker);

    semop(refrigeratorSem, &p, 1);
    printf("%sbaker %d is in the refrigerator\n", color, baker);
    // grabs Milk and Butter
    semop(refrigeratorSem, &v, 1);
    printf("%sbaker %d left the refrigerator\n", color, baker);

    semop(bowlSem, &p, 1);
    semop(spoonSem, &p, 1);
    semop(mixerSem, &p, 1);
    printf("%sbaker %d got a bowl, a spoon, and a mixer\n", color, baker);
    printf("%sbaker %d is mixing ingredients\n", color, baker);
    semop(bowlSem, &v, 1);
    semop(spoonSem, &v, 1);
    semop(mixerSem, &v, 1);
    printf("%sbaker %d gave back a bowl, a spoon, and a mixer\n", color, baker);
    printf("%sbaker %d is baking cookies\n", color, baker);
    progress[baker_id].cookies++;
    printf("%sBaker %d made cookies\n", color, baker);
    break;
  }

  // Baker is making pancakes
  while (1)
  { // Acquire ingredients from the pantry

    semop(pantrySem, &p, 1);
    printf("%sbaker %d is in the pantry\n", color, baker);
    // grabs Flour, Sugar, Baking Soda, and Salt
    semop(pantrySem, &v, 1);
    printf("%sbaker %d left the pantry\n", color, baker);

    semop(refrigeratorSem, &p, 1);
    printf("%sbaker %d is in the refrigerator\n", color, baker);
    // grabs Egg, Milk, and Butter
    semop(refrigeratorSem, &v, 1);
    printf("%sbaker %d left the refrigerator\n", color, baker);

    semop(bowlSem, &p, 1);
    semop(spoonSem, &p, 1);
    semop(mixerSem, &p, 1);
    printf("%sbaker %d got a bowl, a spoon, and a mixer\n", color, baker);
    printf("%sbaker %d is mixing ingredients\n", color, baker);
    semop(bowlSem, &v, 1);
    semop(spoonSem, &v, 1);
    semop(mixerSem, &v, 1);
    printf("%sbaker %d gave back a bowl, a spoon, and a mixer\n", color, baker);
    printf("%sbaker %d is baking pancakes\n", color, baker);
    progress[baker_id].pancakes++;
    printf("%sBaker %d made pancakes\n", color, baker);
    break;
  }

  // Similar logic for making pancakes and pretzels
  // ...

  // Baker has completed all recipes
  printf("%sBaker %d finished baking\n", color, baker);
  return NULL;
}

int main()
{
  char text[256];
  int rgb = 1;

  while (1)
  {
    printf("\x1B[38;2;173;250;183mIs this text light green? (y/n): ");

    if (fgets(text, sizeof(text), stdin) != NULL)
    {
      if (strcmp(text, "y\n") == 0)
      {
        break;
      }
      else if (strcmp(text, "n\n") == 0)
      {
        rgb = 0;
        break;
      }
      else
      {
        printf("Invalid input. Please enter 'y' if it was red or 'n' if it wasn't red.\n");
      }
    }
    else
    {
      fprintf(stderr, "fgets failure: Invalid answer");
      exit(1);
    }
  }
  printf("\x1B[0m"); // Reset the text color to the default

  // get baker amount
  int numberOfBakers;
  char kInput[256];
  while (1)
  {
    printf("Enter an ammount of bakers: ");

    if (fgets(kInput, sizeof(kInput), stdin) != NULL)
    {
      if (sscanf(kInput, "%d", &numberOfBakers) == 1 && numberOfBakers > 0)
      {
        break;
      }
      else
      {
        printf("Invalid input. Please enter a natural number greater than 0.\n");
      }
    }
    else
    {
      fprintf(stderr, "fgets failure: Invalid baker number");
      exit(1);
    }
  }
  BakerProgress progress[numberOfBakers];

  pthread_t threads[numberOfBakers];
  struct ThreadArgs thread_args[numberOfBakers];

  // Initialize semaphores
  mixerSem = semget(IPC_PRIVATE, 1, S_IRUSR | S_IWUSR);
  pantrySem = semget(IPC_PRIVATE, 1, S_IRUSR | S_IWUSR);
  refrigeratorSem = semget(IPC_PRIVATE, 1, S_IRUSR | S_IWUSR);
  bowlSem = semget(IPC_PRIVATE, 1, S_IRUSR | S_IWUSR);
  spoonSem = semget(IPC_PRIVATE, 1, S_IRUSR | S_IWUSR);
  ovenSem = semget(IPC_PRIVATE, 1, S_IRUSR | S_IWUSR);
  semctl(mixerSem, 0, SETVAL, 2);
  semctl(pantrySem, 0, SETVAL, 1);
  semctl(refrigeratorSem, 0, SETVAL, 2);
  semctl(bowlSem, 0, SETVAL, 3);
  semctl(spoonSem, 0, SETVAL, 5);
  semctl(ovenSem, 0, SETVAL, 1);

  // Initialize baker progress
  for (int i = 0; i < numberOfBakers; i++)
  {
    progress[i].cookies = 0;
    progress[i].pancakes = 0;
    progress[i].pizza = 0;
    progress[i].pretzels = 0;
    progress[i].rolls = 0;

    thread_args[i].id = i;
    if (rgb == 1)
    {
      thread_args[i].color = getColor();
    }
    else
    {
      thread_args[i].color = colors[i % (sizeof(colors) / sizeof(colors[0]))];
    }
    pthread_create(&threads[i], NULL, baker_thread, &thread_args[i]);
    printf("%sbaker %d created\n", thread_args[i].color, thread_args[i].id);
  }
  // Wait for all baker threads to finish
  for (int i = 0; i < numberOfBakers; i++)
  {
    pthread_join(threads[i], NULL);
  }
  printf("\x1B[0m"); // Reset the text color to the default

  printf("done waiting for all bakers to finish\n");
  semctl(mixerSem, 0, IPC_RMID);
  semctl(pantrySem, 0, IPC_RMID);
  semctl(refrigeratorSem, 0, IPC_RMID);
  semctl(bowlSem, 0, IPC_RMID);
  semctl(spoonSem, 0, IPC_RMID);
  semctl(ovenSem, 0, IPC_RMID);
  return 0;
}

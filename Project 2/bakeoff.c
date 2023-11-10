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
int colorsrgb[256][256][256] = {0};

struct sembuf p = {0, -1, SEM_UNDO};
struct sembuf v = {0, +1, SEM_UNDO};

int mixerSem, pantrySem, fridgeSem, bowlSem, spoonSem, ovenSem;

struct ThreadArgs
{
  int id;
  char *color;
  int ramsied;
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

// Function with code for each thread
void *baker_thread(void *args)
{
  struct ThreadArgs *thread_args = (struct ThreadArgs *)args;
  int baker_id = thread_args->id;
  char *color = thread_args->color;
  int ramsied = thread_args->ramsied;
  int baker = baker_id + 1;
  int ramsiedRecipe = 5;

  // Choose a recipe to ramsied if this baker is ramsied. randomly.
  if (ramsied == 1)
  {
    srand(time(NULL));
    ramsiedRecipe = rand() % 5;
  }

  // Baker is making cookies
  while (1)
  { // Acquire ingredients from the pantry
    semop(pantrySem, &p, 1);
    printf("%s[Cookies] baker %d is in the pantry\n", color, baker);
    // grabs Flour and Sugar
    semop(pantrySem, &v, 1);
    printf("%s[Cookies] baker %d left the pantry\n", color, baker);

    semop(fridgeSem, &p, 1);
    printf("%s[Cookies] baker %d is in the fridge\n", color, baker);
    // grabs Milk and Butter
    semop(fridgeSem, &v, 1);
    printf("%s[Cookies] baker %d left the fridge\n", color, baker);

    // Acquire utensils
    semop(bowlSem, &p, 1);
    semop(spoonSem, &p, 1);
    semop(mixerSem, &p, 1);
    printf("%s[Cookies] baker %d got a bowl, a spoon, and a mixer\n", color, baker);
    printf("%s[Cookies] baker %d is mixing ingredients\n", color, baker);

    // Check if you are ramsied on this recipe
    if (ramsied == 1 && ramsiedRecipe == 0)
    {
      printf("%s[Cookies] baker %d got Ramsied\n", color, baker);

      ramsied = 0;
      semop(bowlSem, &v, 1);
      semop(spoonSem, &v, 1);
      semop(mixerSem, &v, 1);
      continue;
    }

    // Use Oven
    semop(ovenSem, &p, 1);
    printf("%s[Cookies] baker %d is baking cookies in the oven\n", color, baker);

    // give back utensils while baking in the oven
    semop(bowlSem, &v, 1);
    semop(spoonSem, &v, 1);
    semop(mixerSem, &v, 1);
    printf("%s[Cookies] baker %d gave back a bowl, a spoon, and a mixer\n", color, baker);

    semop(ovenSem, &v, 1);
    printf("%s[Cookies] Baker %d made cookies\n", color, baker);
    break;
  }

  // Baker is making pancakes
  while (1)
  {
    semop(pantrySem, &p, 1);
    printf("%s[Pancake] baker %d is in the pantry\n", color, baker);
    semop(pantrySem, &v, 1);
    printf("%s[Pancake] baker %d left the pantry\n", color, baker);

    semop(fridgeSem, &p, 1);
    printf("%s[Pancake] baker %d is in the fridge\n", color, baker);
    semop(fridgeSem, &v, 1);
    printf("%s[Pancake] baker %d left the fridge\n", color, baker);

    semop(bowlSem, &p, 1);
    semop(spoonSem, &p, 1);
    semop(mixerSem, &p, 1);
    printf("%s[Pancake] baker %d got a bowl, a spoon, and a mixer\n", color, baker);
    printf("%s[Pancake] baker %d is mixing ingredients\n", color, baker);
    if (ramsied == 1 && ramsiedRecipe == 1)
    {
      printf("%s[Pancake] baker %d got Ramsied\n", color, baker);

      ramsied = 0;
      semop(bowlSem, &v, 1);
      semop(spoonSem, &v, 1);
      semop(mixerSem, &v, 1);
      continue;
    }
    semop(ovenSem, &p, 1);
    printf("%s[Pancake] baker %d is baking pancakes in the oven\n", color, baker);

    semop(bowlSem, &v, 1);
    semop(spoonSem, &v, 1);
    semop(mixerSem, &v, 1);
    printf("%s[Pancake] baker %d gave back a bowl, a spoon, and a mixer\n", color, baker);

    semop(ovenSem, &v, 1);
    printf("%s[Pancake] Baker %d made pancakes\n", color, baker);
    break;
  }

  // Baker is making pizza
  while (1)
  {
    semop(pantrySem, &p, 1);
    printf("%s[ Pizza ] baker %d is in the pantry\n", color, baker);
    semop(pantrySem, &v, 1);
    printf("%s[ Pizza ] baker %d left the pantry\n", color, baker);

    semop(fridgeSem, &p, 1);
    printf("%s[ Pizza ] baker %d is in the fridge\n", color, baker);
    semop(fridgeSem, &v, 1);
    printf("%s[ Pizza ] baker %d left the fridge\n", color, baker);

    semop(bowlSem, &p, 1);
    semop(spoonSem, &p, 1);
    semop(mixerSem, &p, 1);
    printf("%s[ Pizza ] baker %d got a bowl, a spoon, and a mixer\n", color, baker);
    printf("%s[ Pizza ] baker %d is mixing ingredients\n", color, baker);
    if (ramsied == 1 && ramsiedRecipe == 2)
    {
      printf("%s[ Pizza ] baker %d got Ramsied\n", color, baker);

      ramsied = 0;
      semop(bowlSem, &v, 1);
      semop(spoonSem, &v, 1);
      semop(mixerSem, &v, 1);
      continue;
    }
    semop(ovenSem, &p, 1);
    printf("%s[ Pizza ] baker %d is baking pancakes in the oven\n", color, baker);

    semop(bowlSem, &v, 1);
    semop(spoonSem, &v, 1);
    semop(mixerSem, &v, 1);
    printf("%s[ Pizza ] baker %d gave back a bowl, a spoon, and a mixer\n", color, baker);

    semop(ovenSem, &v, 1);
    printf("%s[ Pizza ] Baker %d made homemade pizza dough\n", color, baker);
    break;
  }
  // Baker is making pretzels
  while (1)
  {
    semop(pantrySem, &p, 1);
    printf("%s[Pretzel] baker %d is in the pantry\n", color, baker);
    semop(pantrySem, &v, 1);
    printf("%s[Pretzel] baker %d left the pantry\n", color, baker);

    semop(fridgeSem, &p, 1);
    printf("%s[Pretzel] baker %d is in the fridge\n", color, baker);
    semop(fridgeSem, &v, 1);
    printf("%s[Pretzel] baker %d left the fridge\n", color, baker);

    semop(bowlSem, &p, 1);
    semop(spoonSem, &p, 1);
    semop(mixerSem, &p, 1);
    printf("%s[Pretzel] baker %d got a bowl, a spoon, and a mixer\n", color, baker);
    printf("%s[Pretzel] baker %d is mixing ingredients\n", color, baker);
    if (ramsied == 1 && ramsiedRecipe == 3)
    {
      printf("%s[Pretzel] baker %d got Ramsied\n", color, baker);

      ramsied = 0;
      semop(bowlSem, &v, 1);
      semop(spoonSem, &v, 1);
      semop(mixerSem, &v, 1);
      continue;
    }
    semop(ovenSem, &p, 1);
    printf("%s[Pretzel] baker %d is baking pancakes in the oven\n", color, baker);

    semop(bowlSem, &v, 1);
    semop(spoonSem, &v, 1);
    semop(mixerSem, &v, 1);
    printf("%s[Pretzel] baker %d gave back a bowl, a spoon, and a mixer\n", color, baker);

    semop(ovenSem, &v, 1);
    printf("%s[Pretzel] Baker %d made soft pretzels\n", color, baker);
    break;
  }
  // Baker is making rolls
  while (1)
  {
    semop(pantrySem, &p, 1);
    printf("%s[Cn Roll] baker %d is in the pantry\n", color, baker);
    semop(pantrySem, &v, 1);
    printf("%s[Cn Roll] baker %d left the pantry\n", color, baker);

    semop(fridgeSem, &p, 1);
    printf("%s[Cn Roll] baker %d is in the fridge\n", color, baker);
    semop(fridgeSem, &v, 1);
    printf("%s[Cn Roll] baker %d left the fridge\n", color, baker);

    semop(bowlSem, &p, 1);
    semop(spoonSem, &p, 1);
    semop(mixerSem, &p, 1);
    printf("%s[Cn Roll] baker %d got a bowl, a spoon, and a mixer\n", color, baker);
    printf("%s[Cn Roll] baker %d is mixing ingredients\n", color, baker);
    if (ramsied == 1 && ramsiedRecipe == 4)
    {
      printf("[Cn Roll] %sbaker %d got Ramsied\n", color, baker);

      ramsied = 0;
      semop(bowlSem, &v, 1);
      semop(spoonSem, &v, 1);
      semop(mixerSem, &v, 1);
      continue;
    }
    semop(ovenSem, &p, 1);
    printf("%s[Cn Roll] baker %d is baking pancakes in the oven\n", color, baker);

    semop(bowlSem, &v, 1);
    semop(spoonSem, &v, 1);
    semop(mixerSem, &v, 1);
    printf("%s[Cn Roll] baker %d gave back a bowl, a spoon, and a mixer\n", color, baker);

    semop(ovenSem, &v, 1);
    printf("%s[Cn Roll] Baker %d made cinnamon rolls\n", color, baker);
    break;
  }

  printf("%s[Program] Baker %d is done!\n", color, baker);

  return NULL;
}

int main()
{
  char text[256];
  int rgb = 1;

  // set what colors to use depending on user terminal
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

  int numberOfBakers;
  char kInput[256];
  srand(time(NULL));

  // get baker amount
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

  pthread_t threads[numberOfBakers];
  struct ThreadArgs thread_args[numberOfBakers];

  // Choose a baker to ramsied
  int ramsiedBaker = rand() % numberOfBakers;

  // Initialize semaphores
  mixerSem = semget(IPC_PRIVATE, 1, S_IRUSR | S_IWUSR);
  pantrySem = semget(IPC_PRIVATE, 1, S_IRUSR | S_IWUSR);
  fridgeSem = semget(IPC_PRIVATE, 1, S_IRUSR | S_IWUSR);
  bowlSem = semget(IPC_PRIVATE, 1, S_IRUSR | S_IWUSR);
  spoonSem = semget(IPC_PRIVATE, 1, S_IRUSR | S_IWUSR);
  ovenSem = semget(IPC_PRIVATE, 1, S_IRUSR | S_IWUSR);
  semctl(mixerSem, 0, SETVAL, 2);
  semctl(pantrySem, 0, SETVAL, 1);
  semctl(fridgeSem, 0, SETVAL, 2);
  semctl(bowlSem, 0, SETVAL, 3);
  semctl(spoonSem, 0, SETVAL, 5);
  semctl(ovenSem, 0, SETVAL, 1);

  // Initialize baker progress with correct values
  for (int i = 0; i < numberOfBakers; i++)
  {

    thread_args[i].id = i;
    if (i == ramsiedBaker)
    {
      thread_args[i].ramsied = 1;
    }
    else
    {
      thread_args[i].ramsied = 0;
    }
    if (rgb == 1)
    {
      thread_args[i].color = getColor();
    }
    else
    {
      thread_args[i].color = colors[i % (sizeof(colors) / sizeof(colors[0]))];
    }
    printf("%s[Program] baker %d created\n", thread_args[i].color, thread_args[i].id + 1);
    pthread_create(&threads[i], NULL, baker_thread, &thread_args[i]);
  }
  // Wait for all baker threads to finish
  for (int i = 0; i < numberOfBakers; i++)
  {

    pthread_join(threads[i], NULL);
  }
  printf("\x1B[0m"); // Reset the text color to the default

  printf("done waiting for all bakers to finish\n");
  sleep(1);

  // clean up semaphores
  semctl(mixerSem, 0, IPC_RMID);
  semctl(pantrySem, 0, IPC_RMID);
  semctl(fridgeSem, 0, IPC_RMID);
  semctl(bowlSem, 0, IPC_RMID);
  semctl(spoonSem, 0, IPC_RMID);
  semctl(ovenSem, 0, IPC_RMID);
  return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <time.h>

void sigHandler(int);
int k;
int badApple;

typedef struct
{
  int header;
  char message[256];
} Apple;

int main()
{
  signal(SIGINT, sigHandler);

  // Set k value
  char kInput[256];
  while (1)
  {
    printf("Enter an ammount of nodes: ");

    if (fgets(kInput, sizeof(kInput), stdin) != NULL)
    {
      if (sscanf(kInput, "%d", &k) == 1 && k > 2)
      {
        break;
      }
      else
      {
        printf("Invalid input. Please enter a natural number greater than 2 to have a 'circular' comm sys.\n");
      }
    }
    else
    {
      fprintf(stderr, "fgets failure: Invalid k number");
      exit(1);
    }
  }

  // Choose a node to be the bad apple
  char badAppleInput[256];
  while (1)
  {
    printf("Choose a node to be the bad apple (type -1 if you dont want a bad apple.): ");

    if (fgets(badAppleInput, sizeof(badAppleInput), stdin) != NULL)
    {
      if (sscanf(badAppleInput, "%d", &badApple) == 1 && badApple > -2 && badApple < k)
      {

        break;
      }
      else
      {
        printf("Invalid input. Please enter a number between -1 and %d.\n", k - 1);
      }
    }
    else
    {
      fprintf(stderr, "fgets failure: Invalid bad apple node");
      exit(1);
    }
  }

  // Create pipes for communication
  int pipes[k][2];
  for (int i = 0; i < k; i++)
  {
    if (pipe(pipes[i]) < 0)
    {
      perror("Failed pipe creation\n");
      exit(1);
    }
  }

  // Spawn child processes
  int pids[k];

  for (int i = 1; i < k; i++)
  {
    int pid = fork();

    if (pid < 0)
    {
      perror("Fork failed");
      exit(1);
    }
    if (pid == 0)
    { // Node #i
      while (1)
      {
        Apple appleBuffer;
        close(pipes[(i + k - 1) % k][1]);
        read(pipes[(i + k - 1) % k][0], &appleBuffer, sizeof(appleBuffer));
        printf("[LOG] Node %d received the apple.\n", i);
        if (appleBuffer.header == i)
        {
          printf("Node %d: The message is for me! It says [%s].\n", i, appleBuffer.message);
          appleBuffer.header = -1;
        }
        else
        {
          printf("Node %d: The message is not for me!\n", i);
        }
        if (i == badApple)
        { // Modify data in the apple
          printf("Node %d: I am the bad apple\n", i);

          int numCharsToReplace = (strlen(appleBuffer.message) / 5) + 1;
          if (numCharsToReplace > 0)
          {
            srand(time(NULL));

            for (int j = 0; j < numCharsToReplace; j++)
            {
              char randomChar = 'a' + (rand() % 26);
              int randomIndex = rand() % strlen(appleBuffer.message);
              appleBuffer.message[randomIndex] = randomChar;
            }
          }
        }
        for (int i = 0; i < 1000; i++)
        {
          for (int j = 0; j < 1000000; j++)
          {
            // This loop consumes CPU cycles to create a delay of 1 second
          }
        }
        close(pipes[i][0]);
        write(pipes[i][1], &appleBuffer, sizeof(appleBuffer));
        printf("[LOG] Node %d sent the apple.\n", i);
      }
    }
    else
    { // Parent process
      pids[i] = pid;
    }
  }

  // Get message
  Apple msgFirst;
  char msgInput[256];
  printf("Enter a message: ");
  if (fgets(msgInput, sizeof(msgInput), stdin) == NULL)
  {
    fprintf(stderr, "Fgets failure: %s", strerror(errno));
    exit(1);
  }
  msgInput[strcspn(msgInput, "\n")] = '\0';
  strncpy(msgFirst.message, msgInput, sizeof(msgFirst.message));

  // Get node header
  char nodeInput[100];
  int node;
  while (1)
  {
    printf("What node to send the message to: ");

    if (fgets(nodeInput, sizeof(nodeInput), stdin) != NULL)
    {
      if (sscanf(nodeInput, "%d", &node) == 1 && node > -1 && node < k)
      {
        msgFirst.header = node;
        break;
      }
      else
      {
        printf("Invalid input. Please enter a number between 0 and %d.\n", k - 1);
      }
    }
    else
    {
      fprintf(stderr, "fgets failure: Invalid node number");
      exit(1);
    }
  }

  // Write the message into node 0
  write(pipes[k - 1][1], &msgFirst, sizeof(msgFirst));

  while (1)
  { // Node 0
    Apple appleBuffer;
    close(pipes[(0 + (k - 1)) % k][1]);
    read(pipes[(0 + (k - 1)) % k][0], &appleBuffer, sizeof(appleBuffer));
    printf("[LOG] Node %d received the apple.\n", 0);
    if (appleBuffer.header == -1)
    {
      printf("Apple has an empty header. Send another message.\n");

      // Get message
      printf("Enter a message: ");
      if (fgets(msgInput, sizeof(msgInput), stdin) == NULL)
      {
        fprintf(stderr, "Fgets failure: %s", strerror(errno));
        exit(1);
      }
      msgInput[strcspn(msgInput, "\n")] = '\0';
      strncpy(appleBuffer.message, msgInput, sizeof(appleBuffer.message));

      // Get node header
      while (1)
      {
        printf("What node to send the message to: ");

        if (fgets(nodeInput, sizeof(nodeInput), stdin) != NULL)
        {
          if (sscanf(nodeInput, "%d", &node) == 1 && node > -1 && node < k)
          {
            appleBuffer.header = node;
            break;
          }
          else
          {
            printf("Invalid input. Please enter a number between 0 and %d.\n", k - 1);
          }
        }
        else
        {
          fprintf(stderr, "fgets failure: Invalid node number");
          exit(1);
        }
      }
      printf("[LOG] Node %d received the apple.\n", 0);
    }

    if (appleBuffer.header == 0)
    {
      printf("Node %d: The message is for me! It says [%s].\n", 0, appleBuffer.message);
      appleBuffer.header = -1;
    }
    else
    {
      printf("Node %d: The message is not for me!\n", 0);
    }

    if (0 == badApple)
    {
      printf("Node %d: I am the bad apple\n", 0);

      int numCharsToReplace = strlen(appleBuffer.message) / 5 + 1;

      if (numCharsToReplace > 0)
      {
        srand(time(NULL));

        for (int j = 0; j < numCharsToReplace; j++)
        {
          char randomChar = 'A' + (rand() % 26);
          int randomIndex = rand() % strlen(appleBuffer.message);
          appleBuffer.message[randomIndex] = randomChar;
        }
      }
    }

    for (int i = 0; i < 1000; i++)
    {
      for (int j = 0; j < 1000000; j++)
      {
      }
    }
    close(pipes[0][0]);
    write(pipes[0][1], &appleBuffer, sizeof(appleBuffer));
    printf("[LOG] Node %d sent the apple.\n", 0);
  }

  return 0;
}

// Function to handle Ctrl+C
void sigHandler(int sigNum)
{
  printf("Process #%d Shutting down...\n", getpid());
  exit(0);
}
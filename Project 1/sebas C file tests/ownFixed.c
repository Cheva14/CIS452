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
  // Set k value
  char kInput[256];
  while (1)
  {
    printf("Enter a number for k: ");

    if (fgets(kInput, sizeof(kInput), stdin) != NULL)
    {
      if (sscanf(kInput, "%d", &k) == 1 && k > 2)
      {
        break;
      }
      else
      {
        printf("Invalid input. Please enter a natural number greater than 2 for k.\n");
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
        printf("Node %d received the apple.\n", i);
        if (appleBuffer.header == i)
        {
          printf("The message was for me! It says [%s].\n", appleBuffer.message);
          appleBuffer.header = -1;
        }
        else
        {
          printf("The message was not for me!\n");
        }
        if (i == badApple)
        {
          // Calculate the number of characters to replace as half the length of appleBuffer.message
          printf("I am the bad apple\n");

          int numCharsToReplace = (strlen(appleBuffer.message) / 5) + 1;
          // Check that numCharsToReplace is greater than 0 to avoid issues
          if (numCharsToReplace > 0)
          {
            srand(time(NULL)); // Initialize the random number generator

            for (int j = 0; j < numCharsToReplace; j++)
            {
              // Generate a random character and replace it in msg.message
              char randomChar = 'a' + (rand() % 26); // Replace with the desired character range
              int randomIndex = rand() % strlen(appleBuffer.message);
              appleBuffer.message[randomIndex] = randomChar;
            }
          }
        }
        sleep(1);
        close(pipes[i][0]);
        write(pipes[i][1], &appleBuffer, sizeof(appleBuffer));
        printf("Node %d sent the apple.\n", i);
      }
    }
    else
    { // Parent process
      pids[i] = pid;
    }
  }

  signal(SIGINT, sigHandler);

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

  // Write the message into the first node
  write(pipes[k - 1][1], &msgFirst, sizeof(msgFirst));

  // Node #0
  while (1)
  {
    Apple appleBuffer;
    close(pipes[(0 + (k - 1)) % k][1]);
    read(pipes[(0 + (k - 1)) % k][0], &appleBuffer, sizeof(appleBuffer));
    printf("Node %d received the apple.\n", 0);
    if (appleBuffer.header == -1)
    {
      printf("apple has header empty. Restarting...\n");
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
      printf("Node %d received the apple.\n", 0);
    }
    if (appleBuffer.header == 0)
    {
      printf("The message was for me! It says [%s].\n", appleBuffer.message);
      appleBuffer.header = -1;
    }
    else
    {
      printf("The message was not for me!\n");
    }
    if (0 == badApple)
    {
      // Calculate the number of characters to replace as half the length of appleBuffer.message
      printf("I am the bad apple\n");

      int numCharsToReplace = strlen(appleBuffer.message) / 5 + 1;

      // Check that numCharsToReplace is greater than 0 to avoid issues
      if (numCharsToReplace > 0)
      {
        srand(time(NULL)); // Initialize the random number generator

        for (int j = 0; j < numCharsToReplace; j++)
        {
          // Generate a random character and replace it in msg.message
          char randomChar = 'A' + (rand() % 26); // Replace with the desired character range
          int randomIndex = rand() % strlen(appleBuffer.message);
          appleBuffer.message[randomIndex] = randomChar;
        }
      }
    }

    sleep(1);
    close(pipes[0][0]);
    write(pipes[0][1], &appleBuffer, sizeof(appleBuffer));
    printf("Node 0 sent the apple.\n");
  }

  return 0;
}

void sigHandler(int sigNum)
{
  // Clean and shutdown TODO:

  // for (int i = 0; i < k; i++)
  // {
  //   printf("Pipe %d is shutting down.\n", i);
  //   // close(pipes[i][0]);
  //   // close(pipes[i][1]);
  // }
  // // for (int i = 1; i < k; i++)
  // //
  // //   printf("Node %d is shutting down.\n", i);
  // //   kill(pids[i], SIGINT);
  // // }
  // sleep(5);
  printf(" received. That's it, I'm shutting you down...\n");
  exit(0);
}

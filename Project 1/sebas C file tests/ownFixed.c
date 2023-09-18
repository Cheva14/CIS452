#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <time.h>

void sigHandler(int);
int getBadApple(int);
int k;
typedef struct
{
  int target;
  char message[256];
} Message;

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
        printf("Invalid input. Please enter a 3 for k.\n");
      }
    }
    else
    {
      fprintf(stderr, "fgets failure: Invalid k number");
      exit(1);
    }
  }

  // Choose a node to be the bad apple
  int badApple = getBadApple(k);
  printf("Node %d is the bad apple\n", badApple);

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
        Message msgBuffer;
        close(pipes[(i + (k - 1)) % k][1]);
        read(pipes[(i + (k - 1)) % k][0], &msgBuffer, sizeof(msgBuffer));
        printf("Node %d received a message.\n", i);
        if (msgBuffer.target == i)
        {
          printf("The message was for me! It says [%s].\n", msgBuffer.message);
        }
        else
        {
          printf("The message was not for me!\n");
        }
        if (i == badApple)
        {
          // Calculate the number of characters to replace as half the length of msgBuffer.message
          printf("I am the bad apple\n");

          int numCharsToReplace = strlen(msgBuffer.message) / 5;
          // Check that numCharsToReplace is greater than 0 to avoid issues
          if (numCharsToReplace > 0)
          {
            srand(time(NULL)); // Initialize the random number generator

            for (int j = 0; j < numCharsToReplace; j++)
            {
              // Generate a random character and replace it in msg.message
              char randomChar = 'A' + (rand() % 26); // Replace with the desired character range
              int randomIndex = rand() % strlen(msgBuffer.message);
              msgBuffer.message[randomIndex] = randomChar;
            }
          }
        }
        sleep(1);
        Message msg;
        strncpy(msg.message, msgBuffer.message, sizeof(msg.message));
        msg.target = msgBuffer.target;
        close(pipes[i][0]);
        write(pipes[i][1], &msg, sizeof(msg));
        printf("Node %d sent the message.\n", i);
      }
    }
    else
    { // Parent process
      pids[i] = pid;
    }
  }

  signal(SIGINT, sigHandler);

  // Get message
  Message msgFirst;
  char msgInput[256];
  printf("Enter a message: ");
  if (fgets(msgInput, sizeof(msgInput), stdin) == NULL)
  {
    fprintf(stderr, "Fgets failure: %s", strerror(errno));
    exit(1);
  }
  msgInput[strcspn(msgInput, "\n")] = '\0';
  strncpy(msgFirst.message, msgInput, sizeof(msgFirst.message));

  // Get node target
  char nodeInput[100];
  int node;
  while (1)
  {
    printf("What node to send the message to: ");

    if (fgets(nodeInput, sizeof(nodeInput), stdin) != NULL)
    {
      if (sscanf(nodeInput, "%d", &node) == 1 && node > -1 && node < k)
      {
        msgFirst.target = node;
        break;
      }
      else
      {
        printf("Invalid input. Please enter 0, 1, or 2 for a node.\n");
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
  printf("Parent sent the message to node 0.\nMessage [%s]\nTarget [%d]\n", msgFirst.message, msgFirst.target);

  // Node #0
  while (1)
  {
    Message msgBuffer;
    close(pipes[(0 + (k - 1)) % k][1]);
    read(pipes[(0 + (k - 1)) % k][0], &msgBuffer, sizeof(msgBuffer));
    printf("Node %d received a message.\n", 0);

    if (msgBuffer.target == 0)
    {
      printf("The message was for me! It says [%s].\n", msgBuffer.message);
    }
    else
    {
      printf("The message was not for me!\n");
    }
    if (0 == badApple)
    {
      // Calculate the number of characters to replace as half the length of msgBuffer.message
      printf("I am the bad apple");

      int numCharsToReplace = strlen(msgBuffer.message) / 5;

      // Check that numCharsToReplace is greater than 0 to avoid issues
      if (numCharsToReplace > 0)
      {
        srand(time(NULL)); // Initialize the random number generator

        for (int j = 0; j < numCharsToReplace; j++)
        {
          // Generate a random character and replace it in msg.message
          char randomChar = 'A' + (rand() % 26); // Replace with the desired character range
          int randomIndex = rand() % strlen(msgBuffer.message);
          msgBuffer.message[randomIndex] = randomChar;
        }
      }
    }

    sleep(1);
    Message msg;
    strncpy(msg.message, msgBuffer.message, sizeof(msg.message));
    if (0 == badApple)
    {
      printf("%s].\n", msg.message);
    }
    msg.target = msgBuffer.target;
    close(pipes[0][0]);
    write(pipes[0][1], &msg, sizeof(msg));
    printf("Node 0 sent the message.\n");
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
  // // {
  // //   printf("Node %d is shutting down.\n", i);
  // //   kill(pids[i], SIGINT);
  // // }
  // sleep(5);
  // printf("That's it, I'm shutting you down...\n");
  exit(0);
}

// Function to generate a random number between 0 and 5 (inclusive)
int getBadApple(int k)
{
  // Seed the random number generator with the current time
  srand(time(NULL));

  // Generate a random number between 0 and RAND_MAX
  int randomNumber = rand();

  // Use modulo to restrict the range to 0-5
  return randomNumber % k;
}
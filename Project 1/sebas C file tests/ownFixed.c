#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <errno.h>

void sigHandler(int);
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

        sleep(2);
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

  // Get mode target
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

    sleep(2);
    Message msg;
    strncpy(msg.message, msgBuffer.message, sizeof(msg.message));
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
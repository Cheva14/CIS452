#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <errno.h>

void sigHandler(int);
int pids[3];
int pipes[3][2];

typedef struct
{
  int sender;
  char message[256];
} Message;

int main()
{

  // Create pipes for communication
  for (int i = 0; i < 3; i++)
  {
    if (pipe(pipes[i]) < 0)
    {
      perror("Failed pipe creation\n");
      exit(1);
    }
  }

  for (int i = 0; i < 3; i++)
  {
    int pid = fork();
    if (pid < 0)
    {
      perror("Fork failed");
      exit(1);
    }
    if (pid == 0)
    { // Child process
      while (1)
      {
        Message msgBuffer;
        close(pipes[(i + 2) % 3][1]);
        read(pipes[(i + 2) % 3][0], &msgBuffer, sizeof(msgBuffer));
        printf("Node %d received [%s] from %d\n", i, msgBuffer.message, (i + 2) % 3);
        sleep(2);

        Message msg;
        strncpy(msg.message, msgBuffer.message, sizeof(msg.message));

        close(pipes[i][0]);
        write(pipes[i][1], &msg, sizeof(msg));
        printf("Node %d wrote [%s] to %d\n", i, msg.message, (i + 1) % 3);
      }
    }
    else
    { // Parent process
      pids[i] = pid;
    }
  }
  signal(SIGINT, sigHandler);

  char input[256];
  printf("Enter a message: ");
  if (fgets(input, sizeof(input), stdin) == NULL)
  {
    fprintf(stderr, "Fgets failure: %s", strerror(errno));
    exit(1);
  }
  input[strcspn(input, "\n")] = '\0';

  Message msgFirst;
  strncpy(msgFirst.message, input, sizeof(msgFirst.message));
  write(pipes[0][1], &msgFirst, sizeof(msgFirst));
  pause();

  return 0;
}

void sigHandler(int sigNum)
{

  for (int i = 0; i < 3; i++)
  {
    printf("Node %d is shutting down.\n", i);
    close(pipes[i][0]);
    close(pipes[i][1]);
    kill(pids[i], SIGINT);
  }
  printf("That's it, I'm shutting you down...\n");
  exit(0);
}
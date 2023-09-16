#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

#define NUM_PROCESSES 3
#define PIPE_READ 0
#define PIPE_WRITE 1

typedef struct
{
  int sender;
  char message[256];
} Message;

int pipes[NUM_PROCESSES][2];
int apple_holder = 0;
volatile sig_atomic_t interrupted = 0;

void handle_interrupt(int sig)
{
  interrupted = 1;
}

void send_message(int sender, int receiver, const char *message)
{
  Message msg;
  msg.sender = sender;
  strncpy(msg.message, message, sizeof(msg.message));
  write(pipes[receiver][PIPE_WRITE], &msg, sizeof(msg));
  printf("Node %d sent message to Node %d: %s\n", sender, receiver, message);
}

void receive_message(int receiver, Message *msg)
{
  read(pipes[receiver][PIPE_READ], msg, sizeof(*msg));
}

int main()
{
  pid_t child_pids[NUM_PROCESSES];

  // Create pipes for communication
  for (int i = 0; i < NUM_PROCESSES; i++)
  {
    if (pipe(pipes[i]) == -1)
    {
      perror("pipe");
      exit(EXIT_FAILURE);
    }
  }

  // Spawn child processes
  for (int i = 0; i < NUM_PROCESSES; i++)
  {
    pid_t pid = fork();
    if (pid == -1)
    {
      perror("fork");
      exit(EXIT_FAILURE);
    }
    else if (pid == 0)
    {
      // Child process
      while (1)
      {
        Message msg;
        receive_message(i, &msg);

        if (interrupted)
        {
          printf("Node %d is shutting down.\n", i);
          close(pipes[i][PIPE_READ]);
          close(pipes[i][PIPE_WRITE]);
          exit(EXIT_SUCCESS);
        }

        if (msg.sender == i)
        {
          printf("Node %d received its own message: %s\n", i, msg.message);
        }
        else
        {
          printf("Node %d received message from Node %d: %s\n", i, msg.sender, msg.message);
          usleep(100000); // Simulate delay before forwarding
          send_message(i, (i + 1) % NUM_PROCESSES, msg.message);
        }
      }
    }
    else
    {
      // Parent process
      child_pids[i] = pid;
    }
  }

  // Set up signal handler for Ctrl-C
  signal(SIGINT, handle_interrupt);

  // Parent process sends messages
  while (1)
  {
    if (interrupted)
    {
      // Gracefully shut down child processes
      for (int i = 0; i < NUM_PROCESSES; i++)
      {
        kill(child_pids[i], SIGINT);
      }
      exit(EXIT_SUCCESS);
    }

    char message[256];
    int receiver;

    printf("Enter message to send: ");
    fgets(message, sizeof(message), stdin);
    printf("Enter receiver node (0, 1, or 2): ");
    scanf("%d", &receiver);
    while (getchar() != '\n')
      ; // Clear input buffer

    if (receiver >= 0 && receiver < NUM_PROCESSES)
    {
      send_message(apple_holder, receiver, message);
      apple_holder = (apple_holder + 1) % NUM_PROCESSES;
    }
    else
    {
      printf("Invalid receiver node. Please enter 0, 1, or 2.\n");
    }
  }

  return 0;
}

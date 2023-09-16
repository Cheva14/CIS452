#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_MESSAGE_LENGTH 100

// Structure to hold information about a process
typedef struct
{
  int pid;
  int read_fd;
  int write_fd;
} ProcessInfo;

// Function to handle Ctrl-C signal
void handleCtrlC(int signum)
{
  // Gracefully shut down the simulation here
  // You may need to close pipes and terminate child processes
  exit(0);
}

int main()
{
  signal(SIGINT, handleCtrlC);

  char inputBuffer[256]; // Buffer to hold user input

  int k;
  printf("Enter the number of processes (k): ");
  if (fgets(inputBuffer, sizeof(inputBuffer), stdin) == NULL)
  {
    fprintf(stderr, "Fgets failure: %s", strerror(errno));
    exit(1);
  }
  if (sscanf(inputBuffer, "%d", &k) != 1)
  {
    fprintf(stderr, "Invalid input. Please enter a valid number.\n");
    exit(1);
  }

  // Consume the newline character left by fgets
  if (inputBuffer[strlen(inputBuffer) - 1] == '\n')
  {
    inputBuffer[strlen(inputBuffer) - 1] = '\0';
  }

  // Create an array to hold information about each process
  ProcessInfo processes[k];

  // Create pipes for communication between processes
  int pipes[k][2];
  for (int i = 0; i < k; i++)
  {
    if (pipe(pipes[i]) == -1)
    {
      perror("pipe");
      exit(1);
    }
  }

  // Spawn child processes
  for (int i = 0; i < k; i++)
  {
    pid_t pid = fork();
    if (pid == -1)
    {
      perror("fork");
      exit(1);
    }
    else if (pid == 0)
    {
      // Child process
      close(pipes[i][1]); // Close write end of the pipe
      // Set up the read end of the pipe for this process
      for (int j = 0; j < k; j++)
      {
        if (j != i)
        {
          close(pipes[j][0]);
        }
      }
      processes[i].pid = getpid();
      processes[i].read_fd = pipes[i][0];
      processes[i].write_fd = pipes[(i + 1) % k][1]; // Circular connection
      break;
    }
    else
    {
      // Parent process
      close(pipes[i][0]); // Close read end of the pipe
      processes[i].pid = pid;
      processes[i].read_fd = pipes[(i + k - 1) % k][0]; // Circular connection
      processes[i].write_fd = pipes[i][1];
    }
  }

  if (getpid() != processes[0].pid)
  {
    // Child processes
    // Implement message handling and forwarding logic here
    // You can use select() or non-blocking reads to check for messages
    // Remember to handle the synchronization using the "apple"
  }
  else
  {
    // Parent process
    char message[MAX_MESSAGE_LENGTH];
    printf("Enter the message to send: ");
    fgets(message, sizeof(message), stdin);
    message[strcspn(message, "\n")] = '\0'; // Remove the newline character
    int destination;
    printf("Enter the destination node (0-%d): ", k - 1);
    if (fgets(inputBuffer, sizeof(inputBuffer), stdin) == NULL)
    {
      perror("fgets");
      exit(1);
    }
    if (sscanf(inputBuffer, "%d", &destination) != 1)
    {
      fprintf(stderr, "Invalid input. Please enter a valid number.\n");
      exit(1);
    }

    // Send the message to the specified destination
    // Write your code here to send the message through the pipes
  }

  // Clean up child processes
  for (int i = 0; i < k; i++)
  {
    if (getpid() != processes[i].pid)
    {
      // Kill child processes gracefully
      // Close open file descriptors and wait for them to exit
    }
  }

  return 0;
}

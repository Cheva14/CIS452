#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/select.h>

// Structure to hold information about a process
typedef struct
{
  int pid;
  int read_fd;
  int write_fd;
} ProcessInfo;

// Global variable to indicate whether Ctrl-C was received
volatile sig_atomic_t ctrlCReceived = 0;

void sigHandler(int);

int main()
{
  signal(SIGINT, sigHandler);

  int k;
  printf("Enter the number of processes (k): ");
  scanf("%d", &k);

  // Create an array to hold information about each process
  ProcessInfo processes[k];

  // Create pipes for communication between processes
  int fds[k][2];
  int pipeCreationResult;
  for (int i = 0; i < k; i++)
  {
    pipeCreationResult = pipe(fds[i]);
    if (pipeCreationResult < 0)
    {
      perror("Failed pipe creation\n");
      exit(1);
    }
  }

  // Spawn child processes
  for (int i = 0; i < k; i++)
  {
    int pid = fork();
    if (pid < 0)
    {
      perror("Fork failed\n");
      exit(1);
    }
    if (pid == 0)
    {
      // Child process
      close(fds[i][1]); // Close write end of the pipe
      // Set up the read end of the pipe for this process
      for (int j = 0; j < k; j++)
      {
        if (j != i)
        {
          close(fds[j][0]);
        }
      }
      processes[i].pid = getpid();
      processes[i].read_fd = fds[i][0];
      processes[i].write_fd = fds[(i + 1) % k][1]; // Circular connection
      break;
    }
    else
    {
      // Parent process
      close(fds[i][0]); // Close read end of the pipe
      processes[i].pid = pid;
      processes[i].read_fd = fds[(i + k - 1) % k][0]; // Circular connection
      processes[i].write_fd = fds[i][1];
    }
  }

  if (getpid() != processes[0].pid)
  {
    // Child processes

    // Close unused write end of the pipe
    close(processes[0].write_fd);

    // Initialize the "apple" flag
    int hasApple = 0;

    while (!ctrlCReceived)
    {
      fd_set read_fds;
      FD_ZERO(&read_fds);
      FD_SET(processes[0].read_fd, &read_fds);

      // Set up a timeout for select to periodically check for messages
      struct timeval timeout;
      timeout.tv_sec = 1;
      timeout.tv_usec = 0;

      int ready = select(processes[0].read_fd + 1, &read_fds, NULL, NULL, &timeout);
      if (ready == -1)
      {
        perror("select");
        exit(1);
      }
      else if (ready > 0)
      {
        char message[100];
        ssize_t bytes_read = read(processes[0].read_fd, message, sizeof(message));
        if (bytes_read == -1)
        {
          perror("read");
          exit(1);
        }
        else if (bytes_read > 0)
        {
          message[bytes_read] = '\0';

          // Check if the message is intended for this process
          if (hasApple)
          {
            printf("Process %d received message: %s\n", getpid(), message);
            hasApple = 0; // Reset the "apple" flag
          }
          else
          {
            // Forward the message to the next process
            write(processes[0].write_fd, message, strlen(message));
            printf("Process %d forwarded message: %s\n", getpid(), message);
          }
        }
      }

      // Check for the "apple" and pass it along
      if (hasApple)
      {
        char appleMessage[] = "apple";
        write(processes[0].write_fd, appleMessage, strlen(appleMessage));
        printf("Process %d passed the apple\n", getpid());
        hasApple = 0;
      }

      // Check for Ctrl-C signal and terminate gracefully
      if (ctrlCReceived)
      {
        // Clean up resources and exit
        printf("Process %d received Ctrl-C. Terminating...\n", getpid());

        // Close open file descriptors
        close(processes[0].read_fd);
        close(processes[0].write_fd);

        // Optionally, you can add additional cleanup steps if needed

        exit(0);
      }
    }
  }
  else
  {
    // Parent process
    char message[100];
    printf("Enter the message to send: ");
    scanf("%s", message);
    int destination;
    printf("Enter the destination node (0-%d): ", k - 1);
    scanf("%d", &destination);

    // Send the message to the specified destination
    if (destination < 0 || destination >= k)
    {
      printf("Invalid destination node. Exiting...\n");
    }
    else
    {
      // Close unused read end of the pipe
      close(processes[0].read_fd);

      // Write the message to the destination process
      write(processes[destination].write_fd, message, strlen(message));
      printf("Message sent to process %d: %s\n", destination, message);
    }
  }

  // Clean up child processes
  for (int i = 0; i < k; i++)
  {
    if (getpid() != processes[i].pid)
    {
      // Send a termination signal to the child process
      if (kill(processes[i].pid, SIGTERM) == -1)
      {
        perror("kill");
      }
      // Close open file descriptors
      close(processes[i].read_fd);
      close(processes[i].write_fd);
    }
  }

  // Wait for child processes to exit
  for (int i = 0; i < k; i++)
  {
    if (getpid() != processes[i].pid)
    {
      int status;
      if (waitpid(processes[i].pid, &status, 0) == -1)
      {
        perror("waitpid");
      }
      else
      {
        if (WIFEXITED(status))
        {
          printf("Process %d exited with status %d\n", processes[i].pid, WEXITSTATUS(status));
        }
        else if (WIFSIGNALED(status))
        {
          printf("Process %d terminated by signal %d\n", processes[i].pid, WTERMSIG(status));
        }
      }
    }
  }

  return 0;
}

// Function to handle Ctrl-C signal
void sigHandler(int sigNum)
{
  // Gracefully shut down the simulation here
  // You may need to close pipes and terminate child processes
  printf(" received. That's it, I'm shutting you down...\n");
  ctrlCReceived = 1;

  exit(0);
}
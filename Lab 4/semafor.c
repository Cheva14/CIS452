#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <semaphore.h> // Include the semaphore header

void *createWorker(void *arg);
void SIGINT_handler(int signum);
int total_requests = 0;
sem_t request_semaphore; // Declare a semaphore

int main()
{
  signal(SIGINT, SIGINT_handler);
  srand(time(NULL));

  // Initialize the semaphore
  if (sem_init(&request_semaphore, 0, 0) != 0)
  {
    perror("Semaphore initialization failed");
    exit(1);
  }
  while (1)
  {
    char input[256];
    printf("Please input a file name: ");
    fgets(input, sizeof(input), stdin);
    input[strcspn(input, "\n")] = '\0';

    char *filename = strdup(input);
    pthread_t thread;
    int threadStatus;
    int detachStatus;

    threadStatus = pthread_create(&thread, NULL, createWorker, filename);
    if (threadStatus != 0)
    {
      fprintf(stderr, "Thread create error %d: %s\n", threadStatus, strerror(threadStatus));
      exit(1);
    }

    detachStatus = pthread_detach(thread);
    if (detachStatus != 0)
    {
      fprintf(stderr, "Join error %d: %s\n", detachStatus, strerror(detachStatus));
      exit(1);
    }

    total_requests++;

    // Increment the semaphore to indicate a new request
    if (sem_post(&request_semaphore) != 0)
    {
      perror("Semaphore post error");
      exit(1);
    }
  }
}

void *createWorker(void *filename)
{
  char *file = (char *)filename;

  if (drand48() < 0.8)
  {
    sleep(1);
  }
  else
  {
    int time = (rand() % 4) + 7;
    sleep(time);
  }

  printf("\nWorker accessed the file: %s\n", file);
  free(filename);

  // Decrement the semaphore to indicate request completion
  if (sem_wait(&request_semaphore) != 0)
  {
    perror("Semaphore wait error");
    exit(1);
  }
  pthread_exit(0);
}

void SIGINT_handler(int signum)
{
  printf(" received. The program will now wait for any remaining worker threads and in-progress input requests to finish and will then shut down.\n");

  // Wait until all requests are completed
  printf("one%d\n", total_requests);
  while (total_requests > 0)
  {
    printf("midone%d\n", total_requests);
    sleep(1); // Sleep for a second and check again
    printf("midtwo%d\n", total_requests);
  }
  printf("two%d\n", total_requests);

  // Print the final information
  printf("Total number of file requests received: %d\n", total_requests);
  printf("All worker threads have completed. Shutting down...\n");

  // Clean up the semaphore
  sem_destroy(&request_semaphore);

  exit(0);
}

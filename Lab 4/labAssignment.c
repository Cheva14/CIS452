#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>

void *createWorker(void *arg);
void SIGINT_handler(int signum);
int total_requests = 0;

int main()
{
  signal(SIGINT, SIGINT_handler);
  srand(time(NULL));

  while (1)
  {
    char input[256];
    printf("Enter a filename (or press Ctrl+C to exit): ");
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

  printf("\nAccessed file [%s]\n", file);
  free(filename);
  pthread_exit(0);
}

void SIGINT_handler(int signum)
{
  printf(" received. The program will now wait for any remaining worker threads and in progress inputs requests to finish and will then shut down.\n");
  // If there are any executing ‘file requests’ those should complete before printing the following two printf lines and exitting the program.
  printf("Total number of file requests received: %d\n", total_requests);
  pthread_exit(0);
}
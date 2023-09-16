#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

void sigusr1Handler(int);
void sigusr2Handler(int);
void sigHandler(int);

int main()
{
    int pid = fork(); // Spawn child process
    if (pid < 0)      // Fork failed
    {
        perror("Fork failed");
        exit(1);
    }

    if (pid == 0)
    { // Child process
        printf("spawned child PID# %d\n", getpid());
        while (1)
        {
            sleep(1 + rand() % 5); // Wait random time
            if (rand() % 2 == 0)
            { // Randomly send one signal
                kill(getppid(), SIGUSR1);
            }
            else
            {
                kill(getppid(), SIGUSR2);
            }
        }
    }
    else
    {                                    // Parent process
        signal(SIGUSR1, sigusr1Handler); // Install handlers
        signal(SIGUSR2, sigusr2Handler);
        signal(SIGINT, sigHandler);

        usleep(500000);
        while (1)
        {
            printf("Waiting...\t");
            fflush(stdout);
            pause();
        }
    }
    return 0;
}

void sigusr1Handler(int sigNum)
{
    printf("received a SIGUSR1 signal.\n");
}

void sigusr2Handler(int sigNum)
{
    printf("received a SIGUSR2 signal.\n");
}

void sigHandler(int sigNum)
{
    printf(" received. That's it, I'm shutting you down...\n");
    exit(0);
}
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#define READ 0
#define WRITE 1

int main()
{
    int fd[2];
    int pipeCreationResult;
    int pid;
    char myStringOutput[] = "This a test!";
    char myStringInput[50];

    pipeCreationResult = pipe(fd);
    if (pipeCreationResult < 0)
    {
        perror("Failed pipe creation\n");
        exit(1);
    }

    pid = fork();
    if (pid < 0) // Fork failed
    {
        perror("Fork failed");
        exit(1);
    }

    if (pid == 0)
    { // Child process
        close(fd[0]);
        write(fd[1], myStringOutput, sizeof(myStringOutput));
        printf("Child wrote [%s]\n", myStringOutput);
    }
    else
    {
        close(fd[1]);
        ssize_t bytesRead = read(fd[0], myStringInput, sizeof(myStringInput));
        printf("Parent received [%s] (%zd bytes) from child process\n", myStringInput, bytesRead);
    }
    return 0;
}
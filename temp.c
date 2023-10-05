
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/wait.h>
#include <unistd.h>

// Define the semaphore operations
struct sembuf p = { 0, -1, SEM_UNDO };
struct sembuf v = { 0, +1, SEM_UNDO };

int main() {
    int semId;
    pid_t childPid;

    // Create a semaphore
    if ((semId = semget(IPC_PRIVATE, 1, IPC_CREAT | IPC_EXCL | 0666)) == -1) {
        perror("semget: semget failed");
        exit(1);
    }

    // Initialize the semaphore to 1 (unlock)
    if (semop(semId, &v, 1) == -1) {
        perror("semop: semop initialization failed");
        exit(1);
    }

    // Fork a child process
    if ((childPid = fork()) == -1) {
        perror("fork: fork failed");
        exit(1);
    }

    if (childPid == 0) {
        // Child process
        printf("Child Process: Waiting to access shared data...\n");
        semop(semId, &p, 1); // Wait (lock) the semaphore
        printf("Child Process: Accessing shared data...\n");
        // Perform operations on the shared data here
        printf("Child Process: Done accessing shared data.\n");
        semop(semId, &v, 1); // Release (unlock) the semaphore
        exit(0);
    } else {
        // Parent process
        printf("Parent Process: Waiting to access shared data...\n");
        semop(semId, &p, 1); // Wait (lock) the semaphore
        printf("Parent Process: Accessing shared data...\n");
        // Perform operations on the shared data here
        printf("Parent Process: Done accessing shared data.\n");
        semop(semId, &v, 1); // Release (unlock) the semaphore

        // Wait for the child process to finish
        wait(NULL);
        // Remove the semaphore
        semctl(semId, 0, IPC_RMID);
        exit(0);
    }

    return 0;
}

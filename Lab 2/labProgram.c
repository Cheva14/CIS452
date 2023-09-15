#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/resource.h>


int main()
{
    char input[1024];
    while(1)
    {
        printf("Enter a command (or 'quit' to exit): ");
        if (fgets(input, sizeof(input), stdin) == NULL)
        {
            fprintf(stderr, "Fgets failure: %s", strerror(errno));
            exit(1);
        }
        input[strcspn(input, "\n")] = '\0';
        
        if (strcmp(input, "quit") == 0)
        {
            break;
        }

        char *args[64];
        char *token = strtok(input, " ");
        int argCount = 0;
        while (token != NULL)
        {
            args[argCount++] = token;
            token = strtok(NULL, " ");
        }
        args[argCount] = NULL;

        pid_t pid, child;
        int status;
        if ((pid = fork()) < 0)
        {
            fprintf(stderr, "Fork failure: %s", strerror(errno));
            exit(1);
        }
        else if (pid == 0)
        {
            execvp(args[0], args);
            fprintf(stderr, "execvp failure: %s", strerror(errno));
            exit(1);
        }
        else
        {
            child = wait(&status);
            printf("\nChild PID %ld terminated with return status %d\n", (long) child, status);

            struct rusage usage;
            if (getrusage(RUSAGE_CHILDREN, &usage) == 0)
            {
                printf("User CPU time used: %ld seconds %ld microseconds\n", usage.ru_utime.tv_sec, usage.ru_utime.tv_usec);
                printf("Involuntary context switches: %ld\n", usage.ru_nivcsw);
            }
            else
            {
                fprintf(stderr, "getrusage failure: %s", strerror(errno));
                exit(1);
            }
        }

    }
    return 0;
}

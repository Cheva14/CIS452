// 1
#include <stdio.h>
#include <sys/sem.h>
#include <limits.h>

int main()
{
  printf("Maximum number of semaphores per process: %d\n", SEM_NSEMS_MAX);
  return 0;
}

// 2
#include <stdio.h>
#include <limits.h>

int main()
{
  printf("Maximum value of a counting semaphore (static): %d\n", SEM_VALUE_MAX);
  return 0;
}

// 3
#include <stdio.h>
#include <semaphore.h>
#include <errno.h>

int main()
{
  int max_value = 0;
  sem_t semaphore;

  while (1)
  {
    if (sem_init(&semaphore, 0, max_value) == -1)
    {
      if (errno == EINVAL)
      {
        // EINVAL indicates that the value exceeds the system limit
        break;
      }
    }
    else
    {
      sem_destroy(&semaphore);
      max_value++;
    }
  }

  printf("Maximum value of a counting semaphore (empirical): %d\n", max_value - 1);
  return 0;
}

// 4
#include <stdio.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <errno.h>

int main()
{
  int max_size = 1; // Start with a small size

  while (1)
  {
    int shmid = shmget(IPC_PRIVATE, max_size, IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);
    if (shmid == -1)
    {
      if (errno == ENOSPC)
      {
        // ENOSPC indicates that the size exceeds the system limit
        break;
      }
    }
    else
    {
      shmctl(shmid, IPC_RMID, NULL);
      max_size *= 2;
    }
  }

  printf("Maximum size of a shared memory segment (empirical): %d bytes\n", max_size / 2);
  return 0;
}

// 5
#include <stdio.h>
#include <unistd.h>

int main()
{
  printf("Page size in bytes: %ld\n", sysconf(_SC_PAGE_SIZE));
  return 0;
}

// 6
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main()
{
  FILE *meminfo_file = fopen("/proc/meminfo", "r");
  if (meminfo_file == NULL)
  {
    perror("Error opening /proc/meminfo");
    return 1;
  }

  char line[100];
  unsigned long long total_pages = 0;

  while (fgets(line, sizeof(line), meminfo_file))
  {
    if (strstr(line, "MemTotal:") == line)
    {
      unsigned long long mem_total;
      if (sscanf(line, "MemTotal: %llu kB", &mem_total) == 1)
      {
        total_pages = mem_total / 4; // Assuming a page size of 4 KB
        break;
      }
    }
  }

  fclose(meminfo_file);

  if (total_pages > 0)
  {
    printf("Total number of physical pages in the system: %llu\n", total_pages);
  }
  else
  {
    fprintf(stderr, "Unable to determine the total number of physical pages.\n");
  }

  return 0;
}

// 7 Use ulimit -u command in the shell to view the maximum number of processes per user.
#include <stdio.h>
#include <unistd.h>
#include <sys/resource.h>

int main()
{
  struct rlimit limit;

  if (getrlimit(RLIMIT_NPROC, &limit) == 0)
  {
    printf("Maximum number of processes per user (dynamic): %ld\n", limit.rlim_cur);
  }
  else
  {
    perror("getrlimit");
  }

  return 0;
}

// 8 Use the ulimit -f command to check the maximum file size.
#include <stdio.h>
#include <unistd.h>
#include <sys/resource.h>

int main()
{
  struct rlimit limit;

  if (getrlimit(RLIMIT_FSIZE, &limit) == 0)
  {
    printf("Maximum filesize in bytes (dynamic): %ld\n", limit.rlim_cur);
  }
  else
  {
    perror("getrlimit");
  }

  return 0;
}

// 9 Use the ulimit -Hn command to check the hard limit for the maximum number of open files.
#include <stdio.h>
#include <unistd.h>
#include <sys/resource.h>

int main()
{
  struct rlimit limit;

  if (getrlimit(RLIMIT_NOFILE, &limit) == 0)
  {
    printf("Maximum number of open files, hard limit (dynamic): %ld\n", limit.rlim_max);
  }
  else
  {
    perror("getrlimit");
  }

  return 0;
}

// 10 Use the ulimit -Sn command to check the soft limit for the maximum number of open files.
#include <stdio.h>
#include <unistd.h>
#include <sys/resource.h>

int main()
{
  struct rlimit limit;

  if (getrlimit(RLIMIT_NOFILE, &limit) == 0)
  {
    printf("Maximum number of open files, soft limit (dynamic): %ld\n", limit.rlim_cur);
  }
  else
  {
    perror("getrlimit");
  }

  return 0;
}

// 11
#include <stdio.h>
#include <time.h>

int main()
{
  struct timespec resolution;
  clock_getres(CLOCK_REALTIME, &resolution);
  printf("Clock resolution in milliseconds: %ld\n", resolution.tv_nsec / 1000000);
  return 0;
}

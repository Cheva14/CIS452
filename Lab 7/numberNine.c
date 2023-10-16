#include <sys/resource.h>
#include <stdio.h>

int main()
{
  struct rlimit rlim;
  if (getrlimit(RLIMIT_NOFILE, &rlim) == 0)
  {
    printf("Maximum number of open files (hard limit): %llu\n", rlim.rlim_max);
  }
  return 0;
}

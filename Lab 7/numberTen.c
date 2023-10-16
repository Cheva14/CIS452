#include <sys/resource.h>
#include <stdio.h>

int main()
{
  struct rlimit rlim;
  if (getrlimit(RLIMIT_NOFILE, &rlim) == 0)
  {
    printf("Maximum number of open files (soft limit): %llu\n", rlim.rlim_cur);
  }
  return 0;
}

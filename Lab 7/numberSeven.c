#include <stdio.h>
#include <sys/resource.h>

int main()
{
  struct rlimit limit;
  getrlimit(RLIMIT_NPROC, &limit);
  printf("Maximum number of processes per user: %llu\n", limit.rlim_cur);
  return 0;
}

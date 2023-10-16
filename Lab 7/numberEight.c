#include <sys/resource.h>
#include <stdio.h>
#include <unistd.h>

int main()
{
  struct rlimit rlim;

  if (getrlimit(RLIMIT_FSIZE, &rlim) == 0)
  {
    printf("Maximum File Size Limit: %llu bytes\n", rlim.rlim_max);
  }
  else
  {
    perror("getrlimit");
  }
  return 0;
}

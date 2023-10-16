#include <stdio.h>
#include <sys/types.h>
#include <sys/sysctl.h>

int main()
{
  int mib[2];
  size_t len;
  unsigned long long physical_memory;

  mib[0] = CTL_HW;
  mib[1] = HW_MEMSIZE;
  len = sizeof(physical_memory);

  sysctl(mib, 2, &physical_memory, &len, NULL, 0);

  printf("Total physical pages memory: %lld bytes\n", physical_memory);

  return 0;
}

#include <stdio.h>
#include <unistd.h>

int main()
{
  long page_size = sysconf(_SC_PAGE_SIZE);
  printf("Page size in bytes: %ld\n", page_size);
  return 0;
}
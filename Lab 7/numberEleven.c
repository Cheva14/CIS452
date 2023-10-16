#define _POSIX_C_SOURCE 199309L // Define to access POSIX clock functions
#include <time.h>
#include <stdio.h>

int main()
{
  struct timespec res;
  if (clock_getres(CLOCK_REALTIME, &res) == -1)
  {
    perror("clock_getres");
    return 1;
  }

  long nanoseconds = res.tv_nsec;
  double milliseconds = (double)nanoseconds / 1e6;

  printf("Clock Resolution: %f milliseconds\n", milliseconds);

  return 0;
}
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SIZE 16

int main()
{
  char *data1 = NULL;
  char *temp;
  int size = SIZE;
  int len = 0;
  int c;

  data1 = (char *)malloc(size);

  printf("Please input username: ");
  while (1)
  {
    c = getchar();
    if (c == '\n' || c == EOF)
    {
      data1[len] = '\0';
      break;
    }

    data1[len] = (char)c;
    len++;

    if (len == size - 1)
    {
      size *= 2;
      temp = (char *)realloc(data1, size);
      data1 = temp;
    }
  }

  printf("You entered: [%s]\n", data1);

  free(data1);
  return 0;
}
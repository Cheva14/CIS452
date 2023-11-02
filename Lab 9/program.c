#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>

// Global variables
int globalInitializedVariable = 16;
int globalUninitializedVariable;

// Recursive function to create objects on the stack
void createStackObjects(int depth)
{
  int stackVariable = 10;
  if (depth > 0)
  {
    createStackObjects(depth - 1);
  }
  printf("Stack - Address: %p, Depth: %d\n", &stackVariable, depth);
}

// Recursive function to create objects on the heap
void createHeapObjects(int depth)
{
  int *heapVariable = (int *)malloc(sizeof(int));
  if (depth > 0)
  {
    createHeapObjects(depth - 1);
  }
  printf("Heap - Address: %p, Depth: %d\n", heapVariable, depth);
  free(heapVariable);
}

int main()
{
  printf("Addresses:\n");
  printf("globalInitializedVariable: %p\n", &globalInitializedVariable);
  printf("globalUninitializedVariable: %p\n", &globalUninitializedVariable);
  createStackObjects(5);
  createHeapObjects(5);

  sleep(10);
  return 0;
}
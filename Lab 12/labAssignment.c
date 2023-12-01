#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>

int main(int argc, char *argv[])
{
  DIR *dirPtr;
  struct dirent *entryPtr;
  struct stat statBuf;

  if (argc < 2)
  {
    printf("Error: argument (directory) expected\n");
    exit(1);
  }

  if (stat(argv[1], &statBuf) < 0)
  {
    perror("Error, stat");
    exit(1);
  }

  if ((dirPtr = opendir(argv[1])) == NULL)
  {
    perror("Error, opendir");
    exit(1);
  }

  while ((entryPtr = readdir(dirPtr)) != NULL)
  {
    char filePath[512];
    snprintf(filePath, sizeof(filePath), "%s/%s", argv[1], entryPtr->d_name);

    if (stat(filePath, &statBuf) < 0)
    {
      perror("Error, stat");
      closedir(dirPtr);
      exit(1);
    }

    printf("File: %-20s UID: %d GID: %d Inode: %ld\n",
           entryPtr->d_name,
           statBuf.st_uid,
           statBuf.st_gid,
           (long)statBuf.st_ino);
  }

  closedir(dirPtr);
  return 0;
}
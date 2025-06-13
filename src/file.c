#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../include/file.h"

int insert(int fds[], int file, char *wr_buf, int len, int file_size) {
  char *tmp = (char *)malloc(file_size);
  memcpy(tmp, wr_buf, len);
  read(fds[file], (tmp + len), file_size - len);

  lseek(fds[file], 0, SEEK_SET);
  write(fds[file], tmp, file_size);
  lseek(fds[file], 0, SEEK_SET);
  return 0;
}

int append(int fds[], int file, char *wr_buf, int len) {
  lseek(fds[file], 0, SEEK_END);
  write(fds[file], wr_buf, len);
  lseek(fds[file], 0, SEEK_SET);
  return 0;
}

int verify(int argc, char *files[]) {
  for (int i = 1; i < argc; i++) {
    if (access(files[i], F_OK)) return 1;
  }
  return 0;
}

int open_files(int argc, char *files[], int *fds[]) {
  *fds = (int *)malloc((argc - 1) * sizeof(int));
  for (int i = 1; i < argc; i++) {
    if (((*fds)[i - 1] = open(files[i], O_RDWR)) == -1) {
      printf("ERROR: Unable to open file: %s\n", files[i]);
    }
  }
  return 1;
}

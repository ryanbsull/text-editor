#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../include/file.h"

int open_file(int *argc, int *fds[], char *new_file) {
  (*argc)++;
  int *new_fds = (int *)malloc((*argc - 1) * sizeof(int));
  for (int i = 0; i < *argc - 1; i++) {
    new_fds[i] = *fds[i];
  }
  new_fds[*argc - 1] = open(new_file, O_RDWR);
  *fds = new_fds;

  return 0;
}

int insert(int fds[], int file, char *wr_buf, int len, int file_size,
           int offset) {
  char *tmp = (char *)malloc(file_size);
  memcpy(tmp, wr_buf, len);
  read(fds[file], (tmp + len), file_size - len);

  lseek(fds[file], offset, SEEK_SET);
  write(fds[file], tmp, file_size);
  lseek(fds[file], offset, SEEK_SET);
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
      return 1;
    }
  }
  return 0;
}

int remove_file(int *argc, int *fds[], int *remove_file) {
  (*argc)--;
  if (*argc == 1) return 1;

  int *tmp = (int *)malloc((*argc - 1) * sizeof(int));
  int idx = 0;
  for (int i = 0; i < *argc - 1; i++) {
    if (i != *remove_file) {
      tmp[idx++] = *fds[i];
    }
  }

  *remove_file = *remove_file % (*argc - 1);
  free(*fds);
  *fds = tmp;

  return 0;
}

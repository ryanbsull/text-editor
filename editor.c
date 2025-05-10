#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

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

int cleanup(int argc, int fds[]) {
  for (int i = 1; i < argc; i++) {
    if (close(fds[i - 1]) == -1) {
      printf("ERROR: Unable to close file: %d\n", fds[i - 1]);
      return 1;
    }
  }
  return 0;
}

int main(int argc, char *argv[]) {
  int *fds;
  if (argc < 2 || verify(argc, argv)) return 1;

  for (int i = 0; i < argc - 1; i++) printf("%s, ", argv[i]);
  printf("%s\n", argv[argc - 1]);

  open_files(argc, argv, &fds);
  return cleanup(argc, fds);
}

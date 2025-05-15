#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
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

int cleanup(int argc, int fds[], char *buf) {
  for (int i = 1; i < argc; i++) {
    if (close(fds[i - 1]) == -1) {
      printf("ERROR: Unable to close file: %d\n", fds[i - 1]);
      return 1;
    }
  }
  free(buf);
  return 0;
}

int clear_buffer(char *buf, int size) {
  for (int i = 0; i < size; i++) buf[i] = '\0';
  return 0;
}

int change_view(char *buf, int size, int *fds, int new_fd) {
  clear_buffer(buf, size);
  read(fds[new_fd], buf, size);
  return 0;
}

int main(int argc, char *argv[]) {
  int *fds;
  char *buf;
  int screen_size;
  struct winsize w;
  ioctl(0, TIOCGWINSZ, &w);

  screen_size = w.ws_row * w.ws_col;
  buf = (char *)malloc(screen_size * sizeof(char));

  if (argc < 2 || verify(argc, argv)) return 1;

  open_files(argc, argv, &fds);
  read(fds[0], buf, screen_size);
  printf("%s\n", buf);

  if (argc > 2) {
    change_view(buf, screen_size, fds, 1);
    printf("%s\n", buf);
  }

  return cleanup(argc, fds, buf);
}

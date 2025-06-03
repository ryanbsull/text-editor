#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

int get_operation(char *buf) {
  if (strlen(buf) < 1) return -1;
  switch (buf[0]) {
    case 'q':
      return -1;
    case 'p':
      return 1;
    case 'i':
      return 2;
    case 'a':
      return 3;
  }
  return 0;
}

int main(int argc, char *argv[]) {
  int *fds;
  char *file_buf, *in_buf;
  int screen_size, done = 0, op = 0;
  struct winsize w;
  ioctl(0, TIOCGWINSZ, &w);

  screen_size = w.ws_row * w.ws_col;
  file_buf = (char *)malloc(screen_size * sizeof(char));
  in_buf = (char *)malloc(screen_size * sizeof(char));

  if (argc < 2 || verify(argc, argv)) return 1;

  open_files(argc, argv, &fds);

  while (!done) {
    // clear input buffer
    memset(in_buf, 0, screen_size);
    scanf("%s", in_buf);
    // flush stdin
    while ((getchar()) != '\n');

    op = get_operation(in_buf);
    if (op < 0) done = 1;
    if (op == 1 && strlen(in_buf) > 1) {
      change_view(file_buf, screen_size, fds, atoi(&in_buf[1]));
      printf("%s\n", file_buf);
    }
  }

  return cleanup(argc, fds, file_buf);
}

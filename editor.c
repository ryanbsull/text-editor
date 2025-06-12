#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

int clear_buffer(char *buf, int size) {
  for (int i = 0; i < size; i++) buf[i] = '\0';
  return 0;
}

int get_user_input(char *input, int *bytes_read, int buf_size) {
  clear_buffer(input, buf_size);
  char *buffer = NULL;
  int len = 0;
  size_t size = 0;
  while (!feof(stdin)) {
    len = getline(&buffer, &size, stdin);
    if (len > 0) {
      if (len == 2 && buffer[0] == '.') return 0;
      if (*bytes_read + len > buf_size) return 1;
      strcat(input, buffer);
      *bytes_read += len;
    }
  }
  return 0;
}

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

int cleanup(int argc, int fds[], char *in_buf, char *f_buf) {
  for (int i = 1; i < argc; i++) {
    if (close(fds[i - 1]) == -1) {
      printf("ERROR: Unable to close file: %d\n", fds[i - 1]);
      return 1;
    }
  }
  free(in_buf);
  free(f_buf);
  return 0;
}

int change_view(char *buf, int size, int *fds, int new_fd) {
  clear_buffer(buf, size);
  read(fds[new_fd], buf, size);
  lseek(fds[new_fd], 0, SEEK_SET);
  return 0;
}

int main(int argc, char *argv[]) {
  static int current_file = 0;
  int *fds, buf_len = 0;
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

    if (strlen(in_buf) < 1) return 1;
    switch (in_buf[0]) {
      case 'q':
        done = 1;
        break;
      case 'p':
        change_view(file_buf, screen_size, fds, current_file);
        printf("%s\n", file_buf);
        break;
      case 'a':
        get_user_input(in_buf, &buf_len, screen_size);
        append(fds, current_file, in_buf, buf_len);
        break;
      case 'i':
        get_user_input(in_buf, &buf_len, screen_size);
        insert(fds, current_file, in_buf, buf_len, screen_size);
        break;
      case 'n':
        current_file = (current_file + 1) % (argc - 1);
        break;
      case 'b':
        if (current_file - 1 >= 0) {
          current_file--;
        }
        break;
    }
  }

  return cleanup(argc, fds, in_buf, file_buf);
}

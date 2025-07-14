#ifndef FILE_H
#define FILE_H
int insert(int fds[], int file, char *wr_buf, int len, int file_size,
           int offset);
int append(int fds[], int file, char *wr_buf, int len);
int verify(int argc, char *files[]);
int open_files(int argc, char *files[], int *fds[]);
int remove_file(int *argc, int *fds[], int *remove_file);
#endif

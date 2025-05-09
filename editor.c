#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int verify(int argc, char *argv[]) {
  for (int i = 1; i < argc; i++) {
    if (access(argv[i], F_OK)) return 1;
  }
  return 0;
}

int main(int argc, char *argv[]) {
  if (argc < 2 || verify(argc, argv)) return 1;

  for (int i = 0; i < argc - 1; i++) printf("%s, ", argv[i]);
  printf("%s\n", argv[argc - 1]);

  return 0;
}

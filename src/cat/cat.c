#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef struct {
  int b;
  int e;
  int n;
  int s;
  int t;
  int v;
} CatFlags;

void NoArgs() {
  char c;
  while (1) {
    if ((c = getchar()) == EOF) return;
    printf("%c", c);
  }
}
int CatParseArg(CatFlags *flags, char *argv) {
  ++argv;
  int k = 0;
  if (*argv == '-') {
    if (strcmp(argv, "-number-nonblank") == 0) flags->b = 1;
    if (strcmp(argv, "-number") == 0) flags->n = 1;
    if (strcmp(argv, "-squeeze-blank") == 0) flags->s = 1;
  } else {
    while (*argv != '\0') {
      char f = *argv;
      switch (f) {
        case 'b':
          flags->b = 1;
          break;
        case 'e':
          flags->v = 1;
          flags->e = 1;
          break;
        case 'E':
          flags->e = 1;
          break;
        case 'n':
          flags->n = 1;
          break;
        case 's':
          flags->s = 1;
          break;
        case 't':
          flags->v = 1;
          flags->t = 1;
          break;
        case 'T':
          flags->t = 1;
          break;
        case 'v':
          flags->v = 1;
          break;
        default:
          k = 1;
          break;
      }
      ++argv;
    }
  }
  return k;
}

void FlagsBN(CatFlags flags, char *c, int *j, char *prev) {
  if ((*prev == '\n' && *c != '\n' && flags.b == 1) ||
      (*prev == '\n' && flags.n == 1)) {
    printf("%6d\t", *j);
    (*j)++;
  }
}

void FlagS(CatFlags flags, FILE *file, char *c, char *prev) {
  if (flags.s == 1 && *prev == '\n' && *c == '\n') {
    while (*c == '\n' && *c != EOF) {
      *prev = *c;
      *c = fgetc(file);
    }
    if (*c != EOF) {
      fseek(file, -2, SEEK_CUR);
      *c = fgetc(file);
    }
  }
}

void FlagT(CatFlags flags, FILE *file, char *c, char *prev) {
  if (flags.t == 1 && *c == '\t')
    while (*c == '\t') {
      printf("^I");
      *prev = *c;
      *c = fgetc(file);
    }
}

void FlagV(CatFlags flags, char *c) {
  if (flags.v && *c != EOF) {
    if (*c < 32 && *c != 9 && *c != 10) {
      *c = *c + 64;
      printf("^");
    } else if (*c == 127) {
      *c = *c - 64;
      printf("^");
    }
  }
}

void FlagE(CatFlags flags, char *c) {
  if ((*c == '\n' || *c == EOF) && flags.e == 1) printf("$");
}

void Print_File(CatFlags flags, char *filename) {
  FILE *file = fopen(filename, "r");
  if (file == NULL) {
    fprintf(stderr, "cat: %s: No such file or directory\n", filename);
    return;
  }
  int j = 1;
  char c, prev = -1;
  c = fgetc(file);

  if ((flags.b == 1 && c != '\n') || flags.n == 1) {
    printf("%6d\t", j);
    (j)++;
  }
  do {
    FlagsBN(flags, &c, &j, &prev);
    FlagS(flags, file, &c, &prev);
    FlagT(flags, file, &c, &prev);
    FlagE(flags, &c);
    prev = c;
    FlagV(flags, &c);
    if (c != EOF)
      printf("%c", c);
    else if (prev != '\0')
      printf("\n");

  } while ((c = fgetc(file)) != EOF);
  fclose(file);
}

void WithArgs(int argc, char **argv) {
  CatFlags flags = {0};
  for (int i = 1; i != argc; i++) {
    if (*argv[i] == '-') {
      if (CatParseArg(&flags, argv[i]) == 1) {
        fprintf(stderr, "illegal option\n");
        return;
      }
    } else
      break;
  }
  if (flags.b == 1) flags.n = 0;
  int l = 0;
  l = 0;
  for (int i = 1; i != argc; i++) {
    if (l == 1)
      Print_File(flags, argv[i]);
    else if (*argv[i] != '-') {
      Print_File(flags, argv[i]);
      l = 1;
    }
  }
}

int main(int argc, char **argv) {
  if (argc == 1)
    NoArgs();
  else
    WithArgs(argc, argv);
}

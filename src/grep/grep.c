#include <getopt.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define N 8192

typedef struct {
  int e;
  int i;
  int v;
  int c;
  int l;
  int n;
  int s;
  int f;
  int h;
  int o;
} GrepFlags;

int fileCheck(char **argv, GrepFlags *flags, char *buff, char *pattern,
              int file_count, int opt_count) {
  FILE *file;
  if ((file = fopen(argv[opt_count], "r")) != NULL) {
    regex_t regex;
    regmatch_t match[1];
    int result = 0;
    int match_count = 0;
    int line_count = 0;
    int regcom =
        regcomp(&regex, pattern, (flags->i ? REG_ICASE : REG_EXTENDED));
    if (regcom == 0) {
      while (fgets(buff, N, file) != NULL) {
        if (strchr(buff, '\n') == NULL) strcat(buff, "\n");
        result = regexec(&regex, buff, 1, match, 0) - flags->v;
        if (result == 0) match_count++;
        line_count++;
        if (!flags->c && !flags->l) {
          if (!flags->h && file_count > 1 && result == 0)
            printf("%s:", argv[opt_count]);
          if (flags->n && result == 0) printf("%d:", line_count);
          if (result == 0 && (!flags->o || (flags->o && flags->v)))
            printf("%s", buff);
          if (flags->o && !flags->v) {
            char *o_flags = buff;
            while (!result && match[0].rm_eo != match[0].rm_so) {
              printf("%.*s\n", (int)(match[0].rm_eo - match[0].rm_so),
                     o_flags + match[0].rm_so);
              o_flags += match[0].rm_eo;
              result = regexec(&regex, o_flags, 1, match, 0);
            }
          }
        }
      }
      if (flags->c || flags->l) {
        if (flags->c && !flags->l) {
          if (file_count > 1 && !flags->h) printf("%s:", argv[opt_count]);
          printf("%d\n", match_count);
        } else if (flags->c && flags->l) {
          if (file_count > 0 && match_count > 0) match_count = 1;
          if (file_count > 1) {
            if (!flags->h) printf("%s:", argv[opt_count]);
            printf("%d\n", match_count);
          } else
            printf("%d\n", match_count);
        }
      }
      if (flags->l && match_count > 0) printf("%s\n", argv[opt_count]);
    }
    regfree(&regex);
    fclose(file);
  } else if (flags->s != 1)
    fprintf(stderr, "grep: %s: No such file or directory:\n", argv[opt_count]);
  return 0;
}

void read_pattern(char **argv, GrepFlags *flags, char *pattern,
                  char *flag_f_name) {
  int i = 0;
  if (!flags->e && !flags->f) {
    snprintf(pattern, N, "%s", argv[optind]);
    optind++;
  }
  if (flags->f) {
    FILE *file_f;
    char cur_sym;
    file_f = fopen(flag_f_name, "r");
    if (file_f != NULL) {
      while ((cur_sym = fgetc(file_f)) != EOF) {
        if (cur_sym == '\n' || cur_sym == 13) {
          cur_sym = '|';
          pattern[i] = cur_sym;
        } else {
          pattern[i] = cur_sym;
        }
        i++;
      }
      fclose(file_f);
    }
  }
  int a = strlen(pattern);
  if (pattern[a - 1] == '|') pattern[a - 1] = '\0';
}

int GrepParseArg(int argc, char *argv[], GrepFlags *flags, char *pattern) {
  int k = 0;
  char flag_f_name[N] = {0};
  int optflags = getopt_long(argc, argv, "e:sivcolhnf:", NULL, 0);
  while (optflags != -1) {
    switch (optflags) {
      case 'e':
        flags->e = 1;
        pattern += snprintf(pattern, N, "%s|", optarg);
        break;
      case 'i':
        flags->i = 1;
        break;
      case 'v':
        flags->v = 1;
        break;
      case 's':
        flags->s = 1;
        break;
      case 'c':
        flags->c = 1;
        break;
      case 'n':
        flags->n = 1;
        break;
      case 'l':
        flags->l = 1;
        break;
      case 'h':
        flags->h = 1;
        break;
      case 'o':
        flags->o = 1;
        break;
      case 'f':
        flags->f = 1;
        snprintf(flag_f_name, N, "%s", optarg);
        break;
      default:
        k = 1;
        fprintf(stderr, "usage: [-eivcln] [file ...]\n");
        break;
    }
    optflags = getopt_long(argc, argv, "e:sivcolhnf:", NULL, 0);
  }
  read_pattern(argv, flags, pattern, flag_f_name);
  return k;
}

int main(int argc, char *argv[]) {
  GrepFlags flags = {0};
  char buff[N] = "";
  char pattern[N] = "";
  if (GrepParseArg(argc, argv, &flags, pattern) == 0) {
    if (argc > 2) {
      int file_count = argc - optind;
      int opt_count = optind;
      while (opt_count < argc) {
        if (fileCheck(argv, &flags, buff, pattern, file_count, opt_count) ==
            0) {
          opt_count++;
        }
      }
    }
  }
  return 0;
}
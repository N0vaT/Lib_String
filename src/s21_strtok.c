#include "s21_string.h"

static int is_delim(char ch, const char *delim) {
  int i = 0;
  while (*delim != '\0') {
    if (ch == *delim) i = 1;
    delim++;
  }
  return i;
}

static char *skip_delimeters(char *str, const char *delim) {
  while (1) {
    if (is_delim(*str, delim)) {
      str++;
      continue;
    }
    if (*str == '\0') str = s21_NULL;
    break;
  }
  return str;
}

char *s21_strtok(char *str, const char *delim) {
  static char *p;
  char *result;

  if (str == s21_NULL) str = p;
  if (str == s21_NULL || *str == '\0') {
    result = s21_NULL;
  } else {
    str = skip_delimeters(str, delim);
    result = str;
    while (1) {
      if (*str == '\0') {
        p = str;
        break;
      }
      if (is_delim(*str, delim)) {
        *str = '\0';
        p = str + 1;
        break;
      }
      str++;
    }
  }
  return result;
}

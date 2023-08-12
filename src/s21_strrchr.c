#include "s21_string.h"

char *s21_strrchr(const char *str, int c) {
  char *result = s21_NULL;
  int i = s21_strlen(str);
  if (str != s21_NULL) {
    for (; i >= 0; i--) {
      if (str[i] == c) {
        result = (char *)&str[i];
        break;
      }
    }
  }
  return result;
}

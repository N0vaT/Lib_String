#include "s21_string.h"

char *s21_strchr(const char *str, int c) {
  s21_size_t length = s21_strlen(str);
  const char *buffer = s21_NULL;
  for (s21_size_t i = 0; i <= length; i++)
    if (c == str[i]) {
      buffer = &str[i];
      break;
    }
  return (char *)buffer;
}

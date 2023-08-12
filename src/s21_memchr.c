#include "s21_string.h"

void *s21_memchr(const void *str, int c, s21_size_t n) {
  s21_size_t length = s21_strlen((const char *)str);
  unsigned char *buf_str = (unsigned char *)str;
  void *result = s21_NULL;
  for (s21_size_t i = 0; i <= length && i < n && !result; i++) {
    if (buf_str[i] == c) result = &buf_str[i];
  }
  return result;
}

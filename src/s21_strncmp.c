#include "s21_string.h"

int s21_strncmp(const char *str1, const char *str2, s21_size_t n) {
  const unsigned char *buf_str1 = (const unsigned char *)str1;
  const unsigned char *buf_str2 = (const unsigned char *)str2;
  int result = 0;
  s21_size_t i = 0;
  for (; (buf_str1[i] || buf_str1[i] == '\0') &&
         (buf_str2[i] || buf_str2[i] == '\0') && i < n && !result;
       i++) {
    if (buf_str1[i] != buf_str2[i]) result = buf_str1[i] - buf_str2[i];
  }
  if (!result && buf_str2[i] == '\0') result = buf_str1[i] - buf_str2[i];
  return result;
}

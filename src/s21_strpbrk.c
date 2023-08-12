#include "s21_string.h"

char *s21_strpbrk(const char *str1, const char *str2) {
  const char *result = s21_NULL;
  s21_size_t i = 0;
  while (str1[i] && !result) {
    s21_size_t j = 0;
    while (str2[j] && !result) {
      if (str1[i] == str2[j]) result = &str1[i];
      j++;
    }
    i++;
  }
  return (char *)result;
}

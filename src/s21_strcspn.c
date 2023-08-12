#include "s21_string.h"

s21_size_t s21_strcspn(const char *str1, const char *str2) {
  s21_size_t result = 0, i = 0;
  int flag = -1;
  if (*str1 == '\0') result = -1;
  if (str2 && *str2 == '\0') result = s21_strlen(str1) - 1;
  while (str1[i] && flag == -1) {
    s21_size_t j = 0;
    while (str2[j] && flag == -1) {
      if (str1[i] == str2[j]) {
        flag = 0;
      }
      result = i;
      j++;
    }
    i++;
  }
  return flag == -1 ? result + 1 : result;
}

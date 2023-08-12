#include "s21_string.h"

void *s21_to_upper(const char *str) {
  char *res = s21_NULL;
  if (str) {
    res = malloc((s21_strlen(str) + 1) * sizeof(char));
    s21_size_t i = 0;
    for (i = 0; i < s21_strlen(str); i++) res[i] = str[i];
    res[i] = '\0';
    for (char *tmp_str = res; *tmp_str; tmp_str++)
      if (*tmp_str >= 'a' && *tmp_str <= 'z') *tmp_str -= 32;
  }
  return res;
}

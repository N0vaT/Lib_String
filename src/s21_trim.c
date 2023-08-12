#include "s21_string.h"

void *s21_trim(char *str, const char *trim_chars) {
  char *res = s21_NULL;
  if (str && trim_chars) {
    char *start_str = str;
    char *end_str = str + s21_strlen(str);
    while (*start_str && s21_strchr(trim_chars, *start_str)) start_str++;
    while (end_str > start_str && s21_strchr(trim_chars, *(end_str - 1)))
      end_str--;
    res = malloc((end_str - start_str + 1) * sizeof(char));
    if (res) {
      s21_strncpy(res, start_str, end_str - start_str);
      *(res + (end_str - start_str)) = '\0';
    }
  }
  return res;
}

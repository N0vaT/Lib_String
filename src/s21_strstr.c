#include "s21_string.h"

int compare(const char *str1, const char *str2) {
  while (*str1 && *str2) {
    if (*str1 != *str2) {
      return 0;
    }
    str1++;
    str2++;
  }
  return (*str2 == '\0');
}

char *s21_strstr(const char *haystack, const char *needle) {
  char *result = s21_NULL;
  if (needle && *needle == '\0') result = (char *)haystack;
  while (*haystack != '\0') {
    if ((*haystack == *needle) && compare(haystack, needle)) {
      result = (char *)haystack;
      break;
    }
    haystack++;
  }
  return result;
}

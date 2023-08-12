#include "s21_string.h"

char *s21_strncat(char *dest, const char *src, s21_size_t n) {
  s21_size_t size_dest = s21_strlen(dest);
  s21_size_t size_src = 0;
  while (size_src < n && src[size_src]) {
    dest[size_dest] = src[size_src];
    size_dest++;
    size_src++;
  }
  dest[size_dest] = '\0';
  return dest;
}

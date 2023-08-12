#include "s21_string.h"

char *s21_strncpy(char *dest, const char *src, s21_size_t n) {
  unsigned char *buf_dest = (unsigned char *)dest;
  const unsigned char *buf_src = (const unsigned char *)src;
  s21_size_t i = 0;
  for (; i < n && buf_src[i]; i++) buf_dest[i] = buf_src[i];
  if (buf_src[i] == '\0') buf_dest[i] = buf_src[i];
  return (void *)buf_dest;
}

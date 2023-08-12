#include "s21_string.h"

char *s21_strcpy(char *dest, const char *src) {
  unsigned char *buf_dest = (unsigned char *)dest;
  const unsigned char *buf_src = (const unsigned char *)src;
  s21_size_t length = s21_strlen(src);
  for (s21_size_t i = 0; i <= length; i++) buf_dest[i] = buf_src[i];
  return (void *)buf_dest;
}

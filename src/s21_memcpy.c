#include "s21_string.h"

void *s21_memcpy(void *dest, const void *src, s21_size_t n) {
  unsigned char *buf_dest = (unsigned char *)dest;
  const unsigned char *buf_src = (const unsigned char *)src;
  for (s21_size_t i = 0; i < n; i++) buf_dest[i] = buf_src[i];
  return (void *)buf_dest;
}

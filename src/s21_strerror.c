#include "s21_strerror.h"

#include "s21_string.h"

char *s21_strerror(int errnum) {
  static char err[100];

  if (errnum < 0 || errnum >= S21_ERRLIST_SIZE)
    sprintf(err, "Unknown error: %d", errnum);
  else
    s21_strncpy(err, s21_sys_errlist[errnum], 100);

  return err;
}

#ifndef S21_SSCANF_H
#define S21_SSCANF_H

#include <math.h>
#include <stdarg.h>
#include <stdlib.h>

#include "s21_string.h"

#define BUFF_SIZE 512

typedef struct {
  int no_match;
  int result;
  int width;
  int star;
  int length;
  int base;
  int pointer;
  long long int value;
  unsigned long long int value_unsigned;
  long double value_f;
  char specifier;
  int count;
  int char_flag;
  char *tmp_ptr;
  int skleyka;
  int result_zero;

} opt_sscanf;

void check_value_unsigned(char **src, opt_sscanf *Options);
void put_value_unsigned(va_list factor, opt_sscanf *Options);

void check_per(char **src, opt_sscanf *Options);
void skip_spaces(char **src, char **format, opt_sscanf *Options);
void processing(char **src, char **format, va_list factor, opt_sscanf *Options);

void put_str(va_list factor, opt_sscanf *Options, char **src);
void check_width(char **format, opt_sscanf *Options);

void put_value(va_list factor, opt_sscanf *Options);
void check_value_f(char **src, opt_sscanf *Options);
void put_value_f(va_list factor, opt_sscanf *Options);
void put_hex(va_list factor, opt_sscanf *Options, char **src);
void put_count(va_list factor, opt_sscanf *Options, char **src);
void scientfic_notification(char **src, opt_sscanf *Options);

unsigned long long int get_hex_number(char *string, opt_sscanf *Options);
char get_dec_digit(char number);
int check_space(char ch, opt_sscanf *Options);
char *read_string(char **src, opt_sscanf *Options);
void priority_put_char(char **src, va_list factor, opt_sscanf *Options);
void put_char(char **src, va_list factor, opt_sscanf *Options);
int result_change(opt_sscanf *Options);
void check_value_i(char **src, opt_sscanf *Options, va_list factor);
void check_value(char **src, opt_sscanf *Options, va_list factor);

#endif

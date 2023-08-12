#include "s21_sscanf.h"

int s21_sscanf(const char *str, const char *format, ...) {
  s21_size_t size_str = s21_strlen(str);        // Длина str
  s21_size_t size_format = s21_strlen(format);  // Длина str
  opt_sscanf Options = {0};
  char *tmp_str = calloc((size_str) + 1, sizeof(char));
  char *tmp_format = calloc((size_format) + 1, sizeof(char));
  char *s = tmp_str;
  char *f = tmp_format;
  s21_strncpy(tmp_str, str, size_str);
  s21_strncpy(tmp_format, format, size_format);

  va_list factor;  // указатель на необязательный параметр
  va_start(factor, format);  // устанавливаем указатель
  Options.no_match = 1;
  Options.skleyka = 1;

  for (; *tmp_format && Options.no_match; tmp_str++, tmp_format++) {
    skip_spaces(&tmp_str, &tmp_format, &Options);
    // printf("%s|%s \n", tmp_str, tmp_format);

    Options.count = tmp_str - s;
    if (*tmp_str == '\0') {
      Options.no_match = 0;
      // if(*tmp_str == '\0' && !Options.skleyka){
      //   Options.no_match = 0;
    } else if (*tmp_format == '%' || Options.length != 0) {
      processing(&tmp_str, &tmp_format, factor, &Options);
    } else if (*tmp_str == *tmp_format) {
      continue;
    } else {
      Options.no_match = 0;
    }
    Options.skleyka = 1;
  }
  va_end(factor);
  free(s);
  free(f);
  return result_change(&Options);
}

// int main() {
//     char s1[BUFF_SIZE] = {'\0'};
//     char s2[BUFF_SIZE] = {'\0'};
//     char s3[BUFF_SIZE] = {'\0'};
//     char s4[BUFF_SIZE] = {'\0'};

//     char s5[BUFF_SIZE] = {'\0'};
//     char s6[BUFF_SIZE] = {'\0'};
//     char s7[BUFF_SIZE] = {'\0'};
//     char s8[BUFF_SIZE] = {'\0'};

//     const char str[] = "aboba\tshlepa\tasdfasdf\tasdfasdf";
//     const char fstr[] = "%150s%1s%1s\t%3s";

//     int16_t res1 = s21_sscanf(str, fstr, s1, s2, s3, s4);
//     int16_t res2 = sscanf(str, fstr, s5, s6, s7, s8);

//     printf("OUR:  '%d' '%s' '%s' '%s' '%s'\n", res1, s1, s2, s3, s4);

//     printf("ORIGINAL: '%d' '%s' '%s' '%s' '%s' \n", res2, s5, s6, s7, s8);

// }

void processing(char **src, char **format, va_list factor,
                opt_sscanf *Options) {
  if (Options->length == 0) (*format)++;
  check_width(format, Options);
  switch (**format) {
    case 'x':
    case 'X':
      put_hex(factor, Options, src);
      break;
    case 'p':
      Options->pointer = 1;
      put_hex(factor, Options, src);
      break;
    case 'e':
    case 'E':
    case 'f':
    case 'g':
    case 'G':
      check_value_f(src, Options);
      put_value_f(factor, Options);
      break;
    case 'c':
      priority_put_char(src, factor, Options);
      break;
    case 'd':
      check_value(src, Options, factor);
      break;
    case 'i':
      check_value_i(src, Options, factor);

      break;
    case 'o':
      Options->base = 8;
      check_value_unsigned(src, Options);
      put_value_unsigned(factor, Options);
      break;
    case 's':
      put_str(factor, Options, src);
      break;
    case 'u':
      check_value_unsigned(src, Options);
      put_value_unsigned(factor, Options);
      break;
    case 'n':
      put_count(factor, Options, src);
      break;
    case '%':
      check_per(src, Options);
      break;
    case 'h':
      (*src)--;
      Options->length = 1;
      break;
    case '*':
      Options->result_zero = 1;
      Options->star = 1;
      (*src)--;
      **format = '%';
      (*format)--;
      break;
    case 'l':
      Options->length = 2;
      if (*(*format + 1) == 'l') {
        Options->length = 3;
        (*format)++;
      }
      (*src)--;
      break;
    case 'L':
      Options->length = 4;
      (*src)--;
      break;
  }
}

void put_value(va_list factor, opt_sscanf *Options) {
  if ((!Options->star) ^ (!Options->no_match)) {
    if (Options->length == 0 || Options->length == 4) {
      *(int *)va_arg(factor, void *) = Options->value;
    }
    if (Options->length == 1) {
      *(short *)va_arg(factor, void *) = Options->value;
    }
    if (Options->length == 2) {
      *(long int *)va_arg(factor, void *) = Options->value;
    }
    if (Options->length == 3) {
      *(long long int *)va_arg(factor, void *) = Options->value;
    }
    va_end(factor);
    Options->result++;
  }
  Options->value = 0;
  Options->length = 0;
  Options->star = 0;
  Options->width = 0;
}

void put_value_f(va_list factor, opt_sscanf *Options) {
  if ((!Options->star) ^ (!Options->no_match)) {
    if (Options->length == 0 || Options->length == 1) {
      *(float *)va_arg(factor, void *) = Options->value_f;
    }
    if (Options->length == 2) {
      *(double *)va_arg(factor, void *) = Options->value_f;
    }
    if (Options->length == 4) {
      *(long double *)va_arg(factor, void *) = Options->value_f;
    }

    va_end(factor);
    Options->result++;
  }
  Options->value_f = 0;
  Options->length = 0;
  Options->star = 0;
  Options->width = 0;
  Options->result_zero = 1;
}

void check_value_f(char **src, opt_sscanf *Options) {
  int flag = 1;
  int minus = 0;
  if (**src == '-') {
    if (Options->width == 1) Options->no_match = 0;
    (*src)++;
    minus = 1;
  } else if (**src == '+') {
    if (Options->width == 1) Options->no_match = 0;
    (*src)++;
  }
  if (((**src == 'i' || **src == 'I') &&
       (*(*src + 1) == 'n' || *(*src + 1) == 'N') &&
       (*(*src + 2) == 'f' || *(*src + 2) == 'F'))) {
    Options->value_f = INFINITY;
    *src = *src + 3;
  } else if (((**src == 'n' || **src == 'N') &&
              (*(*src + 1) == 'a' || *(*src + 1) == 'A') &&
              (*(*src + 2) == 'n' || *(*src + 2) == 'N'))) {
    Options->value_f = NAN;
    *src = *src + 3;
  } else {
    if (**src <= 47 || **src >= 58) {
      Options->no_match = 0;
    }
    for (; *src && flag;) {
      if (**src > 47 && **src < 58) {
        Options->value_f = Options->value_f * 10 + (int)**src - 48;
        (*src)++;
      } else {
        flag = 0;
      }
    }
    flag = 1;
    if (**src == '.') {
      (*src)++;
      double number = 0.0;
      int cnt = 0;
      for (; *src && flag;) {
        cnt++;
        if (**src > 47 && **src < 58) {
          number = number * 10 + (int)**src - 48;
          (*src)++;
        } else {
          flag = 0;
        }
      }
      Options->value_f += (double)number / (pow(10, cnt - 1));
    }
    scientfic_notification(src, Options);
  }
  if (minus) Options->value_f *= -1;
}

void check_value(char **src, opt_sscanf *Options, va_list factor) {
  int flag = 1;
  int minus = 0;
  if (**src == '-') {
    if (Options->width == 1) Options->no_match = 0;
    (*src)++;
    minus = 1;
    Options->width--;
  } else if (**src == '+') {
    if (Options->width == 1) Options->no_match = 0;
    (*src)++;
    Options->width--;
  }
  if (**src <= 47 || **src >= 58) {
    Options->no_match = 0;
  }
  int flag_width = Options->width != 0 ? Options->width : -1;
  int i = 0;
  for (; **src != '\0' && flag && i != flag_width; i++) {
    if (**src > 47 && **src < 58) {
      Options->value = Options->value * 10 + (int)**src - 48;
      (*src)++;
    } else {
      flag = 0;
      (*src)--;
    }
  }
  // printf("----");
  if (flag_width != -1) {
    (*src)--;
  }
  if (minus) {
    Options->value *= -1;
  }
  put_value(factor, Options);
}

void check_value_unsigned(char **src, opt_sscanf *Options) {
  int flag = 1;
  int minus = 0;
  if (**src == '-') {
    if (Options->width == 1) Options->no_match = 0;
    (*src)++;
    minus = 1;
  } else if (**src == '+') {
    if (Options->width == 1) Options->no_match = 0;
    (*src)++;
  }
  if (**src <= 47 || **src >= 58) {
    Options->no_match = 0;
  }
  int base = 10;
  int largest_digit = 58;

  if (Options->base == 8) {
    base = 8;
    largest_digit = 56;
  }
  int flag_width = Options->width != 0 ? Options->width : -1;
  int i = 0;
  for (; *src && flag && i != flag_width; i++) {
    if (**src > 47 && **src < largest_digit) {
      Options->value_unsigned =
          Options->value_unsigned * base + (int)**src - 48;
      (*src)++;
    } else {
      flag = 0;
      (*src)--;
    }
  }
  if (flag_width != -1) {
    (*src)--;
  }

  if (minus) Options->value_unsigned *= -1;
}

void put_value_unsigned(va_list factor, opt_sscanf *Options) {
  if ((!Options->star) ^ (!Options->no_match)) {
    if (Options->pointer) {
      *(void **)va_arg(factor, void **) = (void *)Options->value_unsigned;
    } else {
      if (Options->length == 0 || Options->length == 4) {
        *(unsigned int *)va_arg(factor, void *) = Options->value_unsigned;
      }
      if (Options->length == 1) {
        *(unsigned short *)va_arg(factor, void *) = Options->value_unsigned;
      }
      if (Options->length == 2) {
        *(unsigned long int *)va_arg(factor, void *) = Options->value_unsigned;
      }
      if (Options->length == 3) {
        *(unsigned long long int *)va_arg(factor, void *) =
            Options->value_unsigned;
      }
    }

    va_end(factor);
    Options->result++;
  }
  Options->value_unsigned = 0;
  Options->length = 0;
  Options->base = 0;
  Options->star = 0;
  Options->width = 0;
  Options->result_zero = 1;
}

void check_width(char **format, opt_sscanf *Options) {
  int flag = 1;
  for (; *format && flag;) {
    if (**format > 47 && **format < 58) {
      Options->width = Options->width * 10 + (int)**format - 48;
      (*format)++;
    } else {
      flag = 0;
    }
  }
}

void put_str(va_list factor, opt_sscanf *Options, char **src) {
  char *new_str = read_string(src, Options);

  if (!Options->star) {
    int k = s21_strlen(new_str) + 1;
    va_list tmp_list;
    va_copy(tmp_list, factor);
    for (int j = 0; j < k; j++) {
      *((char *)va_arg(tmp_list, char *) + j) = new_str[j];
      if (j + 1 < k) {
        va_end(tmp_list);
        va_copy(tmp_list, factor);
      }
    }
    va_end(factor);
    va_copy(factor, tmp_list);
    va_end(tmp_list);
    Options->result++;
  }
  Options->star = 0;
  Options->width = 0;
  free(new_str);
}

void check_per(char **src, opt_sscanf *Options) {
  if (**src != '%') {
    Options->no_match = 0;
  } else {
    Options->result_zero = 1;
  }
}

void skip_spaces(char **src, char **format, opt_sscanf *Options) {
  Options->tmp_ptr = *src;
  if (**src == **format || (!check_space(**src, Options) && **format == '%')) {
    while (!check_space(**src, Options)) {
      (*src)++;
    }
  }
  while (!check_space(**format, Options)) {
    (*format)++;
    Options->skleyka = 0;
  }
}

char get_dec_digit(char number) {
  int decNumbers[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
  const char hexNumbers[] = {'0', '1', '2', '3', '4', '5', '6', '7',
                             '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
  char result = ' ';
  for (unsigned long i = 0; i < sizeof(decNumbers) / sizeof(decNumbers[0]);
       i++) {
    if (number == hexNumbers[i]) result = decNumbers[i];
  }
  return result;
}

// Writes hex number to string and returns it
unsigned long long int get_hex_number(char *string, opt_sscanf *Options) {
  unsigned long long int value = 0;
  int length_number = s21_strlen(string);
  if (*string == '\0') {
  } else if (!((*string > 47 && *string < 58) ||
               (*string > 64 && *string < 71))) {
    Options->no_match = 0;
  }
  for (int i = 0; i < length_number; i++) {
    if ((*string > 47 && *string < 58) || (*string > 64 && *string < 71)) {
      value = value * 16 + get_dec_digit(*string);

      string++;
    }
  }
  return value;
}

void put_hex(va_list factor, opt_sscanf *Options, char **src) {
  int minus = 0;
  if (**src == '-') {
    (*src)++;
    minus = 1;
  } else if (**src == '+')
    (*src)++;

  if ((**src == '0') && ((*(*src + 1) == 'x') || (*(*src + 1) == 'X'))) {
    *src += 2;
  }
  char *new_str = read_string(src, Options);

  char *new_upper_str = s21_to_upper(new_str);

  Options->value_unsigned = get_hex_number(new_upper_str, Options);
  if (minus) Options->value_unsigned *= -1;

  put_value_unsigned(factor, Options);

  Options->pointer = 0;
  Options->no_match = 1;
  Options->result_zero = 1;

  free(new_str);
  free(new_upper_str);
}

void put_count(va_list factor, opt_sscanf *Options, char **src) {
  if (!Options->star) {
    *(int *)va_arg(factor, void *) = Options->count;
    va_end(factor);
  }
  (*src)--;
  Options->result_zero = 1;
  Options->star = 0;
  Options->width = 0;
}

void scientfic_notification(char **src, opt_sscanf *Options) {
  if ((**src == 'e' || **src == 'E') &&
      (*(*src + 1) == '+' || *(*src + 1) == '-') &&
      (*(*src + 2) > 47 && *(*src + 2) < 58)) {
    int sign = *(*src + 1) == '+' ? 0 : 1;

    *src += 2;
    int degree = 0;
    int flag = 1;
    for (; **src && flag;) {
      if (**src > 47 && **src < 58) {
        degree = degree * 10 + (int)**src - 48;
        (*src)++;
      } else {
        flag = 0;
      }
    }

    if (sign) degree *= -1;
    Options->value_f = Options->value_f * pow(10, degree);
  }
}

int check_space(char ch, opt_sscanf *Options) {
  int result = 1;
  if ((ch == ' ' || ch == '\n' || ch == '\t' || ch == '\r' || ch == '\v' ||
       ch == '\f') &&
      !Options->char_flag) {
    result = 0;
  }
  return result;
}

char *read_string(char **src, opt_sscanf *Options) {
  char *new_str = s21_NULL;
  char *tmp_str = s21_NULL;
  new_str = malloc(sizeof(char));
  if (new_str) {
    int i = 0;
    for (; **src && check_space(**src, Options) &&
           (Options->width == 0 || i < Options->width);
         i++) {
      new_str[i] = **src;
      tmp_str = realloc(new_str, (i + 2) * sizeof(char));
      if (tmp_str != s21_NULL) new_str = tmp_str;
      (*src)++;
    }
    (*src)--;
    new_str[i] = '\0';
    // free(tmp_str);
  }
  return new_str;
}

void put_char(char **src, va_list factor, opt_sscanf *Options) {
  if (!Options->star) {
    *(char *)va_arg(factor, void *) = **src;
    va_end(factor);
    Options->result++;
  }
  Options->star = 0;
}

void priority_put_char(char **src, va_list factor, opt_sscanf *Options) {
  if (Options->skleyka) *src = *src - (*src - Options->tmp_ptr);

  if (Options->width) {
    Options->char_flag = 1;
    put_str(factor, Options, src);
    Options->char_flag = 0;
  } else {
    put_char(src, factor, Options);
  }
  Options->width = 0;
}

int result_change(opt_sscanf *Options) {
  int result = 0;
  if ((Options->result_zero) && !Options->result) {
  } else if (!Options->result) {
    result = -1;
  } else {
    result = Options->result;
  }
  return result;
}

void check_value_i(char **src, opt_sscanf *Options, va_list factor) {
  if ((**src == '0') && ((*(*src + 1)) == 'x' || (*(*src + 1)) == 'X')) {
    put_hex(factor, Options, src);
  } else if (**src == '0') {
    Options->base = 8;
    check_value_unsigned(src, Options);
    put_value_unsigned(factor, Options);
  } else {
    check_value(src, Options, factor);
  }
}

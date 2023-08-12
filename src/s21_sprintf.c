#include "s21_sprintf.h"

int s21_sprintf(char *str, const char *format, ...) {
  va_list arg;
  va_start(arg, format);

  int error = 0;
  int success = 0;

  opt opts = {0};
  str[0] = '\0';

  while (*format) {
    if (!error) {
      s21_memset(&opts, 0, sizeof(opt));
      if (*format == '%') {
        format++;
        format = parser(format, &opts, &error, arg);
        str = s21_handler(str, opts, arg, &success);
      } else {
        *str++ = *format++;
        success++;
      }
    } else
      break;
  }
  *str = '\0';
  va_end(arg);
  return success;
}

int s21_atoi(char *digit) {
  int result = 0;
  int sign = 1;

  while (*digit == ' ') digit++;
  if (*digit == '-') {
    digit++;
    sign = -1;
  }
  if (*digit == '+') digit++;

  while (*digit >= '0' && *digit <= '9')
    result = result * 10 + (*digit++ - '0');
  return result * sign;
}

const char *parser(const char *format, opt *opts, int *error, va_list arg) {
  format = get_flags(format, opts);
  format = get_width(format, opts, arg);
  format = get_precision(format, opts, arg);
  format = get_length(format, opts);
  format = get_specifier(format, opts, error);

  return format;
}

const char *get_flags(const char *format, opt *opts) {
  int exit = 0;
  opts->minus = false;
  opts->plus = false;
  opts->space = false;
  opts->hash = false;
  opts->zero = false;
  while (*format == '-' || *format == '+' || *format == ' ' || *format == '0' ||
         *format == '#') {
    switch (*format) {
      case '-':
        opts->minus = true;
        break;
      case '+':
        opts->plus = true;
        break;
      case ' ':
        opts->space = true;
        break;
      case '#':
        opts->hash = true;
        break;
      case '0':
        opts->zero = true;
        break;
      default:
        exit++;
        break;
    }
    format++;
  }
  return format;
}

const char *get_width(const char *format, opt *opts, va_list arg) {
  if (*format == '*') {
    opts->width = va_arg(arg, int);
    format++;
  } else {
    char digit[BUFF_SIZE] = {'\0'};
    for (int i = 0; *format >= '0' && *format <= '9'; i++) digit[i] = *format++;
    opts->width = s21_atoi(digit);
  }
  return format;
}

const char *get_precision(const char *format, opt *opts, va_list arg) {
  if (*format == '.') {
    opts->is_precision_set = true;
    format++;
    if (*format == '*') {
      opts->precision = va_arg(arg, int);
      format++;
    } else {
      char tmp[BUFF_SIZE] = {'\0'};
      for (int i = 0; *format >= '0' && *format <= '9'; i++) tmp[i] = *format++;
      opts->precision = s21_atoi(tmp);
    }
  } else {
    opts->is_precision_set = false;
    opts->precision = -1;
  }

  return format;
}

const char *get_length(const char *format, opt *opts) {
  opts->length = '0';
  if (*format == 'h' || *format == 'l' || *format == 'L') {
    opts->length = *format;
    format++;
  }
  return format;
}

const char *get_specifier(const char *format, opt *opts, int *error) {
  char tmp = *format;
  if (tmp != '\0') {
    char *pattern = "cdieEfgGosuxXpn";
    if (s21_strchr(pattern, tmp) != NULL) {
      opts->specifier = *format++;
    } else if (tmp == '%') {
      opts->specifier = *format++;
    } else {
      *error = 1;
    }
  }
  return format;
}

char *s21_handler(char *string, opt opts, va_list arg, int *success) {
  char line[1024];
  int sign = 1;
  getAsString(line, opts, arg, &sign, success);

  if (opts.hash && s21_strchr("oxX", opts.specifier) != s21_NULL) {
    addPrefix(line, opts.specifier == 'o'   ? "0"
                    : opts.specifier == 'x' ? "0x"
                                            : "0X");
  }

  int length = s21_strlen(line);
  if (opts.plus || sign < 0)
    addSign(line, sign > 0 ? '+' : '-', length++);
  else if (opts.space)
    addSign(line, sign > 0 ? ' ' : '-', length++);
  if (opts.width > 0 && opts.width >= length) {
    if (opts.minus) {  // Left-justify
      s21_strcpy(string, line);
      string += length;
      writeChars(string, opts.zero ? '0' : ' ', opts.width - length);
      string += opts.width - length;
    } else {  // Right-justify
      writeChars(string, opts.zero ? '0' : ' ', opts.width - length);
      string += opts.width - length;
      s21_strcpy(string, line);
      string += length;
    }
    *success += opts.width - length;
  } else {
    s21_strcpy(string, line);
    string += length;
  }
  *success += length;

  return string;
}

void getAsString(char *line, opt opts, va_list arg, int *sign,
                 const int *success) {
  if (opts.specifier == 'x' || opts.specifier == 'X') {
    long arglong = getAppropriateLengthUnsignedInt(opts, arg);
    if (arglong < 0) *sign = -1;
    getHexNumber(line, arglong, opts.precision);
    if (opts.specifier == 'x') lowercase(line);
  } else if (opts.specifier == 'o') {
    long arglong = getAppropriateLengthInt(opts, arg);
    if (arglong < 0) *sign = -1;
    getOctNumber(line, arglong, opts.precision);
  } else if (opts.specifier == 'p') {
    writePointer(line, va_arg(arg, long));
  } else if (opts.specifier == 'c') {
    int argint = va_arg(arg, int);
    if (argint < 0) *sign = -1;
    writeChar(line, argint);
  } else if (opts.specifier == 'd' || opts.specifier == 'i' ||
             opts.specifier == 'u') {
    long arglong = getAppropriateLengthInt(opts, arg);
    if (arglong < 0) *sign = -1;
    writeInteger(line, arglong, opts.precision);
  } else if (opts.specifier == 'E' || opts.specifier == 'e') {
    long double argdouble = getAppropriateLengthFloat(opts, arg);
    if (argdouble < 0) *sign = -1;
    getScientificNotation(line, argdouble,
                          opts.precision == -1 ? 6 : opts.precision,
                          opts.specifier, 0);
  } else if (opts.specifier == 'f') {
    long double argdouble = getAppropriateLengthFloat(opts, arg);
    if (argdouble < 0) *sign = -1;
    writeDouble(line, argdouble, opts.precision == -1 ? 6 : opts.precision, 0);
  } else if (opts.specifier == 's') {
    char *atr_str = va_arg(arg, char *);
    int atrStrLen = s21_strlen(atr_str);
    if (opts.precision == -1) {
      s21_strcpy(line, atr_str);
      line[atrStrLen] = '\0';
    } else
      s21_strncpy(line, atr_str,
                  opts.precision)[atrStrLen < opts.precision ? atrStrLen
                                                             : opts.precision] =
          '\0';
  } else if (opts.specifier == 'n') {
    int *atrpointer = (int *)va_arg(arg, long);
    *atrpointer = *success;
    line[0] = '\0';
  } else if (opts.specifier == 'g' || opts.specifier == 'G') {
    getG(line, opts, arg, sign);
  }
}

long double getAppropriateLengthFloat(opt opts, va_list arg) {
  long double result;
  if (opts.length == 'l')
    result = (long double)va_arg(arg, double);
  else if (opts.length == 'L')
    result = va_arg(arg, long double);
  else
    result = (long double)va_arg(arg, double);
  return result;
}

long getAppropriateLengthInt(opt opts, va_list arg) {
  long result;
  if (opts.length == 'l')
    result = va_arg(arg, long);
  else if (opts.length == 'h')
    result = (long)va_arg(arg, int);
  else
    result = (long)va_arg(arg, int);
  return result;
}

long getAppropriateLengthUnsignedInt(opt opts, va_list arg) {
  long result;
  if (opts.length == 'l')
    result = va_arg(arg, unsigned long);
  else if (opts.length == 'h')
    result = (long)va_arg(arg, unsigned int);
  else
    result = (long)va_arg(arg, unsigned int);
  return result;
}

void getG(char *line, opt opts, va_list arg, int *sign) {
  char bufExp[1024];
  char bufFloat[1024];
  long double argdouble = getAppropriateLengthFloat(opts, arg);
  if (argdouble < 0) *sign = -1;
  writeDouble(
      bufFloat, argdouble,
      opts.precision == -1 ? 6 : (opts.precision == 0 ? 2 : opts.precision), 1);
  getScientificNotation(bufExp, argdouble,
                        opts.precision == -1 ? 6 : opts.precision,
                        opts.specifier == 'G' ? 'E' : 'e', 2);
  if (argdouble == 0) {
    line[0] = '0';
    line[1] = '\0';
  } else if (s21_strlen(bufExp) < s21_strlen(bufFloat)) {
    s21_strcpy(line, bufExp);
    line[s21_strlen(bufExp)] = '\0';
  } else {
    s21_strcpy(line, bufFloat);
    line[s21_strlen(bufFloat)] = '\0';
  }
}

void addPrefix(char *string, char *prefix) {
  if (!ensureNuls(string)) {
    int prefixLength = s21_strlen(prefix);
    int stringLength = s21_strlen(string);
    for (int i = stringLength; i >= 0; i--)
      string[i + prefixLength] = string[i];
    for (int i = 0; i < prefixLength; i++) string[i] = prefix[i];
  }
}

bool ensureNuls(const char *string) {
  bool flag = true;
  for (int i = 0; string[i] != '\0'; i++)
    if (string[i] != '0') flag = false;
  return flag;
}

void addSign(char *string, char sign, int length) {
  for (int i = length - 1; i >= 0; i--) string[i + 1] = string[i];
  string[0] = sign;
}

void writeChars(char *string, char c, int number) {
  for (int i = 0; i < number; i++) string[i] = c;
  string[number] = '\0';
}

char *writeChar(char *string, char c) {
  string[0] = c;
  string[1] = '\0';
  return string + 1;
}

void lowercase(char *string) {
  for (int i = 0; string[i] != '\0'; i++)
    if (string[i] >= 'A' && string[i] <= 'Z') string[i] -= 'A' - 'a';
}

// Returns a char of the digit in hex given a dec number < 16
char getHexDigit(int number) {
  int decNumbers[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
  const char hexNumbers[] = {'0', '1', '2', '3', '4', '5', '6', '7',
                             '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
  char result = ' ';
  for (unsigned long i = 0; i < sizeof(decNumbers) / sizeof(decNumbers[0]);
       i++) {
    if (number == decNumbers[i]) result = hexNumbers[i];
  }
  return result;
}

// Writes hex number to string and returns it
char *getHexNumber(char *string, long number, int precision) {
  if (number == 0) {
    for (int j = 0;
         (j < precision && precision != -1) || (precision == -1 && j < 1); j++)
      string++[0] = '0';
    string[0] = '\0';
  } else {
    int hexNumber[15 + 1];
    int i = 0;  // Number of hex digits
    while (number != 0) {
      int remainder = number % 16;
      number /= 16;
      hexNumber[i] = remainder;
      i++;
    }
    string[i > precision ? i : precision] = '\0';
    for (int j = 0; j < precision - i && precision != -1; j++)
      string++[0] = '0';
    for (int j = i - 1; j >= 0; j--)
      string[j] = getHexDigit(hexNumber[(i - 1) - j]);
  }

  return string;
}

// Returns a char of the digit in oct given a dec number < 8
char getOctDigit(int number) {
  int decNumbers[] = {0, 1, 2, 3, 4, 5, 6, 7};
  const char hexNumbers[] = {'0', '1', '2', '3', '4', '5', '6', '7'};
  char result = ' ';
  for (unsigned long i = 0; i < sizeof(decNumbers) / sizeof(decNumbers[0]);
       i++) {
    if (number == decNumbers[i]) result = hexNumbers[i];
  }
  return result;
}

// Writes oct number to string and returns it
char *getOctNumber(char *string, long number, int precision) {
  if (number == 0) {
    string[0] = '0';
    string[1] = '\0';
  } else {
    int octNumber[15 + 1];
    int i = 0;  // Number of hex digits
    while (number != 0) {
      int remainder = number % 8;
      number /= 8;
      octNumber[i] = remainder;
      i++;
    }
    string[i > precision ? i : precision] = '\0';
    for (int j = 0; j < precision - i && precision != -1; j++) {
      string[0] = '0';
      string++;
    }
    for (int j = i - 1; j >= 0; j--) {
      string[j] = getOctDigit(octNumber[(i - 1) - j]);
    }
  }
  return string;
}

char getDigitChar(int digit) {
  char zero = '0';
  return zero + digit;
}

char *writeInteger(char *string, long number, int precision) {
  int temp[1024];
  int i = 0;
  if (number < 0) number *= -1;
  if (number == 0 && precision != 0) {
    temp[0] = 0;
    i++;
  }
  while (number != 0) {
    temp[i] = number % 10;
    number /= 10;
    i++;
  }
  for (int j = 0; j < precision - i && precision != -1; j++) string++[0] = '0';
  for (int j = i - 1; j >= 0; j--) {
    string[j] = getDigitChar(temp[(i - 1) - j]);
  }
  string[i] = '\0';
  return string + i;
}

char *writeDouble(char *string, long double number, int precision, int flagG) {
  char *start = string;
  if (number < 0) number *= -1;
  string = writeInteger(string, (long)(number / 1), 0);
  if ((number == 0) || ((long)(number / 1) == 0)) string++[0] = '0';
  if (precision > 0) string++[0] = '.';
  number -= (long)(number / 1);
  for (int k = 0; k < precision; k++) {
    number *= 10;
    string[0] = getDigitChar((int)(number / 1));
    string++;
    number -= (int)(number / 1);
  }
  string[0] = '\0';
  if ((number * 10) >= 5)
    for (int i = -1;; i--) {
      if (string[i] == '.') continue;
      if (++string[i] <= '9')
        break;
      else
        string[i] = '0';
    }
  while ((string[-1] == '0' || string[-1] == '.') && flagG == 2) {
    if (string[-1] == '.') flagG = 0;
    string-- [-1] = '\0';
  }
  while (string[-1] == '0' && string[-2] != '.' && flagG == 1)
    string-- [-1] = '\0';
  int i = 0;
  for (int k = 0; start[k] != '\0' && flagG == 1; k++)
    if (start[k] != '.') i++;
  for (int k = 1; k < i - precision + 1; k++)
    if (string[-k] != '.')
      string[-k] = '\0';
    else {
      string--;
      k--;
    }
  return string;
}

char *getScientificNotation(char *string, long double number, int precision,
                            char e, int flagG) {
  long double mantissa = number;
  if (mantissa < 0) mantissa *= -1;
  long exponent = 0;
  while (mantissa >= 10) {
    mantissa /= 10;
    exponent++;
  }
  while (mantissa < 1 && mantissa != 0) {
    mantissa *= 10;
    exponent--;
  }
  char *pointer = string;
  pointer = writeDouble(pointer, mantissa, precision, flagG);
  pointer++[0] = e;
  pointer++[0] = (exponent >= 0) ? '+' : '-';
  pointer = writeInteger(pointer, exponent, 2);
  pointer[0] = '\0';
  return string;
}

char *writePointer(char *string, long pointer) {
  string[0] = '0';
  string[1] = 'x';
  getHexNumber(string + 2, pointer, -1);
  for (int i = 2; string[i] != '\0'; i++) string[i] = tolower(string[i]);
  return string;
}

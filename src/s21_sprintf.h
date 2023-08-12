#ifndef SRC_S21_SPRINTF_H_
#define SRC_S21_SPRINTF_H_

#include <ctype.h>
#include <math.h>
#include <stdarg.h>
#include <stdbool.h>

#include "s21_string.h"

#define BUFF_SIZE 512

typedef struct {
  bool minus;
  bool plus;
  bool space;
  bool hash;
  bool zero;
  int precision;
  int is_precision_set;
  int width;
  char length;
  char specifier;
} opt;

char* s21_handler(char* string, opt opts, va_list arg, int* success);

int s21_atoi(char* digit);
const char* parser(const char* format, opt* opts, int* error, va_list arg);
const char* get_flags(const char* format, opt* opts);
const char* get_width(const char* format, opt* opts, va_list arg);
const char* get_precision(const char* format, opt* opts, va_list arg);
const char* get_length(const char* format, opt* opts);
const char* get_specifier(const char* format, opt* opts, int* error);

char* getHexNumber(char* string, long number, int precision);
void lowercase(char* string);
char* getOctNumber(char* string, long number, int precision);
char* writePointer(char* string, long pointer);
char* writeChar(char* string, char c);
char* writeInteger(char* string, long number, int precision);
char* getScientificNotation(char* string, long double number, int precision,
                            char e, int flagG);
char* writeDouble(char* string, long double number, int precision, int flagG);

void writeChars(char* string, char c, int number);
void addSign(char* string, char sign, int length);
void addPrefix(char* string, char* prefix);
bool ensureNuls(const char* string);
void getAsString(char* line, opt opts, va_list arg, int* sign,
                 const int* success);
long double getAppropriateLengthFloat(opt opts, va_list arg);
long getAppropriateLengthInt(opt opts, va_list arg);
void getG(char* line, opt opts, va_list arg, int* sign);
long getAppropriateLengthUnsignedInt(opt opts, va_list arg);

#endif  // SRC_S21_SPRINTF_H_

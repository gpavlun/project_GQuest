#ifndef LOGGING_H
#define LOGGING_H

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

void info(char *string);
void data(char *string, int number);
void error(int number, char *string);
char *indent(const char *str);
void detail(const char *fmt, ...);


typedef struct logging{
    void (*info)(char *string);
    void (*data)(char *string, int number);
    void (*error)(int number, char *string);
    void (*detail)(const char *, ...);
} logging_t;

extern logging_t logging;


#endif
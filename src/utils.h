#ifndef UTILS_H
#define UTILS_H

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include <pthread.h>

_Noreturn void vdief(char const fmt[], va_list args);
_Noreturn void dief(char const fmt[], ...);
_Noreturn void die(char const msg[]);

void vinformf(char const fmt[], va_list args);
void informf(char const fmt[], ...);
void inform(char const msg[]);

#endif

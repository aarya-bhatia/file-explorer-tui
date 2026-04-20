#pragma once
#include <stdio.h>
#include <string>

#define log_puts(s)                                                            \
  do {                                                                         \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, s);                     \
  } while (0)

#define log_printf(fmt, ...)                                                   \
  do {                                                                         \
    fprintf(stderr, "%s:%d: " fmt "\n", __FILE__, __LINE__, __VA_ARGS__);      \
  } while (0)

enum Colors { Red = 1, Blue, Green, Yellow };

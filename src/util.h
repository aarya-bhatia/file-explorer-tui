#pragma once
#include <stdio.h>
#include <string>
#include <sys/stat.h>

#define log_puts(s)                                                            \
  do {                                                                         \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, s);                     \
  } while (0)

#define log_printf(fmt, ...)                                                   \
  do {                                                                         \
    fprintf(stderr, "%s:%d: " fmt "\n", __FILE__, __LINE__, __VA_ARGS__);      \
  } while (0)

enum Colors { Red = 1, Blue, Green, Yellow };

#define CTRL(c) ((c) & 0x1F)
#define MIN_LINES 20
#define MIN_COLS 50


inline int is_same_file(const char *path1, const char *path2) {
    struct stat stat1, stat2;
    if (stat(path1, &stat1) != 0) return 0;
    if (stat(path2, &stat2) != 0) return 0;

    // Compare device ID and Inode number
    return (stat1.st_dev == stat2.st_dev) && (stat1.st_ino == stat2.st_ino);
}

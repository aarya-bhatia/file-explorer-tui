#pragma once
#include <pwd.h>
#include <string>
#include <sys/stat.h>


#include <filesystem>

namespace fs = std::filesystem;

struct Rect {
  int begy, begx, nlines, ncols;
};

enum Colors { Red = 1, Blue, Green, Yellow };

#define CTRL(c) ((c) & 0x1F)
#define MIN_LINES 3
#define MIN_COLS 20
std::string get_username(uid_t uid);
std::string get_groupname(gid_t gid);
std::string get_human_time(struct timespec &atime);
std::string get_hostname();
std::string get_login_username();

void get_human_size(size_t value, char *buffer, size_t n);
const char *rstrstr(const char *str, const char *substr);

void log_printf(const char *format, ...);

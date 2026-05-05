#pragma once
#include "cwalk.h"
#include "log.h"
#include <pwd.h>
#include <string>
#include <sys/stat.h>

struct Rect {
  int begy, begx, nlines, ncols;
};

enum Colors { Red = 1, Blue, Green, Yellow };

#define CTRL(c) ((c) & 0x1F)
#define MIN_LINES 3
#define MIN_COLS 20

inline int is_same_file(const char *path1, const char *path2) {
  struct stat stat1, stat2;
  if (stat(path1, &stat1) != 0)
    return 0;
  if (stat(path2, &stat2) != 0)
    return 0;

  // Compare device ID and Inode number
  return (stat1.st_dev == stat2.st_dev) && (stat1.st_ino == stat2.st_ino);
}

struct FileStat {
  struct stat s{};
  std::string owner_name = "";
  std::string group_name = "";
  std::string mod_date = "";
  char mode_s[11] = {0};
};

void get_file_stat(const char *filepath, FileStat &st);
std::string get_username(uid_t uid);
std::string get_groupname(gid_t gid);
std::string get_last_access_date(struct timespec &atime);

std::string get_hostname();
std::string get_login_username();

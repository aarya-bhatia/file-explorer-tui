#include "util.h"
#include <array>
#include <cstdarg>
#include <cstdio>
#include <filesystem>
#include <grp.h>
#include <iostream>
#include <limits.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <vector>

namespace fs = std::filesystem;

void log_printf(const char *format, ...) {
  if (strlen(format) == 0)
    return;
  va_list args;
  va_start(args, format);
  vfprintf(stderr, format, args);
  va_end(args);
  if (format[strlen(format) - 1] != '\n')
    fputc('\n', stderr);
}

bool get_system_cwd(std::string &result) {
  try {
    fs::path cwd = fs::current_path();
    std::cout << "Current path is: " << cwd << std::endl;
    result = cwd.string();
    return true;
  } catch (const fs::filesystem_error &e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return false;
  }
}

std::string get_human_time(struct timespec &ts) {
  time_t timestamp = ts.tv_sec;
  struct tm time_info;
  if (localtime_r(&timestamp, &time_info) == NULL) {
    perror("localtime_r");
    return "";
  } else {
    char date_s[80];
    time_t time_now = time(NULL);
    struct tm time_info_now;
    localtime_r(&time_now, &time_info_now);
    if (time_info.tm_year == time_info_now.tm_year)
      strftime(date_s, sizeof date_s, "%b %d %H:%M", &time_info);
    else
      strftime(date_s, sizeof date_s, "%b %d, %Y %H:%M", &time_info);
    return std::string(date_s);
  }
}

std::string get_username(uid_t uid) {

  struct passwd p, *result;
  long bufsize = sysconf(_SC_GETPW_R_SIZE_MAX);
  if (bufsize == -1)
    bufsize = 16384;

  std::vector<char> buf(bufsize);
  getpwuid_r(uid, &p, buf.data(), buf.size(), &result);
  if (!result) {
    return "user";
  }

  std::string username = p.pw_name;
  return username;
}

std::string get_groupname(gid_t gid) {
  struct group grp;
  struct group *g_result;
  long bufsize = sysconf(_SC_GETGR_R_SIZE_MAX);
  if (bufsize == -1)
    bufsize = 16384;
  std::vector<char> gbuf(bufsize);
  getgrgid_r(gid, &grp, gbuf.data(), gbuf.size(), &g_result);
  std::string result;
  if (g_result != NULL) {
    result = std::string(grp.gr_name);
  } else {
    result = "group";
  }
  return result;
}

std::string get_hostname() {
  long bufsize = sysconf(_SC_HOST_NAME_MAX);
  if (bufsize == -1)
    bufsize = 255;
  std::vector<char> buf(bufsize + 1);
  if (gethostname(buf.data(), buf.size()) < 0) {
    perror("gethostname");
  }

  buf.back() = 0;
  return std::string(buf.data());
}

std::string get_login_username() {
  uid_t uid = getuid(); // Get real User ID
  return get_username(uid);
}

void get_human_size(size_t value, char *buffer, size_t n) {
  static const std::array units = {"B", "K", "M", "G", "T"};
  size_t i;
  for (i = 0; value > 1024 && i+1 < std::size(units); i++) {
    value = value / 1024;
  }
  if (value > 1024) {
    snprintf(buffer, n, "inf");
    return;
  }
  snprintf(buffer, n, "%zu%s", value, units[i]);
}

const char *rstrstr(const char *str, const char *substr) {
  const char *s = strstr(str, substr);
  if (!s) {
    return NULL;
  }

  const char *t = NULL;
  while ((t = strstr(s + 1, substr))) {
    s = t;
  }

  return s;
}

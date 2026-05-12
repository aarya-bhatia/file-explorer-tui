#include "util.h"
#include <grp.h>
#include <limits.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <vector>

bool get_system_cwd(std::string &result) {
  char *buf = NULL;
  buf = getcwd(buf, 0);
  if (buf == NULL) {
    perror("getcwd");
    return false;
  }

  result = std::string(buf);
  free(buf);
  return true;
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
  size_t bufsize = sysconf(_SC_GETPW_R_SIZE_MAX);
  if (bufsize == -1)
    bufsize = 16384;

  char *buf = (char *)malloc(bufsize);
  if (buf == NULL) {
    perror("malloc");
    return "user";
  }

  getpwuid_r(uid, &p, buf, bufsize, &result);
  if (!result) {
    free(buf);
    return "user";
  }

  std::string username = p.pw_name;
  free(buf);
  return username;
}

std::string get_groupname(gid_t gid) {
  struct group grp;
  struct group *g_result;
  size_t bufsize = sysconf(_SC_GETGR_R_SIZE_MAX);
  if (bufsize == -1)
    bufsize = 16384;
  char *gbuf = (char *)malloc(bufsize);
  if (gbuf == NULL) {
    perror("malloc");
    return "group";
  }
  getgrgid_r(gid, &grp, gbuf, bufsize, &g_result);
  std::string result;
  if (g_result != NULL) {
    result = std::string(grp.gr_name);
  } else {
    result = "group";
  }
  free(gbuf);
  return result;
}

std::string get_hostname() {
  size_t bufsize = sysconf(_SC_HOST_NAME_MAX);
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
  static const char *units[] = {"B", "K", "M", "G", "T"};
  int unit_index = 0;
  while (value > 1024 && unit_index + 1 < sizeof units / sizeof units[0]) {
    value = value / 1024;
    unit_index++;
  }

  if (value > 1024) {
    snprintf(buffer, n, "inf");
    return;
  }

  snprintf(buffer, n, "%zu%s", value, units[unit_index]);
  buffer[n] = 0;
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

#include "util.h"
#include <errno.h>
#include <grp.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

void get_file_stat(const char *filepath, FileStat &filestat) {
  if (stat(filepath, &filestat.s) < 0) {
    perror("stat");
    return;
  }

  filestat.owner_name = get_username(filestat.s.st_uid);
  filestat.group_name = get_groupname(filestat.s.st_gid);

  mode_t &mode = filestat.s.st_mode;
  if (S_ISDIR(mode)) {
    filestat.mode_s[0] = 'd';
  } else if (S_ISREG(mode)) {
    filestat.mode_s[0] = '-';
  } else {
    filestat.mode_s[0] = '?';
  }

  if (mode & S_IRUSR) {
    filestat.mode_s[1] = 'r';
  } else {
    filestat.mode_s[1] = '-';
  }

  if (mode & S_IWUSR) {
    filestat.mode_s[2] = 'w';
  } else {
    filestat.mode_s[2] = '-';
  }

  if (mode & S_IXUSR) {
    filestat.mode_s[3] = 'x';
  } else {
    filestat.mode_s[3] = '-';
  }

  if (mode & S_IRGRP) {
    filestat.mode_s[4] = 'r';
  } else {
    filestat.mode_s[4] = '-';
  }

  if (mode & S_IWGRP) {
    filestat.mode_s[5] = 'w';
  } else {
    filestat.mode_s[5] = '-';
  }

  if (mode & S_IXGRP) {
    filestat.mode_s[6] = 'x';
  } else {
    filestat.mode_s[6] = '-';
  }

  if (mode & S_IROTH) {
    filestat.mode_s[7] = 'r';
  } else {
    filestat.mode_s[7] = '-';
  }

  if (mode & S_IWOTH) {
    filestat.mode_s[8] = 'w';
  } else {
    filestat.mode_s[8] = '-';
  }

  if (mode & S_IXOTH) {
    filestat.mode_s[9] = 'x';
  } else {
    filestat.mode_s[9] = '-';
  }

  filestat.mod_date = get_last_access_date(filestat.s.st_mtimespec);
}

std::string get_last_access_date(struct timespec &atime) {
  time_t timestamp = atime.tv_sec;
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
      strftime(date_s, sizeof date_s, "%b %d %H:%M:%S", &time_info);
    else
      strftime(date_s, sizeof date_s, "%b %d, %y %H:%M:%S", &time_info);
    return std::string(date_s);
  }
}

std::string get_username(uid_t uid) {

  struct passwd p{};
  size_t bufsize = sysconf(_SC_GETPW_R_SIZE_MAX);
  if (bufsize == -1) /* Value was indeterminate */
    bufsize = 16384; /* Should be more than enough */

  char *buf = (char *)malloc(bufsize);
  if (buf == NULL) {
    perror("malloc");
    return "user";
  }

  struct passwd *result = NULL;
  int s = getpwuid_r(uid, &p, buf, bufsize, &result);
  if (!result) {
    if (s == 0) {
      log_printf("No entry for UID %u", uid);
    } else
      errno = s;
    perror("getpwuid_r");
    free(buf);
    return "user";
  }

  std::string username = p.pw_name;
  log_printf("found username for uid=%d:%s", uid, p.pw_name);
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
    log_printf("Group Name for GID %u is %s", gid, grp.gr_name);
    result = std::string(grp.gr_name);
  } else {
    log_printf("Group name not found for GID %u", gid);
    result = "group";
  }
  free(gbuf);
  return result;
}

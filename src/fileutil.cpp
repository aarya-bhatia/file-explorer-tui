#include "fileutil.h"
#include "include/cwalk.h"
#include "util.h"

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

  filestat.mod_date = get_human_time(filestat.s.st_mtimespec);
}

bool sort_by_name_and_directory(const std::unique_ptr<File> &f1, const std::unique_ptr<File> &f2) {
  if (f1->is_directory() != f2->is_directory()) {
    return f1->is_directory();
  }
  return f1->filename < f2->filename;
}

bool sort_by_filetype(const std::unique_ptr<File> &f1, const std::unique_ptr<File> &f2) {
  const char *ext1, *ext2;
  size_t l1, l2;
  bool has_ext1 = cwk_path_get_extension(f1->filename.c_str(), &ext1, &l1);
  bool has_ext2 = cwk_path_get_extension(f2->filename.c_str(), &ext2, &l2);

  if (has_ext1 != has_ext2) {
    return has_ext1;
  }
  if (has_ext1 && has_ext2) {
    int cmp = strcmp(ext1, ext2);
    if (cmp != 0)
      return cmp < 0;
  }
  return f1->filename < f2->filename;
}

#pragma once

#include <sys/stat.h>
#include <string>

struct File {
  struct stat st;
  std::string filename;
  File(const std::string &cwd, std::string filename);

  bool is_directory() const {
    return S_ISDIR(st.st_mode);
  }

  bool is_file() const {
    return S_ISREG(st.st_mode);
  }

  bool is_link() const { 
    return S_ISLNK(st.st_mode);
  }

  size_t size() const {
    return st.st_size;
  }

  bool operator=(const File &other) {
    return (st.st_dev == other.st.st_dev) && 
      (st.st_ino == other.st.st_ino);
  }
};

#pragma once

#include <sys/stat.h>
#include <string>

struct File {
  struct stat st;
  std::string filename;
  File(const std::string &cwd, std::string filename);
};

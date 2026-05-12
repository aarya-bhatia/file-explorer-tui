#pragma once
#include "file.h"
#include <dirent.h>
#include <memory>
#include <string>
#include <vector>

class Directory {
private:
  DIR *dirp = NULL;
  std::string path;

public:
  Directory(const std::string &_path);
  ~Directory() { closedir(dirp); }
  void list(std::vector<std::unique_ptr<File>> &result);
  bool ok() const { return dirp != NULL; }
};

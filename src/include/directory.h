#pragma once
#include "file.h"
#include <memory>
#include <cassert>
#include <dirent.h>
#include <string>
#include <vector>
class Directory {
private:
  DIR *dirp = NULL;
  std::string path;

public:
  Directory(const std::string &_path) : path(_path) {
    dirp = opendir(path.c_str());
    if (!dirp) {
      log_printf("ERROR opendir(): %s", std::strerror(errno));
    }
  }

  bool ok() const { return dirp != NULL; }

  void list(std::vector<std::unique_ptr<File>> &result) {
    assert(ok());
    rewinddir(dirp);
    struct dirent *e = NULL;
    while ((e = readdir(dirp)) != NULL) {
      if (e->d_name[0] != '.')
        result.emplace_back(std::make_unique<File>(path, std::string(e->d_name)));
    }
  }

  ~Directory() { closedir(dirp); }
};

#pragma once

#include <filesystem>
#include <string>

namespace fs = std::filesystem;

class File {
private:
  fs::directory_entry _entry;

public:
  File(const fs::path &path);

  const fs::path& path() const { return _entry.path(); }
  std::string filename() const { return _entry.path().filename().string(); }

  bool is_directory() const {
    return _entry.is_directory();
  }

  bool is_file() const {
    return _entry.is_regular_file();
  }

  bool is_link() const { 
    return _entry.is_symlink();
  }

  size_t size() const {
    if (is_directory()) return 0;
    try {
        return _entry.file_size();
    } catch (...) {
        return 0;
    }
  }

  bool operator==(const File &other) const {
    return _entry.path() == other._entry.path();
  }
};

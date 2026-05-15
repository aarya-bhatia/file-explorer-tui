#pragma once
#include "file.h"
#include <functional>
#include <memory>
#include <filesystem>

namespace fs = std::filesystem;

using SortStrategy = std::function<bool(const std::unique_ptr<File> &f1,
                                        const std::unique_ptr<File> &f2)>;
using FilterStrategy = std::function<bool(const File &f)>;

struct FileStat {
  fs::file_status status;
  size_t size = 0;
  std::string owner_name = "";
  std::string group_name = "";
  std::string mod_date = "";
  std::string mode_s = "----------";
};

void get_file_stat(const std::unique_ptr<File> &file, FileStat &filestat);

bool sort_by_name_and_directory(const std::unique_ptr<File> &f1,
                                const std::unique_ptr<File> &f2);
bool sort_by_filetype(const std::unique_ptr<File> &f1,
                      const std::unique_ptr<File> &f2);

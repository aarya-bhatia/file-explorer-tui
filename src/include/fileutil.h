#pragma once
#include <filesystem>
#include <functional>

namespace fs = std::filesystem;

using SortStrategy = std::function<bool(const fs::directory_entry &f1,
                                        const fs::directory_entry &f2)>;
using FilterStrategy = std::function<bool(const fs::directory_entry &f)>;

struct FileStat {
  fs::file_status status;
  size_t size = 0;
  std::string owner_name = "";
  std::string group_name = "";
  std::string mod_date = "";
  std::string mode_s = "----------";
};

void get_file_stat(const fs::directory_entry &file, FileStat &filestat);

bool sort_by_name_and_directory(const fs::directory_entry &f1,
                                const fs::directory_entry &f2);

bool sort_by_filetype(const fs::directory_entry &f1,
                      const fs::directory_entry &f2);

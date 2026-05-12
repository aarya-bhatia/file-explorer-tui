#include "file.h"

struct FileStat {
  struct stat s{};
  std::string owner_name = "";
  std::string group_name = "";
  std::string mod_date = "";
  char mode_s[11] = {0};
};

void get_file_stat(const char *filepath, FileStat &st);

bool sort_by_name_and_directory(const std::unique_ptr<File> &f1, const std::unique_ptr<File> &f2);
bool sort_by_filetype(const std::unique_ptr<File> &f1, const std::unique_ptr<File> &f2);


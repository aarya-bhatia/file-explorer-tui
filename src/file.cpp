#include "include/file.h"

File::File(const std::string &cwd, std::string _filename)
    : filename(_filename) {
  std::string filepath = cwd + "/" + _filename;
  memset(&st, 0, sizeof st);
  if (lstat(filepath.c_str(), &st) < 0) {
    perror("stat");
  }
}

#include "app.h"
#include <fcntl.h>
#include <filesystem>
#include <string>
#include <unistd.h>

int main(int argc, const char **argv) {
  namespace fs = std::filesystem;

  const char *LOG_FILENAME = "app.log";
  int logfile = open(LOG_FILENAME, O_CREAT | O_TRUNC | O_WRONLY, 0640);
  dup2(logfile, 2);
  close(logfile);
  setlocale(LC_ALL, "");

  fs::path path;
  if (argc > 1) {
    std::string arg_s = argv[1];
    path = arg_s;
  } else {
    path = fs::current_path();
  }

  if (!fs::exists(path)) {
    exit(1);
  }

  Application app(path);
  app.run();
  return 0;
}

#include "app.h"
#include <fcntl.h>
#include <filesystem>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
namespace fs = std::filesystem;

int main(int argc, const char **argv) {
  const char *LOG_FILENAME = "app.log";
  int logfile = open(LOG_FILENAME, O_CREAT | O_TRUNC | O_WRONLY, 0640);
  dup2(logfile, 2);
  close(logfile);
  setlocale(LC_ALL, "");

  fs::path path;
  if (argc > 1) {
    path = std::string(argv[1]);
  } else {
    path = fs::current_path();
  }

  if(!fs::exists(path)) {
    exit(1);
  }

  Application app(path);
  app.run();
  return 0;
}

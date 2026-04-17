#include "app.h"
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int main() {
  const char *LOG_FILENAME = "app.log";
  int logfile = open(LOG_FILENAME, O_CREAT | O_TRUNC | O_WRONLY, 0640);
  dup2(logfile, 2);
  close(logfile);
  setlocale(LC_ALL, "");
  Application app;
  app.run();
  return 0;
}

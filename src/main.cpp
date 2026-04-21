#include "app.h"
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, const char **argv) {
  const char *LOG_FILENAME = "app.log";
  int logfile = open(LOG_FILENAME, O_CREAT | O_TRUNC | O_WRONLY, 0640);
  dup2(logfile, 2);
  close(logfile);
  setlocale(LC_ALL, "");

  char *abspath = NULL;
  if (argc > 1) {
    abspath = realpath(argv[1], NULL);
    if (!abspath) {
      perror("realpath");
      exit(1);
    }
    log_printf("Start directory: %s", abspath);
  }

  Application app(abspath);
  app.run();
  free(abspath);
  return 0;
}

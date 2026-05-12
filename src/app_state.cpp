#include "include/app_state.h"
#include "include/directory.h"
#include <cstring>
#include <dirent.h>
#include <libgen.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

AppState::AppState(const char *start_cwd) {
  if (start_cwd) {
    cwd = start_cwd;
  } else if (!get_system_cwd(cwd)) {
    log_info("invalid cwd");
    running = false;
    return;
  }
  if (!open_directory(cwd)) {
    running = false;
    return;
  }

  selected_entry = 0;
  log_debug("Initialized AppState with cwd: %s", cwd.c_str());
}

bool AppState::open_directory(const std::string &path) {
  Directory d(path);
  if (!d.ok()) {
    return false;
  }

  cwd = path;

  files.clear();
  log_info("cwd:%s", cwd.c_str());
  d.list(files);
  log_info("Total files: %ld", files.size());
  return true;
}

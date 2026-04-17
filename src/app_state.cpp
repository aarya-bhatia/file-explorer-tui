#include "app_state.h"
#include "util.h"
#include <dirent.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

AppState::AppState() {
  if (!init()) {
    log_printf("%s", "Failed to initialize AppState");
    running = false;
  }
}

bool AppState::update_cwd() {
  char *buf = NULL;
  buf = getcwd(buf, 0);
  if (buf == NULL) {
    perror("getcwd");
    return false;
  }

  cwd = std::string(buf);
  free(buf);
  return true;
}

bool AppState::init() {
  if (!update_cwd())
    return false;
  struct dirent *entry = NULL;
  DIR *dirp = opendir(cwd.c_str());
  if (!dirp) {
    perror("opendir");
    return false;
  }

  log_printf("current working directory is %s", cwd.c_str());

  while ((entry = readdir(dirp)) != NULL) {
    // if(entry->d_type == DT_DIR || entry->d_type == DT_REG)
    log_printf("File: name=%s type=%d", entry->d_name, entry->d_type);
    if (show_dotfiles == false) {
      if (entry->d_namlen > 0 && entry->d_name[0] != '.') {
        files.push_back(std::string(entry->d_name));
      }
    }
  }

  return true;
}

AppState::~AppState() {}

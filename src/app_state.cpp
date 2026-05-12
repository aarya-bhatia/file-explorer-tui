#include "include/app_state.h"
#include "include/cwalk.h"
#include "include/directory.h"
#include <cstring>
#include <dirent.h>
#include <libgen.h>
#include <memory>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

AppState::~AppState() {}

AppState::AppState(const char *_cwd) {
  if (!_cwd) {
    if (!init()) {
      log_info("Failed to initialize AppState");
      running = false;
      return;
    }
  } else {
    cwd = std::string(_cwd);
    if (!reload_file_list()) {
      log_info("Failed to initialize AppState");
      running = false;
      return;
    }
  }

  log_debug("Initialized AppState with cwd: %s", cwd.c_str());
}

bool AppState::reload_file_list() {
  files.clear();

  Directory d(cwd);
  if (!d.ok()) {
    return false;
  }

  log_info("listing files in cwd:%s", cwd.c_str());
  d.list(files);

  log_info("Total files: %ld", files.size());
  return true;
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
  if (!reload_file_list())
    return false;
  selected_entry = 0;
  return true;
}

bool AppState::enter_directory() {
  assert(selected_entry < files.size());
  if (!S_ISDIR(files[selected_entry]->st.st_mode)) {
    return false;
  }
  std::string new_dir = cwd + "/" + get_selected_filename();
  return open_directory(new_dir);
}

bool AppState::open_parent_directory() {
  size_t n = 0;
  cwk_path_get_dirname(cwd.c_str(), &n);
  cwd = cwd.substr(0, n);
  log_debug("changed cwd: %s", cwd.c_str());
  return open_directory(cwd);
}

bool AppState::open_directory(std::string &path) {
  struct stat s;
  memset(&s, 0, sizeof s);
  if (lstat(path.c_str(), &s) < 0) {
    perror("lstat");
    log_info("Failed to open dir: %s", path.c_str());
    return false;
  }
  if (!S_ISDIR(s.st_mode)) {
    log_info("Not a directory: %s", path.c_str());
    return false;
  }
  if (cwd != path)
    cwd = path;
  log_info("changed cwd: %s", cwd.c_str());
  reload_file_list();
  selected_entry = 0;
  return true;
}

void AppState::sort_files() {
  std::sort(files.begin(), files.end(), sort_strategy);
}

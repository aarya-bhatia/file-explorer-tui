#include "include/app_state.h"
#include "include/directory.h"
#include <cstring>
#include <dirent.h>
#include <libgen.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "cwalk.h"

AppState::AppState(const char *start_cwd) {
  if (start_cwd) {
    cwd = start_cwd;
  } else if (!get_system_cwd(cwd)) {
    log_printf("invalid cwd");
    running = false;
    return;
  }
  if (!open_directory(cwd)) {
    running = false;
    return;
  }

  selected_entry = 0;
  log_printf("Initialized AppState with cwd: %s", cwd.c_str());
}

bool AppState::select_prev() {
  if (selected_entry > 0) {
    selected_entry--;
    log_printf("selected %d", selected_entry);
    return true;
  }
  return false;
}

bool AppState::select_next() {
  if (selected_entry + 1 < files.size()) {
    selected_entry++;
    log_printf("selected %d", selected_entry);
    return true;
  }
  return false;
}

  void AppState::select_bottom_entry() {
    selected_entry = bottom_entry_index();
    log_printf("selected %d", selected_entry);
  }

  void AppState::select_top_entry() {
    selected_entry = top_entry_index();
    log_printf("selected %d", selected_entry);
  }

  void AppState::select_middle_entry() {
    int a = top_entry_index();
    int b = bottom_entry_index();
    selected_entry = a + ((b - a) >> 1);
    log_printf("selected %d", selected_entry);
  }

bool AppState::open_parent_directory() {
  size_t n = 0;
  cwk_path_get_dirname(cwd.c_str(), &n);
  cwd = cwd.substr(0, n);
  return open_directory(cwd);
}

std::string AppState::get_selected_filepath() const {
  std::vector<char> buf(cwd.size() + files[selected_entry]->filename.size() +
                        2);
  cwk_path_join(cwd.c_str(), files[selected_entry]->filename.c_str(),
                buf.data(), buf.size());
  return buf.data();
}

bool AppState::open_directory(const std::string &path) {
  Directory d(path);
  if (!d.ok()) {
    return false;
  }

  cwd = path;

  files.clear();
  log_printf("cwd:%s", cwd.c_str());
  d.list(files);
  log_printf("Total files: %ld", files.size());
  return true;
}

void AppState::open_selected_entry() {
  auto &selfile = get_selected_entry();
  if (S_ISDIR(selfile->st.st_mode)) {
    open_selected_directory();
  } else {
    std::string cmd = "open " + get_selected_filepath();
    system(cmd.c_str());
  }
}

void AppState::scroll_down() {
  if (user_scroll + 1 < files.size()) {
    user_scroll++;
    log_printf("scrolled down to %d", user_scroll);
  }
}

void AppState::scroll_up() {
  if (user_scroll > 0) {
    user_scroll--;
    log_printf("scrolled up to %d", user_scroll);
  }
}

void AppState::handle_up_key() {
  if (!select_prev())
    return;

  if (selected_entry == top_entry_index() - 1) {
    scroll_up();
  }
}

void AppState::handle_down_key() {
  if (!select_next())
    return;

  if (selected_entry == 1 + bottom_entry_index()) {
    scroll_down();
  }
}

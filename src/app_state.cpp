#include "include/app_state.h"
#include "include/directory.h"
#include <cstring>
#include <memory>
#include <stdio.h>
#include <unistd.h>

AppState::AppState(const char *start_cwd) {
  fs::path cwd;
  if (start_cwd) {
    cwd = start_cwd;
  } else {
    try {
      cwd = fs::current_path();
    } catch (const fs::filesystem_error &e) {
      log_printf("invalid cwd: %s", e.what());
      running = false;
      return;
    }
  }

  if (!open_directory(cwd)) {
    running = false;
    return;
  }

  log_printf("Initialized AppState with cwd: %s", cwd.c_str());
}

bool AppState::open_directory(const fs::path &path) {
  auto d = std::make_unique<Directory>(path, &file_view_height);
  if (!d->ok()) {
    return false;
  }
  log_printf("Opened directory: %s with %zu files", path.c_str(), d->count_files());
  open_dirs.push(std::move(d));
  return true;
}

void AppState::open_selected_entry() {
  if (count_files() == 0) return;
  auto &dir = cur_dir();
  auto &selfile = dir->get_selected_entry();
  if (selfile->is_directory()) {
    open_selected_directory();
  } else {
    std::string path = dir->get_selected_filepath();
    if (path.empty()) return;
    std::string cmd = "open " + path;
    system(cmd.c_str());
  }
}

void AppState::handle_up_key() {
  auto &dir = cur_dir();
  int old_sel = dir->selected_index();
  if (!dir->select_prev())
    return;

  if (old_sel == dir->top_entry_index()) {
    dir->scroll_up();
  }
}

void AppState::handle_down_key() {
  auto &dir = cur_dir();
  int old_sel = dir->selected_index();
  if (!dir->select_next())
    return;

  if (old_sel == dir->bottom_entry_index()) {
    dir->scroll_down();
  }
}

bool AppState::open_parent_directory() {
  fs::path parent = cur_dir()->get_parent_path();
  if (parent == get_cwd()) return false;
  return open_directory(parent);
}

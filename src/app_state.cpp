#include "include/app_state.h"
#include <cstdlib>
#include <filesystem>
#include <iomanip>
#include <sstream>
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
  if (!fs::exists(path)) {
    return false;
  }
  FileList fl(path, &file_view_height);
  fl.list_files();
  log_printf("Opened directory: %s with %zu files", path.c_str(), fl.size());
  open_dirs.push_back(std::move(fl));
  return true;
}

void AppState::open_selected_entry() {
  if (cur_dir().empty())
    return;
  auto &selfile = cur_dir().selected_file();
  if (selfile.is_directory()) {
    open_selected_directory();
  } else {
    fs::path path = selfile.path();
    std::ostringstream oss;
    oss << std::quoted(path.string());
    std::string path_esc = oss.str();
    std::string cmd = "open " + path_esc;
    system(cmd.c_str());
  }
}

void AppState::handle_up_key() {
  auto &dir = cur_dir();
  dir.select_prev();
  if (dir.selected < dir.top_entry_index()) {
    dir.scroll_up();
  }
}

void AppState::handle_down_key() {
  auto &dir = cur_dir();
  dir.select_next();
  if (dir.selected > dir.bottom_entry_index()) {
    dir.scroll_down();
  }
}

bool AppState::open_parent_directory() {
  fs::path parent = cur_dir().parent_directory();
  if (parent == cur_dir().dirpath)
    return false;
  return open_directory(parent);
}

#pragma once
#include "directory.h"
#include "fileutil.h"
#include "util.h"
#include <assert.h>
#include <stack>
#include <string>
#include <time.h>

struct AppState {
  AppState(const char *cwd = NULL);

  std::stack<std::unique_ptr<Directory>> open_dirs;
  std::unique_ptr<Directory> &cur_dir() {
    assert(!open_dirs.empty());
    return open_dirs.top();
  }
  const std::unique_ptr<Directory> &cur_dir() const {
    assert(!open_dirs.empty());
    return open_dirs.top();
  }
  const std::unique_ptr<File> &get_selected_file() const {
    return cur_dir()->get_selected_entry();
  }
  const std::string &get_selected_filename() const {
    return get_selected_file()->filename;
  }
  std::string get_selected_filepath() const {
    return cur_dir()->get_selected_filepath();
  }
  const std::string &get_cwd() const { return cur_dir()->get_path(); }
  size_t count_files() const { return cur_dir()->count_files(); }
  const std::unique_ptr<File> &get_file(int index) const {
    return cur_dir()->get_file(index);
  }

  bool running = true;
  bool show_dotfiles = false;
  bool show_help_menu = false;
  bool show_preview = false;
  bool statushidden = true;
  bool typing = false;
  int view_height, view_width, file_view_height = 0;
  std::string cmdline_input;
  std::string statusline;
  SortStrategy sort_strategy;
  FilterStrategy filter_strategy;

  int selected_entry() const { return cur_dir()->selected_index(); }
  int user_scroll() const { return cur_dir()->scroll_index(); }

  enum class Mode { Normal, Command, Search } mode = Mode::Normal;

  bool is_typing() const { return typing; }

  Rect get_bounds() const {
    return Rect{
        .begy = 0, .begx = 0, .nlines = view_height, .ncols = view_width};
  }

  bool open_directory(const std::string &path);
  void quit() { running = false; }
  void reload() { cur_dir()->reload(); }
  void toggle_show_status() { statushidden = !statushidden; }
  void toggle_show_help() { show_help_menu = !show_help_menu; }
  void handle_up_key();
  void handle_down_key();

  bool open_selected_directory() {
    return open_directory(cur_dir()->get_selected_filepath());
  }

  void open_selected_entry();

  bool open_parent_directory();
};

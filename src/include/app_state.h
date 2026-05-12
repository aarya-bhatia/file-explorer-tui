#pragma once
#include "file.h"
#include "util.h"
#include <assert.h>
#include <functional>
#include <string>
#include <time.h>
#include <vector>

struct AppState {
  using SortStrategy = std::function<bool(const std::unique_ptr<File> &f1,
                                          const std::unique_ptr<File> &f2)>;
  using FilterStrategy = std::function<bool(const File &f)>;

  AppState(const char *cwd = NULL);

  bool running = true;
  bool show_dotfiles = false;
  bool show_help_menu = false;
  bool show_preview = false;
  bool statushidden = true;
  bool typing = false;
  int selected_entry = 0;
  int user_scroll = 0;
  int view_height, view_width, file_view_height = 0;
  std::string cmdline_input;
  std::string cwd;
  std::string statusline;
  std::vector<std::unique_ptr<File>> files;
  SortStrategy sort_strategy;
  FilterStrategy filter_strategy;

  enum class Mode { Normal, Command, Search } mode = Mode::Normal;

  bool is_typing() const { return typing; }

  Rect get_bounds() const {
    return Rect{
        .begy = 0, .begx = 0, .nlines = view_height, .ncols = view_width};
  }

  bool select_prev();
  bool select_next();
  void select_bottom_entry();
  void select_top_entry();
  void select_middle_entry();
  void open_selected_entry();
  bool open_parent_directory();
  bool open_directory(const std::string &path);
  void scroll_up();
  void scroll_down();
  void handle_up_key();
  void handle_down_key();
  void quit() { running = false; }
  void reload() { open_directory(cwd); }
  void toggle_show_status() { statushidden = !statushidden; }
  void toggle_show_help() { show_help_menu = !show_help_menu; }

  const std::unique_ptr<File> &get_selected_entry() const {
    return files[selected_entry];
  }

  const std::string &get_selected_filename() const {
    return files[selected_entry]->filename;
  }

  std::string get_selected_filepath() const;

  int top_entry_index() { return user_scroll; }

  int bottom_entry_index() {
    return std::min<int>(user_scroll + file_view_height - 1, files.size() - 1);
  }

  bool is_entry_visible(int index) {
    return index >= top_entry_index() && index <= bottom_entry_index();
  }

  bool open_selected_directory() {
    return open_directory(get_selected_filepath());
  }

  void sort_files() { std::sort(files.begin(), files.end(), sort_strategy); }
  void find_and_select(const std::string &filepath);
};

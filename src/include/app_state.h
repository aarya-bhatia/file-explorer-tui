#pragma once
#include "cwalk.h"
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

  bool init();

  Rect get_bounds() {
    return Rect{
        .begy = 0, .begx = 0, .nlines = view_height, .ncols = view_width};
  }

  bool select_prev() {
    if (selected_entry > 0) {
      selected_entry--;
      log_printf("selected %d", selected_entry);
      return true;
    }
    return false;
  }

  bool select_next() {
    if (selected_entry + 1 < files.size()) {
      selected_entry++;
      log_printf("selected %d", selected_entry);
      return true;
    }
    return false;
  }

  bool open_parent_directory() {
    size_t n = 0;
    cwk_path_get_dirname(cwd.c_str(), &n);
    cwd = cwd.substr(0, n);
    return open_directory(cwd);
  }

  bool open_directory(const std::string &path);

  const std::unique_ptr<File> &get_selected_entry() const {
    return files[selected_entry];
  }

  const std::string &get_selected_filename() const {
    return files[selected_entry]->filename;
  }

  std::string get_selected_filepath() const {
    std::vector<char> buf(cwd.size() + files[selected_entry]->filename.size() +
                          2);
    cwk_path_join(cwd.c_str(), files[selected_entry]->filename.c_str(),
                  buf.data(), buf.size());
    return buf.data();
  }

  void find_and_select(const std::string &filepath) {
    // TODO
  }

  int top_entry_index() { return user_scroll; }

  int bottom_entry_index() {
    return std::min<int>(user_scroll + file_view_height - 1, files.size() - 1);
  }

  bool is_entry_visible(int index) {
    return index >= top_entry_index() && index <= bottom_entry_index();
  }

  void scroll_down() {
    if (user_scroll + 1 < files.size()) {
      user_scroll++;
      log_printf("scrolled down to %d", user_scroll);
    }
  }

  void scroll_up() {
    if (user_scroll > 0) {
      user_scroll--;
      log_printf("scrolled up to %d", user_scroll);
    }
  }

  void resize(int lines, int cols) {
    view_width = cols;
    view_height = lines;
  }

  void toggle_show_status() { statushidden = !statushidden; }

  void toggle_show_help() { show_help_menu = !show_help_menu; }

  void quit() { running = false; }

  void select_bottom_entry() {
    selected_entry = bottom_entry_index();
    log_printf("selected %d", selected_entry);
  }

  void select_top_entry() {
    selected_entry = top_entry_index();
    log_printf("selected %d", selected_entry);
  }

  void select_middle_entry() {
    selected_entry = (bottom_entry_index() - top_entry_index()) / 2;
    log_printf("selected %d", selected_entry);
  }

  void handle_up_key() {
    if (!select_prev())
      return;

    if (selected_entry == top_entry_index() - 1) {
      scroll_up();
    }
  }

  void handle_down_key() {
    if (!select_next())
      return;

    if (selected_entry == 1 + bottom_entry_index()) {
      scroll_down();
    }
  }

  void reload() { open_directory(cwd); }

  void open_selected_entry() {
    auto &selfile = get_selected_entry();
    if (S_ISDIR(selfile->st.st_mode)) {
      open_selected_directory();
    } else {
      std::string cmd = "open " + get_selected_filepath();
      system(cmd.c_str());
    }
  }

  bool open_selected_directory() {
    return open_directory(get_selected_filepath());
  }

  void sort_files() { std::sort(files.begin(), files.end(), sort_strategy); }
};

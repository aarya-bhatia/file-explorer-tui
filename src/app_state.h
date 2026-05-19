#pragma once
#include "filelist.h"
#include "fileutil.h"
#include "util.h"
#include <assert.h>
#include <map>
#include <string>
#include <time.h>

struct AppState {
  AppState(const char *cwd = NULL);

  std::map<std::string, FileList> fl_cache;
  std::vector<FileList> splits;
  int focused_split_id;

  void split_exists(int splitid) {
    assert(splitid >= 0);
    assert(splitid < (int)splits.size());
  }

  void focus_split(int splitid) { split_exists(splitid); 
    // TODO implement this
    auto &dirname = get_selection(splitid);
    // TODO if no change return
    for(int i = splitid; i < splits.size(); i++) {
      // TODO implement this
      // if previous split has any focused dir, load the subdir in next split
      // for new split, auto-sort by dir and focus on the first dir by default.
    }
  }

  std::vector<FileList> open_dirs;

  FileList &cur_dir() {
    assert(!open_dirs.empty());
    return open_dirs.back();
  }

  const FileList &cur_dir() const {
    assert(!open_dirs.empty());
    return open_dirs.back();
  }

  const fs::directory_entry &get_selected_file() const {
    return cur_dir().selected_file();
  }

  std::string get_selected_filename() const {
    return cur_dir().selected_file().path().filename().string();
  }

  fs::path get_selected_filepath() const {
    return cur_dir().selected_file().path();
  }

  const fs::path &get_cwd() const { return cur_dir().dirpath; }
  size_t count_files() const { return cur_dir().size(); }
  const fs::directory_entry &get_file(int index) const {
    return cur_dir().at(index);
  }

  int selected_entry() const { return cur_dir().selected; }
  int user_scroll() const { return cur_dir().scroll; }

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

  enum class Mode { Normal, Command, Search } mode = Mode::Normal;

  bool is_typing() const { return typing; }

  Rect get_bounds() const {
    return Rect{
        .begy = 0, .begx = 0, .nlines = view_height, .ncols = view_width};
  }

  bool open_directory(const fs::path &path);
  void quit() { running = false; }
  void toggle_show_status() { statushidden = !statushidden; }
  void toggle_show_help() { show_help_menu = !show_help_menu; }
  void handle_up_key();
  void handle_down_key();

  bool open_selected_directory() {
    return open_directory(cur_dir().selected_file());
  }

  void open_selected_entry();

  bool open_parent_directory();
};

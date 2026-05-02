#pragma once
#include "util.h"
#include <assert.h>
#include <map>
#include <string>
#include <time.h>
#include <vector>

#define DATA_DIRECTORY "data/"

struct AppState {
  struct FileEntry {
    enum EntryType { File, Directory, Symlink, Other } type = File;
    std::string filename = "";

    FileEntry(EntryType _type, const std::string &_filename)
        : type(_type), filename(_filename) {}
  };

  AppState(const char *cwd = NULL);
  ~AppState();

  int user_scroll = 0;

  int view_height, view_width;
  int file_view_height;

  bool running = true;
  bool show_dotfiles = false;
  bool show_help_menu = false;

  bool typing = false;
  const char *prompt = "";
  std::string cmdline_input = "";
  std::string statusline =
      "Type o to create file/directory. Toggle statusline with CTRL-l";
  bool statushidden = false;

  bool init();

  std::vector<std::unique_ptr<FileEntry>> files;
  std::string cwd;
  int selected_entry = 0;

  bool update_cwd();

  bool select_prev() {
    if (selected_entry > 0) {
      selected_entry--;
      return true;
    }
    return false;
  }

  bool select_next() {
    if (selected_entry + 1 < files.size()) {
      selected_entry++;
      return true;
    }
    return false;
  }

  bool reload_file_list();
  bool enter_directory();
  bool open_parent_directory();
  bool open_directory(std::string &path);

  const std::unique_ptr<AppState::FileEntry> &get_selected_entry() const {
    return files[selected_entry];
  }

  const std::string &get_selected_filename() const {
    return files[selected_entry]->filename;
  }

  void find_and_select(const std::string &filepath) {
    for (int i = 0; i < files.size(); i++) {
      if (is_same_file(files[i]->filename.c_str(), filepath.c_str())) {
        selected_entry = i;
        break;
      }
    }
  }

  int window_top_file_index() { return user_scroll; }

  int window_bottom_file_index() { return user_scroll + file_view_height - 1; }

  int window_count_visible_files() { return files.size() - user_scroll; }

  void window_select_bottom_file() {
    selected_entry = window_bottom_file_index();
  }

  void window_select_top_file() { selected_entry = window_top_file_index(); }

  bool window_is_file_visible(int index) {
    return index >= window_top_file_index() &&
           index <= window_bottom_file_index();
  }

  void scroll_down() { 
    if (window_count_visible_files() < file_view_height) {
      return;
    }

    assert(user_scroll < files.size());
    user_scroll++;
  }

  void scroll_up() { 
    if(user_scroll > 0) user_scroll--; 
  }

  void resize(int lines, int cols) {
    view_width = cols;
    view_height = lines;
    file_view_height = lines - 3;
  }
};

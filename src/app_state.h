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
};

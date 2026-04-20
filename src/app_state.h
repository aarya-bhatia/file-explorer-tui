#pragma once
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

  AppState();
  ~AppState();

  bool running = true;
  bool show_dotfiles = false;
  bool show_help_menu = false;

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
};

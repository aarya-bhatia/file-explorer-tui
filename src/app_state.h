#pragma once
#include <map>
#include <string>
#include <vector>
#include <time.h>

#define DATA_DIRECTORY "data/"

struct AppState {
  struct FileEntry {
    enum EntryType {File, Directory, Symlink, Other} type = File;
    std::string filename = "";
  };

  AppState();
  ~AppState();
  bool running = true;
  bool show_dotfiles = false;

  bool init();

  std::vector<FileEntry> files;
  std::string cwd;
  int selected_entry = 0;


  bool update_cwd();
};

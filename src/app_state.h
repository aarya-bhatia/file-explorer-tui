#pragma once
#include <map>
#include <string>
#include <vector>
#include <time.h>

#define DATA_DIRECTORY "data/"

struct AppState {
  AppState();
  ~AppState();
  bool running = true;
  bool show_dotfiles = false;

  bool init();

  std::vector<std::string> files;
  std::string cwd;

  bool update_cwd();
};

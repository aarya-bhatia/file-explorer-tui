#pragma once
#include "app_state.h"
#include "filelist.h"
#include "fileutil.h"
#include "split_controller.h"
#include "util.h"
#include <assert.h>
#include <cstdlib>
#include <map>
#include <stdio.h>
#include <string>
#include <time.h>
#include <unistd.h>

struct AppState {
  AppState(const fs::path &path, int num_split) : split_controller(num_split) {
    split_controller.set_root_directory(path);
    log_printf("Initialized AppState with cwd: %s", path.c_str());
  }

  void open_selected_entry() {
    return;
    // if (cur_dir().empty())
    //   return;
    // auto &selfile = cur_dir().selected_file();
    // if (selfile.is_directory()) {
    //   open_selected_directory();
    // } else {
    //   fs::path path = selfile.path();
    //   std::ostringstream oss;
    //   oss << std::quoted(path.string());
    //   std::string path_esc = oss.str();
    //   std::string cmd = "open " + path_esc;
    //   system(cmd.c_str());
    // }
  }

  SplitController split_controller;

};

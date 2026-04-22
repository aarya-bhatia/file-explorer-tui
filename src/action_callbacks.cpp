#include "action_callbacks.h"
#include "util.h"
#include <fcntl.h>
#include <ncurses.h>
#include <sys/stat.h>
#include <unistd.h>

bool CreateFileCallback::run(AppState &state) {
  log_printf("Started CreateFileCallback with input:%s",
             state.cmdline_input.c_str());
  std::string filepath = state.cmdline_input;
  if (filepath.empty()) {
    state.statusline = "";
    return true;
  }

  if (filepath[0] != '/') {
    filepath = state.cwd + "/" + filepath;
  }

  struct stat st;
  memset(&st, 0, sizeof st);
  if (stat(filepath.c_str(), &st) >= 0) {
    log_printf("file already exists %s", filepath.c_str());
    state.statusline = "file already exists " + filepath;
    return true;
  }

  if (filepath.back() == '/') {
    log_printf("creating directory %s", filepath.c_str());
    if (mkdir(filepath.c_str(), 0777) < 0) {
      perror("mkdir");
      state.statusline = "Failed to create directory " + filepath;
      state.find_and_select(filepath);
    } else {
      state.reload_file_list();
      state.statusline = "successfully created directory " + filepath;
    }
  } else {
    log_printf("creating file %s", filepath.c_str());
    if (creat(filepath.c_str(), 0666) < 0) {
      perror("creat");
      state.statusline = "Failed to create file " + filepath;
    } else {
      state.reload_file_list();
      state.find_and_select(filepath);
      state.statusline = "successfully created file " + filepath;
    }
  }
  return true;
}


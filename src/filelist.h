#pragma once
#include "fileutil.h"
#include "util.h"
#include "viewutil.h"
#include <cassert>
#include <filesystem>
#include <ncurses.h>
#include <string>
#include <vector>

namespace fs = std::filesystem;

struct FileList {
  fs::path dirpath;
  std::vector<fs::directory_entry> files;
  int selected = 0;
  int scroll = 0;
  int height = 0;

  FileList(const fs::path &path, int h) : dirpath(path), height(h) {
    list_files();
    if (!files.empty()) {
      sort_files(sort_by_name_and_directory);
    }
    log_printf("opened directory:%s with %zu files", path.c_str(),
               files.size());
  }

  const fs::directory_entry &at(int i) const { return files[i]; }
  size_t size() const { return files.size(); }
  bool empty() const { return files.empty(); }

  void set_height(int h) { height = h; }

  const fs::directory_entry &selected_file() const {
    assert(!files.empty());
    return files[selected];
  }

  void list_files() {
    files.clear();
    try {
      for (const auto &entry : fs::directory_iterator(dirpath)) {
        if (entry.path().filename().string()[0] != '.') {
          files.emplace_back(entry);
        }
      }
    } catch (const fs::filesystem_error &e) {
      log_printf("ERROR listing directory: %s", e.what());
    }

    if (selected >= (int)files.size()) {
      selected = files.empty() ? 0 : (int)files.size() - 1;
    }
  }

  int top_entry_index() const { return scroll; }

  int bottom_entry_index() const {
    return std::min<int>(scroll + height - 1, (int)files.size() - 1);
  }

  bool is_entry_visible(int index) const {
    return index >= top_entry_index() && index <= bottom_entry_index();
  }

  void sort_files(SortStrategy sort_strategy) {
    std::sort(files.begin(), files.end(), sort_strategy);
  }

  void filter_files(FilterStrategy filter_strategy) {
    files.erase(std::remove_if(files.begin(), files.end(), filter_strategy),
                files.end());
    if (files.empty())
      selected = 0;
    else if (selected >= (int)files.size()) {
      selected = (int)files.size() - 1;
    }
  }

  void select_prev() {
    if (files.empty()) {
      selected = 0;
      return;
    }
    selected = std::max<int>(0, selected - 1);
  }

  void select_next() {
    if (files.empty()) {
      selected = 0;
      return;
    }
    selected = std::min<int>(selected + 1, (int)files.size() - 1);
  }

  void select_first() { selected = 0; }

  void select_last() { selected = std::max<int>(0, (int)files.size() - 1); }

  void select_top() {
    if (files.empty()) {
      selected = 0;
      return;
    }
    selected = top_entry_index();
  }

  void select_bottom() {
    if (files.empty()) {
      selected = 0;
      return;
    }
    selected = bottom_entry_index();
  }

  void select_middle() {
    if (files.empty()) {
      selected = 0;
      return;
    }
    int a = top_entry_index();
    int b = bottom_entry_index();
    selected = a + ((b - a) >> 1);
    selected = std::min<int>(selected, (int)files.size() - 1);
  }

  fs::path parent_directory() const { return dirpath.parent_path(); }

  void scroll_up() { scroll = std::max<int>(0, scroll - 1); }
  void scroll_down() {
    if (files.empty())
      return;
    scroll = std::min<int>(scroll + 1, (int)files.size() - 1);
  }

  bool find_and_select(const fs::path &filepath) {
    for (int i = 0; i < (int)files.size(); i++) {
      if (files[i].path() == filepath) {
        selected = i;
        return true;
      }
    }

    return false;
  }

  void adjust_scroll() {
    if (selected < top_entry_index()) {
      scroll = selected;
    } else if (selected > bottom_entry_index()) {
      scroll = selected - height + 1;
    }
  }

  void render(WINDOW *window) {
    draw_list(window, files, scroll, selected);
    wnoutrefresh(window);
  }
};

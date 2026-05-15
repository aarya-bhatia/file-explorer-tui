#include "include/directory.h"
#include "include/file.h"
#include "include/util.h"
#include <cassert>
#include <filesystem>
#include <memory>
#include <string>
#include <vector>

namespace fs = std::filesystem;

Directory::Directory(const fs::path &path, int *height)
    : _dirpath(path), _scroll(0), _height(height) {
  if (ok()) {
    _list();
  }
}

void Directory::_list() {
  _files.clear();
  try {
    for (const auto &entry : fs::directory_iterator(_dirpath)) {
      if (entry.path().filename().string()[0] != '.') {
        _files.emplace_back(std::make_unique<File>(entry.path()));
      }
    }
  } catch (const fs::filesystem_error &e) {
    log_printf("ERROR listing directory: %s", e.what());
  }

  if (_selected >= (int)_files.size()) {
    _selected = _files.empty() ? 0 : (int)_files.size() - 1;
  }
}

bool Directory::select_prev() {
  if (_selected > 0) {
    _selected--;
    log_printf("selected %d", _selected);
    return true;
  }
  return false;
}

bool Directory::select_next() {
  if (_selected + 1 < (int)_files.size()) {
    _selected++;
    log_printf("selected %d", _selected);
    return true;
  }
  return false;
}

void Directory::select_bottom_entry() {
  if (_files.empty()) {
    _selected = 0;
    return;
  }
  _selected = bottom_entry_index();
  log_printf("selected %d", _selected);
}

void Directory::select_top_entry() {
  if (_files.empty()) {
    _selected = 0;
    return;
  }
  _selected = top_entry_index();
  log_printf("selected %d", _selected);
}

void Directory::select_middle_entry() {
  if (_files.empty()) {
    _selected = 0;
    return;
  }
  int a = top_entry_index();
  int b = bottom_entry_index();
  _selected = a + ((b - a) >> 1);
  if (_selected >= (int)_files.size())
    _selected = (int)_files.size() - 1;
  log_printf("selected %d", _selected);
}

const fs::path& Directory::get_selected_filepath() const {
  static const fs::path empty = "";
  if (_files.empty() || _selected >= (int)_files.size()) {
    return empty;
  }
  return _files[_selected]->path();
}

void Directory::scroll_down() {
  if (_scroll + 1 < (int)_files.size()) {
    _scroll++;
    log_printf("scrolled down to %d", _scroll);
  }
}

void Directory::scroll_up() {
  if (_scroll > 0) {
    _scroll--;
    log_printf("scrolled up to %d", _scroll);
  }
}

fs::path Directory::get_parent_path() const {
  return _dirpath.parent_path();
}

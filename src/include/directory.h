#pragma once
#include "file.h"
#include "fileutil.h"
#include <filesystem>
#include <memory>
#include <string>
#include <vector>

namespace fs = std::filesystem;

class Directory {
private:
  fs::path _dirpath;
  std::vector<std::unique_ptr<File>> _files;
  int _selected = 0;
  int _scroll = 0;
  int *_height = NULL;

  void _list();

public:
  Directory(const fs::path &path, int *height);
  ~Directory() = default;
  bool ok() const { return fs::exists(_dirpath) && fs::is_directory(_dirpath); }
  void reload() { _list(); }
  size_t count_files() const { return _files.size(); }

  std::unique_ptr<File> &get_selected_entry() { 
    static std::unique_ptr<File> dummy = nullptr;
    if (_files.empty()) return dummy;
    return _files[_selected]; 
  }

  bool select_prev();
  bool select_next();
  void select_top_entry();
  void select_middle_entry();
  void select_bottom_entry();
  const fs::path& get_path() const { return _dirpath; }
  const std::unique_ptr<File> &get_file(int i) const { return _files[i]; }

  const fs::path& get_selected_filepath() const;
  fs::path get_parent_path() const;

  void set_selected(int s) { _selected = s; }
  void set_scroll(int s) { _scroll = s; }

  int selected_index() const { return _selected; }
  int scroll_index() const { return _scroll; }
  int top_entry_index() const { return _scroll; }

  int bottom_entry_index() const {
    if (!_height) return _scroll;
    return std::min<int>(_scroll + *_height - 1, (int)_files.size() - 1);
  }

  bool is_entry_visible(int index) const {
    return index >= top_entry_index() && index <= bottom_entry_index();
  }

  void sort_files(SortStrategy sort_strategy) {
    std::sort(_files.begin(), _files.end(), sort_strategy);
  }

  void scroll_up();
  void scroll_down();

  void find_and_select(const std::string &filepath);

};

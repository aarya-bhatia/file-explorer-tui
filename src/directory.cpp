#include "directory.h"
#include "file.h"
#include "util.h"
#include <cassert>
#include <dirent.h>
#include <libgen.h>
#include <memory>
#include <string>
#include <vector>

Directory::Directory(const std::string &_path, int *height)
    : _dirpath(_path), _height(height), _scroll(0) {
  _dirp = opendir(_dirpath.c_str());
  if (!_dirp) {
    log_printf("ERROR opendir(): %s", std::strerror(errno));
  } else {
    _list();
  }
}

void Directory::_list() {
  _files.clear();
  assert(ok());
  rewinddir(_dirp);
  struct dirent *e = NULL;
  while ((e = readdir(_dirp)) != NULL) {
    if (e->d_name[0] != '.')
      _files.emplace_back(
          std::make_unique<File>(_dirpath, std::string(e->d_name)));
  }
  if (_selected >= _files.size()) {
    _selected = _files.empty() ? 0 : _files.size() - 1;
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
  _selected = bottom_entry_index();
  log_printf("selected %d", _selected);
}

void Directory::select_top_entry() {
  _selected = top_entry_index();
  log_printf("selected %d", _selected);
}

void Directory::select_middle_entry() {
  int a = top_entry_index();
  int b = bottom_entry_index();
  _selected = a + ((b - a) >> 1);
  log_printf("selected %d", _selected);
}

std::string Directory::get_selected_filepath() const {
  if (_files.empty() || _selected >= _files.size()) {
    return "";
  }
  std::string filepath = _dirpath + "/" + _files[_selected]->filename;
  char *s = realpath(filepath.c_str(), NULL);
  if (s) {
    filepath = std::string(s);
    free(s);
  }
  return filepath;
}

void Directory::scroll_down() {
  if (_scroll + 1 < _files.size()) {
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

std::string Directory::get_parent_path() const {
  std::vector<char> path_copy{_dirpath.begin(), _dirpath.end()};
  path_copy.push_back(0);
  char *dname = dirname(path_copy.data());
  return std::string(dname);
}

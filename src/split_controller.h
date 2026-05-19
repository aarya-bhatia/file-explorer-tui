#pragma once

#include "filelist.h"
#include "viewutil.h"
#include <filesystem>
#include <map>
#include <ncurses.h>

struct SplitController {
  std::map<fs::path, fs::path> _last_selected;
  std::vector<FileList *> _splits;
  int _cur = 0;
  Rect bounds;
  std::vector<WINDOW *> _views;

  bool is_current_split_empty() const {
    return !_splits[_cur] || _splits[_cur]->empty();
  }

  unsigned count_files_in_current_split() const {
    return is_current_split_empty() ? 0 : _splits[_cur]->size();
  }

  const FileList *get_current_split() const { return _splits[_cur]; }

  fs::path get_dirname_in_current_split() const {
    if (is_current_split_empty())
      return fs::path();
    return _splits[_cur]->dirpath;
  }

  const fs::path &get_selected_file_in_current_split() const {
    assert(!is_current_split_empty());
    return _splits[_cur]->selected_file().path();
  }

  const fs::directory_entry &get_selected_entry_in_current_split() const {
    assert(!is_current_split_empty());
    return _splits[_cur]->selected_file();
  }

  std::string root_path() const {
    return _splits[0] ? _splits[0]->dirpath.string() : "";
  }

  void set_root_path(fs::path path) { set_split_path(0, path); }

  void sort(SortStrategy strategy) {
    if (_splits[_cur] == NULL)
      return;
    _splits[_cur]->sort_files(strategy);
    _splits[_cur]->adjust_scroll();
  }

  SplitController(int num_split = 0) : _splits(num_split) {
    for (int i = 0; i < num_split; i++) {
      _splits[i] = NULL;
    }
  }

  ~SplitController() {
    for (FileList *split : _splits)
      delete split;
    _splits.clear();
  }

  void resize(Rect r) {
    bounds = r;
    for (int i = 0; i < (int)_splits.size(); i++) {
      if (_splits[i] == NULL)
        break;
      _splits[i]->set_height(r.nlines);
      _splits[i]->adjust_scroll();
    }

    for (WINDOW *window : _views)
      delwin(window);
    _views.clear();
    if (_splits.empty())
      return;
    int splitw = r.ncols / _splits.size();
    for (int i = 0; i < (int)_splits.size(); i++) {
      int xoffset = i * splitw;
      int w = splitw;
      if (i == (int)_splits.size() - 1) {
        w = r.ncols - xoffset;
      }
      WINDOW *view = newwin(r.nlines, w, r.begy, r.begx + xoffset);
      wrefresh(view);
      _views.push_back(view);
    }
  }

  void render() {
    assert(_views.size() == _splits.size());
    for (int i = 0; i < (int)_splits.size(); i++) {
      werase(_views[i]);
      wnoutrefresh(_views[i]);
      if (_splits[i] == NULL)
        continue;
      else
        _splits[i]->render(_views[i]);
    }
  }

  void set_split_path(int splitid, const fs::path &path) {
    if (_splits[splitid]) {
      delete _splits[splitid];
    }
    _splits[splitid] = new FileList(path, bounds.nlines);
    select_last_selected_file(splitid);
    _update_split(splitid + 1);
  }

  void _update_split(int splitid) {
    if (splitid >= _splits.size())
      return;

    if (splitid > 0) {
      const FileList *parent_split = _splits[splitid - 1];
      if (!parent_split || parent_split->empty() ||
          !parent_split->selected_file().is_directory()) {
        delete _splits[splitid];
        _splits[splitid] = NULL;
      } else {
        fs::path expected_path = parent_split->selected_file().path();
        if (!_splits[splitid] || expected_path != _splits[splitid]->dirpath) {
          set_split_path(splitid, expected_path);
        }
      }

      if (_splits[splitid]) {
        _splits[splitid]->adjust_scroll();
      }
    }

    if (splitid + 1 < _splits.size()) {
      _update_split(splitid + 1);
    }
  }

  void select_last_selected_file(int splitid) {
    FileList *split = _splits[splitid];
    if (!split)
      return;
    if (split->empty())
      return;
    const fs::path &last_selected_file = _last_selected[split->dirpath];
    if (split->find_and_select(last_selected_file)) {
      _splits[splitid]->adjust_scroll();
    } else {
      _last_selected[split->dirpath] = split->selected_file();
    }
  }

  void update_history() {
    FileList *list = _splits[_cur];
    if (list->empty())
      return;
    const fs::path &path = _splits[_cur]->dirpath;
    _last_selected[path] = _splits[_cur]->selected_file();
  }

  void move_left() {
    if (!_splits[_cur])
      return;

    if (_cur == 0) {
      fs::path new_root = _splits[_cur]->parent_directory();
      if (new_root != _splits[0]->dirpath)
        set_split_path(0, new_root);
    } else {
      _cur--;
    }
  }

  void move_right() {
    if (!_splits[_cur])
      return;
    if (_cur == (int)_splits.size() - 1) {
      set_split_path(0, _splits[1]->dirpath);
    } else {
      _cur++;
    }
  }

  void move_down() {
    if (!_splits[_cur])
      return;
    _splits[_cur]->select_next();
    _splits[_cur]->adjust_scroll();
    update_history();
    _update_split(_cur + 1);
  }

  void move_up() {
    if (!_splits[_cur])
      return;
    _splits[_cur]->select_prev();
    _splits[_cur]->adjust_scroll();
    update_history();
    _update_split(_cur + 1);
  }
};

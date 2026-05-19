#pragma once

#include "filelist.h"
#include "viewutil.h"
#include <filesystem>
#include <map>
#include <ncurses.h>

struct SplitController {
  std::map<fs::path, fs::path> last_selected;
  std::vector<FileList *> splits;
  int cur_split = 0;
  Rect bounds;

  std::vector<WINDOW *> view_cols;

  std::string root_path() const {
    return splits[0] ? splits[0]->dirpath.string() : "";
  }

  void sort(SortStrategy strategy) {
    if (splits[cur_split] == NULL)
      return;
    splits[cur_split]->sort_files(strategy);
    splits[cur_split]->adjust_scroll();
  }

  SplitController(int num_split=0) : splits(num_split) {}

  ~SplitController() {
    for (FileList *split : splits)
      delete split;
  }

  void clean_orphan_splits() {
    for (int i = 0; i < splits.size(); i++) {
      if (splits[i] != NULL && i > 0 && splits[i - 1] == NULL) {
        delete splits[i];
        splits[i] = NULL;
      }
    }
  }

  void resize(Rect r) {
    bounds = r;
    for (int i = 0; i < splits.size(); i++) {
      if (splits[i] == NULL)
        break;
      splits[i]->set_height(r.nlines);
      splits[i]->adjust_scroll();
    }

    for (WINDOW *window : view_cols)
      delwin(window);
    view_cols.clear();
    if (splits.empty())
      return;
    int splitw = r.ncols / splits.size();
    for (int i = 0; i < splits.size(); i++) {
      int xoffset = i * splitw;
      int w = splitw;
      if (i == splits.size() - 1) {
        w = r.ncols - xoffset;
      }
      WINDOW *view = newwin(r.nlines, splitw, r.begy, r.begx + xoffset);
      wrefresh(view);
      view_cols.push_back(view);
    }
  }

  void render() {
    for (int i = 0; i < splits.size(); i++) {
      if (splits[i] == NULL)
        break;
      splits[i]->render(view_cols[i]);
    }
  }

  void set_root_directory(const fs::path &path) {
    FileList *fl = new FileList(path, bounds.nlines);
    fl->list_files();
    splits[0] = fl;
    update_splits();
  }

  void update_history() {
    last_selected[splits[cur_split]->dirpath] =
        splits[cur_split]->selected_file();
  }

  void move_left() {
    if (!splits[cur_split])
      return;
    if (cur_split == 0) {
      fs::path new_root = splits[cur_split]->parent_directory();
      clear_splits(0);
      set_root_directory(new_root);
      update_splits();
    }
    cur_split = std::max(cur_split - 1, 0);
  }

  void move_right() {
    if (!splits[cur_split])
      return;
    cur_split = std::min(cur_split + 1, (int)splits.size() - 1);
  }

  void move_down() {
    if (!splits[cur_split])
      return;
    splits[cur_split]->select_next();
    splits[cur_split]->adjust_scroll();
    update_history();
    update_splits();
  }

  void move_up() {
    if (!splits[cur_split])
      return;
    splits[cur_split]->select_prev();
    splits[cur_split]->adjust_scroll();
    update_history();
    update_splits();
  }

  void update_splits() {
    if (splits[0] == NULL)
      return;
    for (int i = 1; i < splits.size(); i++) {
      fs::path expected_path = splits[i - 1]->selected_file().path();
      if (!splits[i] || expected_path != splits[i]->dirpath) {
        if (splits[i])
          delete splits[i];
        splits[i] = new FileList(expected_path, bounds.nlines);
        splits[i]->sort_files(sort_by_name_and_directory);
        const fs::path &last_selected_file = last_selected[expected_path];
        if (!splits[i]->find_and_select(last_selected_file)) {
          last_selected[expected_path] = splits[i]->selected_file();
        }
      }
    }
  }

  void clear_splits(int splitid) {
    for (int i = splitid; i < splits.size(); i++) {
      if (splits[i])
        delete splits[i];
      splits[i] = NULL;
    }
  }
};

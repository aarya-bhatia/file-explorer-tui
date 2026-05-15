#include "filelistview.h"
#include "viewutil.h"
#include <filesystem>

void FileListView::render(const AppState &state) {
  if (!dirty) {
    if (state.selected_entry() == _prev_sel && state.get_cwd() == _prev_cwd) {
      wnoutrefresh(win);
      return;
    }
  }

  dirty = false;

  _prev_sel = state.selected_entry();
  _prev_cwd = state.get_cwd();

  auto &d = state.cur_dir();
  draw_list(win, d.files, d.scroll, d.selected);
}

#include "filelistview.h"

void _draw_filename(WINDOW *win, const std::unique_ptr<File> &entry) {
  auto &filemode = entry->st.st_mode;
  if (S_ISDIR(filemode)) {
    wprintw(win, "%s/", entry->filename.c_str());
  } else if (S_ISREG(filemode)) {
    wprintw(win, "%s", entry->filename.c_str());
  } else if (S_ISLNK(filemode)) {
    wprintw(win, "%s@", entry->filename.c_str());
  } else {
    wprintw(win, "? %s", entry->filename.c_str());
  }
}

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

  werase(win);
  wmove(win, 0, 0);
  if (state.count_files() == 0) {
    wprintw(win, "[Empty]");
  } else {
    int file_view_height = getmaxy(win);
    for (int i = state.user_scroll();
         i < std::min(file_view_height + state.user_scroll(),
                      (int)state.count_files());
         i++) {
      int y = i - state.user_scroll();
      wmove(win, y, 0);
      const auto &entry = state.get_file(i);
      if (i == state.selected_entry()) {
        wattron(win, COLOR_PAIR(0) | A_REVERSE);
        _draw_filename(win, entry);
        wattroff(win, COLOR_PAIR(0) | A_REVERSE);
      } else {
        _draw_filename(win, entry);
      }
    }
  }
  wnoutrefresh(win);
}

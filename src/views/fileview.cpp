#include "filelistview.h"

void _draw_filename(WINDOW *win,
                    const std::unique_ptr<AppState::FileEntry> &entry) {
  switch (entry->type) {
  case AppState::FileEntry::File:
    wprintw(win, "%s", entry->filename.c_str());
    break;
  case AppState::FileEntry::Directory:
    wprintw(win, "%s/", entry->filename.c_str());
    break;
  case AppState::FileEntry::Symlink:
    wprintw(win, "%s@", entry->filename.c_str());
    break;
  default:
    wprintw(win, "? %s", entry->filename.c_str());
    break;
  }
}

void FileListView::render(const AppState &state) {
  if (state.selected_entry == _prev_sel && state.cwd == _prev_cwd) {
    wnoutrefresh(win);
    return;
  }

  _prev_sel = state.selected_entry;
  _prev_cwd = state.cwd;

  werase(win);
  wmove(win, 0, 0);
  if (state.files.empty()) {
    wprintw(win, "[Empty]");
  } else {
    int file_view_height = getmaxy(win);
    for (int i = state.user_scroll;
         i < std::min(file_view_height + state.user_scroll,
                      (int)state.files.size());
         i++) {
      int y = i - state.user_scroll;
      wmove(win, y, 0);
      const auto &entry = state.files[i];
      if (i == state.selected_entry) {
        wattron(win, COLOR_PAIR(Colors::Green));
        wprintw(win, ">");
        _draw_filename(win, entry);
        wattroff(win, COLOR_PAIR(Colors::Green));
      } else {
        wprintw(win, " ");
        _draw_filename(win, entry);
      }
    }
  }
  wnoutrefresh(win);
}

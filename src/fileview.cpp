#include "app_state.h"
#include "view.h"
#include <ncurses.h>

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
  werase(win);
  wmove(win, 0, 0);
  if (state.files.empty()) {
    wprintw(win, "[Empty]");
  } else {
    for (int i = 0; i < state.files.size(); i++) {
      wmove(win, i, 0);
      const auto &entry = state.files[i];
      if (i == state.selected_entry) {
        wattron(win, COLOR_PAIR(Colors::Green));
        _draw_filename(win, entry);
        wattroff(win, COLOR_PAIR(Colors::Green));
      } else {
        _draw_filename(win, entry);
      }
    }
  }
  wnoutrefresh(win);
}

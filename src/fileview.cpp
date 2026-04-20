#include "app_state.h"
#include "view.h"
#include <ncurses.h>

#define SELECTION_COLOR 1

void FileListView::render(const AppState &state) {
  werase(win);
  wmove(win, 0, 0);
  int y = 0;
  for (const AppState::FileEntry &entry : state.files) {
    wprintw(win, "%s", entry.filename.c_str());
    y++;
    wmove(win, y, 0);
  }
  wnoutrefresh(win);
}

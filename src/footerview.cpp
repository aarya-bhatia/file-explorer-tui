#include "app_state.h"
#include "view.h"
#include <ncurses.h>

#define FOOTER_COLOR 3

void FooterView::render(const AppState &state) {
  werase(win);
  wbkgd(win, COLOR_PAIR(FOOTER_COLOR));
  wattron(win, COLOR_PAIR(FOOTER_COLOR) | A_BOLD);

  // Fill the whole line with the background color
  mvwhline(win, 0, 0, ' ', width);

  wprintw(win, "cwd: %s", state.cwd.c_str());
  if (state.files.size() > 0) {
    wprintw(win, "\t[%d/%d]", 1 + state.selected_entry, state.files.size());
  }

  wattroff(win, COLOR_PAIR(FOOTER_COLOR) | A_BOLD);
  wnoutrefresh(win);
}


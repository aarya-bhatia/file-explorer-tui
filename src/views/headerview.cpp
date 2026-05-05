#include "app_state.h"
#include "view.h"
#include <ncurses.h>

void HeaderView::render(const AppState &state) {
  werase(win);

  wattron(win, COLOR_PAIR(1));
  mvwprintw(win, 0, 0, "[?] help [q] quit [<ENTER>] open dir [-] parent dir [j] down [k] up");
  wattroff(win, COLOR_PAIR(1));

  wnoutrefresh(win);
}

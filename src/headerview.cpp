#include "headerview.h"
void HeaderView::render(const AppState &state) {
  werase(win);

  wattron(win, COLOR_PAIR(1));
  mvwprintw(win, 0, 0, "[?] help [q] quit <enter> open [-] back [j] down [k] up");
  wattroff(win, COLOR_PAIR(1));

  wnoutrefresh(win);
}

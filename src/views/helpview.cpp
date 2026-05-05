#include "app_state.h"
#include "view.h"
#include <ncurses.h>

void HelpView::render(const AppState &state) {
  werase(win);
  mvwprintw(win, 0, 0, "[?] toggle help menu");
  mvwprintw(win, 1, 0, "[q] quit");
  mvwprintw(win, 2, 0, "[j] move down");
  mvwprintw(win, 3, 0, "[k] move up");
  mvwprintw(win, 4, 0, "[<ENTER>] select");
  mvwprintw(win, 5, 0, "[-] go to parent dir");
  wnoutrefresh(win);
}

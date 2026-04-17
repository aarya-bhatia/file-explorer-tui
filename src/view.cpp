#include "app_state.h"
#include "util.h"
#include "view.h"
#include <assert.h>
#include <ncurses.h>

void FooterView::render(const AppState &state) {
  werase(win);
  wbkgd(win, COLOR_PAIR(FOOTER_COLOR));
  wattron(win, COLOR_PAIR(FOOTER_COLOR) | A_BOLD);

  // Fill the whole line with the background color
  mvwhline(win, 0, 0, ' ', width);

  wattroff(win, COLOR_PAIR(FOOTER_COLOR) | A_BOLD);
  wnoutrefresh(win);
}

void HeaderView::render(const AppState &state) {
  werase(win);

  wattron(win, COLOR_PAIR(1));
  mvwprintw(win, 0, 0, "help: [q] quit");
  wattroff(win, COLOR_PAIR(1));

  wnoutrefresh(win);
}

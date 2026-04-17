#include "app_state.h"
#include "view.h"
#include <ncurses.h>

#define SELECTION_COLOR 1
#define OTHER_MONTH_COLOR 2
#define FOOTER_COLOR 3

void FooterView::render(const AppState &state) {
  werase(win);
  wbkgd(win, COLOR_PAIR(FOOTER_COLOR));
  wattron(win, COLOR_PAIR(FOOTER_COLOR) | A_BOLD);

  // Fill the whole line with the background color
  mvwhline(win, 0, 0, ' ', width);

  wprintw(win, "cwd: %s", state.cwd.c_str());

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

void FileListView::render(const AppState &state) {
  werase(win);
  wmove(win, 0, 0);
  int y = 0;
  for (const std::string &file : state.files) {
    wprintw(win, "%s", file.c_str());
    y++;
    wmove(win, y, 0);
  }
  wnoutrefresh(win);
}

#include "app_state.h"
#include "view.h"
#include <libgen.h>
#include <ncurses.h>
#include <stdlib.h>
#include <sys/param.h>
#include <unistd.h>

#define FOOTER_COLOR 3

void FooterView::render(const AppState &state) {
  werase(win);
  wmove(win, 0, 0);
  wattron(win, COLOR_PAIR(FOOTER_COLOR) | A_BOLD);
  wprintw(win, "$ %s", state.cwd.c_str());
  if (state.cwd != "/") wprintw(win, "/");
  wattroff(win, COLOR_PAIR(FOOTER_COLOR) | A_BOLD);
  if (!state.files.empty())
    wprintw(win, "%s", state.get_selected_filename().c_str());

  if (state.files.size() > 0) {
    char count[1024];
    snprintf(count, sizeof count, "[%d/%lu]", 1 + state.selected_entry,
             state.files.size());
    wattron(win, COLOR_PAIR(FOOTER_COLOR) | A_BOLD);
    wmove(win, 0, getmaxx(win) - strlen(count));
    wprintw(win, "%s", count);
    wattroff(win, COLOR_PAIR(FOOTER_COLOR) | A_BOLD);
  }

  wnoutrefresh(win);
}

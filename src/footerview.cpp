#include "app_state.h"
#include "view.h"
#include <ncurses.h>
#include <stdlib.h>
#include <unistd.h>
#include <libgen.h>
#include <sys/param.h>

#define FOOTER_COLOR 3

void FooterView::render(const AppState &state) {
  werase(win);
  wattron(win, COLOR_PAIR(FOOTER_COLOR) | A_BOLD);
  wmove(win, 0, 0);
  if (state.files.size() > 0) {
    wprintw(win, "[%d/%d]", 1 + state.selected_entry, state.files.size());
  }
  wprintw(win, "\t%s/", state.cwd.c_str());
  wattroff(win, COLOR_PAIR(FOOTER_COLOR) | A_BOLD);

  const std::string &selected_filename = state.get_selected_filename();
  static char buf[MAXPATHLEN];
  char *bname = basename_r(selected_filename.c_str(), buf);
  if(!bname) {
    perror("basename_r");
  } else {
    wprintw(win, "%s", bname);
  }
  wnoutrefresh(win);
}


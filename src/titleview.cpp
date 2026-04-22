#include "app_state.h"
#include "view.h"
#include <libgen.h>
#include <ncurses.h>
#include <stdlib.h>
#include <sys/param.h>
#include <unistd.h>

void print_right_align(WINDOW *win, int line_no, char *text) {
  int width = getmaxx(win);
  size_t text_len = strlen(text);
  if (text_len > width) {
    text[width] = 0;
    text_len = width;
  }
  mvwprintw(win, line_no, width - text_len, text);
}

void TitleView::render(const AppState &state) {
  werase(win);
  wmove(win, 0, 0);
  wattron(win, COLOR_PAIR(Colors::Blue) | A_BOLD);
  wprintw(win, "$ %s", state.cwd.c_str());
  if (state.cwd != "/")
    wprintw(win, "/");
  wattroff(win, COLOR_PAIR(Colors::Blue) | A_BOLD);
  if (!state.files.empty()) {
    wprintw(win, "%s", state.get_selected_filename().substr(0, width).c_str());
  }

  if (state.files.size() > 0) {
    char s[24] = {0};
    snprintf(s, sizeof s - 1, "[%d/%lu]", 1 + state.selected_entry,
             state.files.size());
    print_right_align(win, 0, s);
  }

  wnoutrefresh(win);
}

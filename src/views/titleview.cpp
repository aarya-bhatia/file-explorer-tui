#include "titleview.h"

TitleView::TitleView(Rect r) : View(r) { _init(); }

void TitleView::_init() {
  titleline = get_login_username() + "@" + get_hostname() + " %";
}

void TitleView::render(const AppState &state) {
  werase(win);
  wmove(win, 0, 0);

  wprintw(win, "%s ", titleline.c_str());

  wattron(win, COLOR_PAIR(Colors::Blue) | A_BOLD);
  wprintw(win, "%s", state.cwd.c_str());

  if (state.cwd != "/")
    wprintw(win, "/");
  wattroff(win, COLOR_PAIR(Colors::Blue) | A_BOLD);
  if (!state.files.empty()) {
    wprintw(win, "%s", state.get_selected_filename().substr(0, width()).c_str());
  }

  if (state.files.size() > 0) {
    char s[24] = {0};
    snprintf(s, sizeof s - 1, "[%d/%lu]", 1 + state.selected_entry,
             state.files.size());
    print_right_align(0, s);
  }

  wnoutrefresh(win);
}

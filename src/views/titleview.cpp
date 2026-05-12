#include "titleview.h"
#include "cwalk.h"

TitleView::TitleView(Rect r) : View(r) { _init(); }

void TitleView::_init() {
  titleline = get_login_username() + "@" + get_hostname() + ":";
}

void TitleView::render(const AppState &state) {
  werase(win);

  // wmove(win, 0, 0);
  // wprintw(win, "%s", state.cwd.c_str());

  // wattron(win, COLOR_PAIR(Colors::Blue) | A_BOLD);
  // wprintw(win, "%s", state.cwd.c_str());

  // if (state.cwd != "/")
  //   wprintw(win, "/");
  // wattroff(win, COLOR_PAIR(Colors::Blue) | A_BOLD);

  wmove(win, 0, 0);
  wprintw(win, "%s ", titleline.c_str());

  char *homepath = getenv("HOME");
  std::string display_path = state.get_selected_filepath();
  if (homepath != NULL) {
    if (state.cwd.substr(0, strlen(homepath)) == std::string(homepath)) {
      std::vector<char> buf(display_path.size() + 1);
      if (cwk_path_get_relative(homepath, display_path.c_str(), buf.data(),
                                buf.size()) > 0) {
        display_path = std::string(buf.data()).insert(0, "~/");
      }
    }
  }

  if (!state.files.empty()) {
    wprintw(win, "%s", display_path.c_str());
  }

  wnoutrefresh(win);
}

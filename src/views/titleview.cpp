#include "cwalk.h"
#include "titleview.h"

TitleView::TitleView(Rect r) : View(r) { _init(); }

void TitleView::_init() {
  titleline = get_login_username() + "@" + get_hostname() + ":";
}

void TitleView::render(const AppState &state) {
  werase(win);

  wmove(win, 0, 0);
  wprintw(win, "%s ", titleline.c_str());

  char *homepath = getenv("HOME");
  std::string display_path = state.get_selected_filepath();
  if (homepath != NULL) {
    if (state.get_cwd().substr(0, strlen(homepath)) == std::string(homepath)) {
      std::vector<char> buf(display_path.size() + 1);
      if (cwk_path_get_relative(homepath, display_path.c_str(), buf.data(),
                                buf.size()) > 0) {
        display_path = std::string(buf.data()).insert(0, "~/");
      }
    }
  }

  if (!state.count_files()) {
    wprintw(win, "%s", display_path.c_str());
  }

  wnoutrefresh(win);
}

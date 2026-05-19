#include "titleview.h"

TitleView::TitleView(Rect r) : View(r) { _init(); }

void TitleView::_init() {
  titleline = get_login_username() + "@" + get_hostname() + ":";
}

void TitleView::render(const AppState &state) {
  werase(win);

  wmove(win, 0, 0);
  wprintw(win, "%s ", titleline.c_str());

  char *home_env = getenv("HOME");
  fs::path display_path = state.get_selected_filepath();
  fs::path cwd = state.get_cwd();
  
  if (home_env != NULL) {
    fs::path home(home_env);
    // Check if cwd is under home
    auto rel = fs::path();
    try {
        // Simple check: does the path start with home?
        std::string cwd_s = cwd.string();
        std::string home_s = home.string();
        if (cwd_s.find(home_s) == 0) {
            // It's under home
            if (cwd_s == home_s) {
                wprintw(win, "~");
            } else {
                wprintw(win, "~/%s", cwd_s.substr(home_s.length() + 1).c_str());
            }
        } else {
            wprintw(win, "%s", cwd_s.c_str());
        }
    } catch (...) {
        wprintw(win, "%s", cwd.c_str());
    }
  } else {
    wprintw(win, "%s", cwd.c_str());
  }

  wnoutrefresh(win);
}

#include "app_state.h"
#include "view.h"
#include <ncurses.h>

void CmdLineView::render(const AppState &state) {
  werase(win);
  wmove(win, 0, 0);

  if (state.typing) {
    wprintw(win, "%s %s", state.prompt, state.cmdline_input.c_str());
    // } else if(!state.statushidden) {
    //   mvwprintw(win, 0, 0, "%s", state.statusline.c_str());
  } else {
    const std::string &selected_filename = state.get_selected_filename();
    FileStat s{};
    get_file_stat(selected_filename.c_str(), s);
    wattron(win, COLOR_PAIR(Colors::Blue));
    wprintw(win, "%s", s.mode_s);
    wattroff(win, COLOR_PAIR(Colors::Blue));
    wprintw(win, " %s %s %s", s.owner_name.c_str(), s.group_name.c_str(),
            s.mod_date.c_str());
  }

  wnoutrefresh(win);
}

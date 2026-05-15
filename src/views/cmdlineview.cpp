#include "../include/fileutil.h"
#include "../include/util.h"
#include "cmdlineview.h"

void CmdLineView::render(const AppState &state) {
  werase(win);
  wmove(win, 0, 0);

  if (!state.statushidden) {
    wprintw(win, "%s", state.statusline.c_str());
    wnoutrefresh(win);
    return;
  }

  switch (state.mode) {
  case AppState::Mode::Normal:
    print_file_stat(state);
    print_file_index(state);
    break;
  case AppState::Mode::Command:
    wprintw(win, ":%s", state.cmdline_input.c_str());
    break;
  case AppState::Mode::Search:
    wprintw(win, "/%s", state.cmdline_input.c_str());
    break;
  }

  wnoutrefresh(win);
}

void CmdLineView::print_file_stat(const AppState &state) {
  if (state.count_files() == 0)
    return;
  auto &selected_file = state.get_selected_file();
  FileStat s{};
  get_file_stat(selected_file, s);
  wattron(win, COLOR_PAIR(Colors::Blue));
  wprintw(win, "%s", s.mode_s.c_str());
  wattroff(win, COLOR_PAIR(Colors::Blue));
  wprintw(win, " %s %s %s", s.owner_name.c_str(), s.group_name.c_str(),
          s.mod_date.c_str());
  std::vector<char> dispsize(32);
  get_human_size(s.size, dispsize.data(), dispsize.size());
  wprintw(win, " %s", dispsize.data());
}

void CmdLineView::print_file_index(const AppState &state) {
  if (state.count_files() > 0) {
    char s[24] = {0};
    snprintf(s, sizeof s, "[%d/%lu]", 1 + state.selected_entry(),
             state.count_files());
    print_right_align(0, s);
  }
}

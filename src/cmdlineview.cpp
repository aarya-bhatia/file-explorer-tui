#include "app_state.h"
#include "view.h"
#include <ncurses.h>

void CmdLineView::render(const AppState &state) {
  werase(win);

  if(state.typing) {
    mvwprintw(win, 0, 0, "%s %s", state.prompt, state.cmdline_input.c_str());
  } else if(!state.statushidden) {
    mvwprintw(win, 0, 0, "%s", state.statusline.c_str());
  }

  wnoutrefresh(win);
}


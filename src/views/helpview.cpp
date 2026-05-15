#include "helpview.h"
#include <array>

void HelpView::render(const AppState &state) {
  werase(win);
  static const std::array menu_items = {
    "[?] toggle help menu",
    "[q] quit",
    "[j] move down",
    "[k] move up",
    "[<ENTER>] select",
    "[-] go to parent dir",
  };

  for(int i = 0; i < std::size(menu_items); i++) {
    mvwprintw(win, i, 0, menu_items[i]);
  }
  wnoutrefresh(win);
}

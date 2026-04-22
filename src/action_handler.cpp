#include "action_handler.h"
#include "util.h"
#include <ncurses.h>

bool ActionHandler::operator()(int ch, AppState &state) {
  log_printf("Got input: %d", ch);
  switch (ch) {
  case 'q':
    state.running = false;
    return true;
  case '?':
    state.show_help_menu = !state.show_help_menu;
    return true;
  case 'k':
    state.select_prev();
    return true;
  case 'j':
    state.select_next();
    return true;
  case KEY_ENTER:
  case '\n':
    if (!state.enter_directory()) {
      log_printf("Failed to open directory: %s",
                 state.get_selected_filename().c_str());
    }
    return true;
  case '-':
    state.open_parent_directory();
    return true;
  }

  return false;
}

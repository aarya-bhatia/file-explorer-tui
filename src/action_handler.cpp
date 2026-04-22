#include "action_handler.h"
#include "util.h"
#include <ncurses.h>

bool ActionHandler::operator()(int ch, AppState &state) {
  log_printf("Got input: 0x%0x", ch);

  if (ch == KEY_ENTER || ch == '\n') {
    return on_enter(state);
  }

  if (state.typing) {
    if (ch == KEY_BACKSPACE || ch == 127 || ch == '\b') {
      state.cmdline_input.pop_back();
    } else if (ch == CTRL('u')) {
      state.cmdline_input.clear();
    } else if (isprint(ch)) {
      state.cmdline_input += ch;
    } else {
      log_puts("illegal input character");
    }
    return true;
  }

  if (ch == 'q') {
    log_puts("Stopping application...");
    state.running = false;
    return true;
  }

  if (ch == '?') {
    log_puts("Opening help menu...");
    state.show_help_menu = !state.show_help_menu;
    return true;
  }

  if (ch == 'k') {
    state.select_prev();
    return true;
  }

  if (ch == 'j') {
    state.select_next();
    return true;
  }

  if (ch == '-') {
    log_puts("Opening parent directory...");
    state.open_parent_directory();
    return true;
  }

  if (ch == 'o') {
    log_puts("Create file request...");
    return create_file_prompt(state);
  }

  if (ch == CTRL('l')) {
    log_puts("clearing statusline...");
    state.statushidden = !state.statushidden;
    return true;
  }

  return false;
}

bool ActionHandler::create_file_prompt(AppState &state) {
  state.typing = true;
  state.prompt = "Enter file name > ";
  state.cmdline_input = "";
  next_callback = std::make_unique<CreateFileCallback>();
  return true;
}

bool ActionHandler::on_enter(AppState &state) {
  if (state.typing) {
    log_puts("Running callback...");
    if (!next_callback) {
      log_puts("ERROR callback is null");
    } else {
      next_callback->run(state);
      state.typing = false;
      state.prompt = "";
      state.cmdline_input = "";
      next_callback = nullptr;
    }
  } else {
    log_puts("Opening directory...");
    if (!state.enter_directory()) {
      log_printf("Failed to open directory: %s",
                 state.get_selected_filename().c_str());
    }
  }
  return true;
}

#include "action_handler.h"
#include "util.h"
#include <ncurses.h>

bool ActionHandler::operator()(int ch, AppState &state) {
  log_printf("Got input: %d", ch);

  if (ch == KEY_ENTER || ch == '\n') {
    return on_enter(state);
  }

  if (state.typing) {
    state.cmdline_input += ch;
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

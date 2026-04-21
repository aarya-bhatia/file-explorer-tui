#include "app.h"
#include "util.h"
#include "view.h"
#include <ncurses.h>
#include <stdio.h>

Application::Application(const char *cwd): state(cwd) {
  initscr();
  cbreak();
  noecho();
  keypad(stdscr, TRUE);
  curs_set(0);
  start_color();
  use_default_colors();
  init_pair(Colors::Red, COLOR_RED, -1);
  init_pair(Colors::Blue, COLOR_BLUE, -1);
  init_pair(Colors::Green, COLOR_GREEN, -1);
  init_pair(Colors::Yellow, COLOR_YELLOW, -1);
}

Application::~Application() { endwin(); }

void Application::recompute_layout() {
  views.clear();
  erase();
  refresh();

  if (LINES < MIN_LINES || COLS < MIN_COLS) {
    log_printf("screen is too small (min: %d x %d)", MIN_LINES, MIN_COLS);
    state.running = false;
    return;
  }

  log_printf("screen size: %d x %d", LINES, COLS);

  // int header_h = 4;
  // int footer_h = 1;
  // int footer_y = header_h;

  views.push_back(std::make_unique<HeaderView>(0, 0, 1, COLS));
  views.push_back(std::make_unique<FileListView>(2, 0, LINES - 5, COLS));
  views.push_back(std::make_unique<FooterView>(LINES - 2, 0, 1, COLS));

  helpview = std::make_unique<HelpView>(0, 0, LINES, COLS);
}

void Application::render() {
  if (state.show_help_menu) {
    helpview->render(state);
  } else {
    for (auto &view : views) {
      view->render(state);
    }
  }
  doupdate();
}

void Application::handle_input_typing(int ch) {}

void Application::handle_input_actions(int ch) {
  log_printf("Got input: %d", ch);
  switch (ch) {
  case 'q':
    state.running = false;
    break;
  case '?':
    state.show_help_menu = !state.show_help_menu;
    break;
  case 'k':
    state.select_prev();
    break;
  case 'j':
    state.select_next();
    break;
  case KEY_ENTER:
  case '\n':
    if (!state.enter_directory()) {
      log_printf("Failed to open directory: %s",
                 state.get_selected_filename().c_str());
    }
    break;
  case '-':
    state.open_parent_directory();
    break;
  }
}

void Application::handle_input() {
  int ch = getch();
  if (ch == KEY_RESIZE) {
    recompute_layout();
    return;
  } else if (ch == KEY_F(1)) {
    return; // exit
  }

  handle_input_actions(ch);
}

void Application::run() {
  if (views.empty())
    recompute_layout();
  while (state.running) {
    render();
    handle_input();
  }
}

#include "app.h"
#include "util.h"
#include "view.h"
#include <fcntl.h>
#include <memory.h>
#include <ncurses.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define CTRL(c) ((c) & 0x1F)

Application::Application() {
  int logfile = open(LOG_FILENAME, O_CREAT | O_TRUNC | O_WRONLY, 0640);
  dup2(logfile, 2);
  close(logfile);
  setlocale(LC_ALL, "");
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

  int header_h = 4;
  int footer_h = 1;
  int footer_y = header_h;

  views.push_back(std::make_unique<HeaderView>(0, 0, header_h, COLS));
  views.push_back(std::make_unique<FooterView>(footer_y, 0, footer_h, COLS));
}

void Application::render() {
  for (auto &view : views) {
    view->render(state);
  }
  doupdate();
}

void Application::handle_input_typing(int ch) {}

void Application::handle_input_actions(int ch) {
  switch (ch) {
  case 'q':
    state.running = false;
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

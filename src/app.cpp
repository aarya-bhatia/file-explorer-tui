#include "app.h"
#include "util.h"
#include <memory>
#include <ncurses.h>

Application::Application(const char *cwd) : state(cwd) {
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
  resize();
}

void Application::resize() {
  if (LINES < MIN_LINES || COLS < MIN_COLS) {
    log_printf("screen is too small (min: %d x %d)", MIN_LINES, MIN_COLS);
    state.running = false;
    return;
  }

  log_printf("screen size: %d x %d", LINES, COLS);
  scene_manager = std::make_unique<SceneManager>(LINES, COLS);
}

Application::~Application() {
  log_puts("exiting ncurses");
  endwin();
}

void Application::run() {
  while (state.running) {
    scene_manager->render(state);
    int ch = getch();
    if (ch == KEY_RESIZE) {
      resize();
      continue;
    } else if (ch == KEY_F(1)) {
      return;
    }
    if (!action_handler(ch, state)) {
      log_printf("Failed to handle action for key 0x%x", ch);
    }
  }
}

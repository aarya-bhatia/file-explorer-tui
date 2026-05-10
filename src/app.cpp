#include "include/app.h"
#include "util.h"
#include "views/cmdlineview.h"
#include "views/filelistview.h"
#include "views/filepreviewview.h"
#include "views/headerview.h"
#include "views/titleview.h"
#include <cctype>
#include <iterator>
#include <memory>
#include <ncurses.h>
#include <sstream>

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
  init_pair(5, COLOR_BLACK, COLOR_BLUE);
  resize();
}

void Application::resize() {
  if (LINES < MIN_LINES || COLS < MIN_COLS) {
    log_info("screen must be at least %d lines x %d cols", MIN_LINES, MIN_COLS);
    state.running = false;
    return;
  }

  log_info("screen size: %d lines x %d cols", LINES, COLS);
  state.resize(LINES, COLS);
  init_views();
}

void Application::init_views() {
  erase();
  refresh();

  views.clear();

  views.push_back(std::make_unique<HeaderView>(
      Rect{.begy = 0, .begx = 0, .nlines = 1, .ncols = COLS}));

  views.push_back(std::make_unique<TitleView>(
      Rect{.begy = 1, .begx = 0, .nlines = 1, .ncols = COLS}));

  views.push_back(std::make_unique<CmdLineView>(
      Rect{.begy = LINES - 1, .begx = 0, .nlines = 1, .ncols = COLS}));

  int h_avail = LINES;
  for(auto &view: views) h_avail -= view->height();
  state.file_view_height = h_avail;

  std::unique_ptr<FileListView> filelist_view =
      std::make_unique<FileListView>(Rect{.begy = 2,
                                          .begx = 0,
                                          .nlines = state.file_view_height,
                                          .ncols = COLS});

  views.push_back(std::move(filelist_view));

  helpview = std::make_unique<HelpView>(
      Rect{.begy = 0, .begx = 0, .nlines = LINES, .ncols = COLS});
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

Application::~Application() {
  log_info("exiting ncurses");
  endwin();
}

Application::Input Application::convert_input(int ch) {
  Input in;
  in.val = 0;

  switch (ch) {
  case KEY_ENTER:
  case '\n':
    in.type = Input::Type::ENTER;
    log_debug("input: <cr>");
    break;

  case KEY_BACKSPACE:
  case 127:
  case '\b':
    in.type = Input::Type::BACKSPACE;
    log_debug("input: <bs>");
    break;

  default:
    if (isprint(ch)) {
      in.type = Input::Type::KEY;
      in.val = ch;
      log_debug("input: %c", in.val);
    } else if (iscntrl(ch)) {
      in.type = Input::Type::CONTROL;
      in.val = ch & 0x1f;
      log_debug("input: ctrl+%c", in.val);
    }
  }

  return in;
}

void Application::run() {
  while (state.running) {
    render();
    int ch = getch();
    if (ch == KEY_RESIZE) {
      resize();
      continue;
    } else if (ch == KEY_F(1)) {
      return;
    }

    Input in = convert_input(ch);
    handle_input(in);
  }
}

void Application::handle_finish_typing() {
  state.typing = false;
  std::istringstream iss(state.cmdline_input);
  state.cmdline_input.clear();
  std::vector<std::string> tokens((std::istream_iterator<std::string>(iss)),
                                  std::istream_iterator<std::string>());
  handle_user_command(tokens);
}

void Application::handle_input_typing(Input &input) {
  if (!state.typing) {
    return;
  }

  switch (input.type) {
  case Input::Type::KEY:
    state.cmdline_input += input.val;
    break;

  case Input::Type::ENTER:
    handle_finish_typing();
    break;

  case Input::Type::BACKSPACE:
    if (!state.cmdline_input.empty())
      state.cmdline_input.pop_back();
    break;

  case Input::Type::CONTROL:
    if (input.val == 'u') {
      state.cmdline_input.clear();
    }
    break;
  }
}

void Application::handle_input(Input &input) {
  if (state.is_typing()) {
    handle_input_typing(input);
    return;
  }

  switch (input.type) {
  case Input::Type::ENTER:
    state.open_selected_entry();
    break;

  case Input::Type::BACKSPACE:
    break;

  case Input::Type::CONTROL:
    if (input.val == 'l') {
      state.toggle_show_status();
    }
    break;

  case Input::Type::KEY:
    handle_input_key(input.val);
    break;
  }
}

void Application::handle_input_key(int ch) {
  switch (ch) {
  case 'q':
    state.quit();
    break;

  case '?':
    state.toggle_show_help();
    break;

  case 'k':
    state.handle_up_key();
    break;

  case 'j':
    state.handle_down_key();
    break;

  case '-':
    state.open_parent_directory();
    break;

  case 'L':
    state.select_bottom_entry();
    break;

  case 'H':
    state.select_top_entry();
    break;

  case 'M':
    state.select_middle_entry();
    break;
  }
}

void Application::handle_user_command(const std::vector<std::string> &tokens) {}

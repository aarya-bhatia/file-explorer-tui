#include "include/app.h"
#include "include/fileutil.h"
#include "views/cmdlineview.h"
#include "views/filelistview.h"
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
  resize();
}

void Application::resize() {
  if (LINES < MIN_LINES || COLS < MIN_COLS) {
    log_printf("ERROR screen must be at least %d lines x %d cols", MIN_LINES, MIN_COLS);
    state.running = false;
    return;
  }

  log_printf("screen size: %d lines x %d cols", LINES, COLS);
  init_views();

  if (!state.is_entry_visible(state.selected_entry)) {
    state.user_scroll =
        std::min<int>(0, state.selected_entry - state.file_view_height / 2);
  }
}

bool Application::add_view(
    std::function<std::unique_ptr<View>(Rect r)> makeview, int req_h) {
  if (req_h < 1)
    return false;

  int y = 0;
  for (auto &view : views) {
    y += view->height();
  }
  if (y + req_h > LINES) {
    return false;
  }
  views.push_back(
      makeview(Rect{.begy = y, .begx = 0, .nlines = req_h, .ncols = COLS}));
  return true;
}

int Application::get_unused_height() {
  int h = 0;
  for (auto &view : views) {
    h += view->height();
  }
  return LINES - h;
}

void Application::init_views() {
  erase();
  refresh();

  views.clear();

  add_view([](Rect r) { return std::make_unique<TitleView>(r); }, 1);

  state.file_view_height = get_unused_height() - 1;
  log_printf("file view height: %d", state.file_view_height);
  add_view([](Rect r) { return std::make_unique<FileListView>(r); },
           state.file_view_height);

  add_view([](Rect r) { return std::make_unique<CmdLineView>(r); }, 1);

  assert(views.size() == 3);

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
  log_printf("exiting ncurses");
  endwin();
}

Application::Input Application::convert_input(int ch) {
  Input in;
  in.val = 0;

  switch (ch) {
  case KEY_ENTER:
  case '\n':
    in.type = Input::Type::ENTER;
    log_printf("input: <cr>");
    break;

  case KEY_BACKSPACE:
  case 127:
  case '\b':
    in.type = Input::Type::BACKSPACE;
    log_printf("input: <bs>");
    break;

  default:
    if (isprint(ch)) {
      in.type = Input::Type::KEY;
      in.val = ch;
      log_printf("input: %c", in.val);
    } else if (iscntrl(ch)) {
      in.type = Input::Type::CONTROL;
      in.val = ch;
      log_printf("input: %s", keyname(ch));
    }
  }

  return in;
}

void Application::run() {
  while (state.running) {
    log_printf("Drawing application...");
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

void Application::handle_start_typing() {
  state.typing = true;
  state.cmdline_input = "";
}

void Application::handle_finish_typing() {
  state.typing = false;
  state.mode = AppState::Mode::Normal;
  if (state.cmdline_input.empty())
    return;
  std::istringstream iss(state.cmdline_input);
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
    else
      handle_finish_typing();
    break;

  case Input::Type::CONTROL:
    if (input.val == CTRL('u')) {
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
    log_printf("Opening selected entry");
    state.open_selected_entry();
    break;

  case Input::Type::BACKSPACE:
    break;

  case Input::Type::CONTROL:
    if (input.val == CTRL('l')) {
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

  case ':':
    handle_start_typing();
    state.mode = AppState::Mode::Command;
    break;

  case '/':
    handle_start_typing();
    state.mode = AppState::Mode::Search;
    break;
  }
}

void Application::handle_user_command(const std::vector<std::string> &tokens) {
  if (tokens.size() == 0)
    return;

  if (tokens[0] == "pwd") {
    state.statusline = state.cwd;
    state.statushidden = false;
  } else if (tokens[0] == "sort") {
    if (tokens.size() == 1) {
      state.sort_strategy = sort_by_name_and_directory;
      state.sort_files();
    } else if (tokens[1] == "filetype") {
      state.sort_strategy = sort_by_filetype;
      state.sort_files();
    }
  }

  for (auto &view : views) {
    view->set_dirty();
  }
}

#include "app.h"
#include <cctype>
#include <sstream>

static const int num_splits = 3;

Application::Application(const fs::path &path) {
  username = get_login_username();
  hostname = get_hostname();

  split_controller = SplitController(num_splits);
  split_controller.set_root_directory(path);

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
    log_printf("ERROR screen must be at least %d lines x %d cols", MIN_LINES,
               MIN_COLS);
    running = false;
    return;
  }

  log_printf("screen size: %d lines x %d cols", LINES, COLS);

  erase();
  refresh();
  int title_h = 1;
  int cmd_h = 1;
  int files_h = LINES - title_h - cmd_h;
  titleview = std::make_unique<TitleView>(
      Rect{.begy = 0, .begx = 0, .nlines = title_h, .ncols = COLS});
  cmdlineview = std::make_unique<CmdLineView>(Rect{
      .begy = title_h + files_h, .begx = 0, .nlines = cmd_h, .ncols = COLS});
  helpview = std::make_unique<HelpView>(
      Rect{.begy = 0, .begx = 0, .nlines = LINES, .ncols = COLS});
  Rect files_rect = {
      .begy = title_h, .begx = 0, .nlines = files_h, .ncols = COLS};
  split_controller.resize(files_rect);
  init_views();
}

void Application::render() {
  if (show_help_menu) {
    helpview->render();
  } else {
    titleview->render();
    cmdlineview->render();
    split_controller.render();
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
  while (running) {
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
  typing = true;
  cmdline_input = "";
}

void Application::handle_finish_typing() {
  typing = false;
  mode = Mode::Normal;
  if (cmdline_input.empty())
    return;
  std::istringstream iss(cmdline_input);
  std::vector<std::string> tokens((std::istream_iterator<std::string>(iss)),
                                  std::istream_iterator<std::string>());
  handle_user_command(tokens);
}

void Application::handle_input_typing(Input &input) {
  if (!typing) {
    return;
  }

  switch (input.type) {
  case Input::Type::KEY:
    cmdline_input += input.val;
    break;

  case Input::Type::ENTER:
    handle_finish_typing();
    break;

  case Input::Type::BACKSPACE:
    if (!cmdline_input.empty())
      cmdline_input.pop_back();
    else
      handle_finish_typing();
    break;

  case Input::Type::CONTROL:
    if (input.val == CTRL('u')) {
      cmdline_input.clear();
    }
    break;
  }
}

void Application::handle_input(Input &input) {
  if (is_typing()) {
    handle_input_typing(input);
    return;
  }

  switch (input.type) {
  case Input::Type::ENTER:
    log_printf("Opening selected entry");
    open_selected_entry();
    break;

  case Input::Type::BACKSPACE:
    break;

  case Input::Type::CONTROL:
    if (input.val == CTRL('l')) {
      statushidden = !statushidden;
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
    running = false;
    break;

  case '?':
    show_help_menu = !show_help_menu;
    break;

  case 'k':
    split_controller.move_up();
    break;

  case 'j':
    split_controller.move_down();
    break;

  case 'h':
    split_controller.move_left();
    break;

  case 'l':
    split_controller.move_right();
    break;

  case 'L':
    // TODO
    // split_controller.select_bottom();
    break;

  case 'H':
    // TODO
    // split_controller.select_top();
    break;

  case 'M':
    // TODO
    // split_controller.select_middle();
    break;

  case ':':
    handle_start_typing();
    mode = Mode::Command;
    break;

  case '/':
    handle_start_typing();
    mode = Mode::Search;
    break;
  }
}

void Application::handle_user_command(const std::vector<std::string> &tokens) {
  if (tokens.size() == 0)
    return;

  if (tokens[0] == "pwd") {
    statusline = split_controller.root_path();
    statushidden = false;
  } else if (tokens[0] == "sort") {
    if (tokens.size() == 1) {
      split_controller.sort(sort_by_name_and_directory);
    } else if (tokens[1] == "filetype") {
      split_controller.sort(sort_by_filetype);
    }
  }
}

void Application::draw_title(WINDOW *win) {
  werase(win);

  // wmove(win, 0, 0);
  // wprintw(win, "%s ", titleline.c_str());
  //
  // fs::path display_path = state.get_selected_filepath();
  // fs::path cwd = state.get_cwd();
  //
  // if (home_env != NULL) {
  //   fs::path home(home_env);
  //   // Check if cwd is under home
  //   auto rel = fs::path();
  //   try {
  //     // Simple check: does the path start with home?
  //     std::string cwd_s = cwd.string();
  //     std::string home_s = home.string();
  //     if (cwd_s.find(home_s) == 0) {
  //       // It's under home
  //       if (cwd_s == home_s) {
  //         wprintw(win, "~");
  //       } else {
  //         wprintw(win, "~/%s", cwd_s.substr(home_s.length() + 1).c_str());
  //       }
  //     } else {
  //       wprintw(win, "%s", cwd_s.c_str());
  //     }
  //   } catch (...) {
  //     wprintw(win, "%s", cwd.c_str());
  //   }
  // } else {
  //   wprintw(win, "%s", cwd.c_str());
  // }

  wnoutrefresh(win);
}

void Application::draw_cmdline(WINDOW *win) {
  werase(win);
  wmove(win, 0, 0);

  // if (!state.statushidden) {
  //   wprintw(win, "%s", state.statusline.c_str());
  //   wnoutrefresh(win);
  //   return;
  // }
  //
  // switch (state.mode) {
  // case AppState::Mode::Command:
  //   wprintw(win, ":%s", state.cmdline_input.c_str());
  //   break;
  // case AppState::Mode::Search:
  //   wprintw(win, "/%s", state.cmdline_input.c_str());
  //   break;
  // case AppState::Mode::Normal:
  //   draw_file_stat();
  //   draw_file_index();
  //   break;
  // }

  wnoutrefresh(win);
}

void Application::draw_file_stat(WINDOW *win) {
  // if (state.count_files() == 0)
  //   return;
  // auto &selected_file = state.get_selected_file();
  // FileStat s{};
  // get_file_stat(selected_file, s);
  // wattron(win, COLOR_PAIR(Colors::Blue));
  // wprintw(win, "%s", s.mode_s.c_str());
  // wattroff(win, COLOR_PAIR(Colors::Blue));
  // wprintw(win, " %s %s %s", s.owner_name.c_str(), s.group_name.c_str(),
  //     s.mod_date.c_str());
  // std::vector<char> dispsize(32);
  // get_human_size(s.size, dispsize.data(), dispsize.size());
  // wprintw(win, " %s", dispsize.data());
}

void Application::draw_file_index(WINDOW *win) {
  //   if (state.count_files() > 0) {
  //     char s[24] = {0};
  //     snprintf(s, sizeof s, "[%d/%lu]", 1 + state.selected_entry(),
  //         state.count_files());
  //     print_right_align(0, s);
  //   }
}

void Application::draw_help(WINDOW *win) {
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

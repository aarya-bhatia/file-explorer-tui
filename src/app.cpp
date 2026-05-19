#include "app.h"
#include <cctype>
#include <sstream>

static const int num_splits = 3;

Application::Application(const fs::path &path) {
  username = get_login_username();
  hostname = get_hostname();

  split_controller = SplitController(num_splits);
  split_controller.set_root_path(path);

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
  delwin(titleview);
  delwin(cmdlineview);
  delwin(helpview);

  // WINDOW *newwin(int nlines, int ncols, int begin_y, int begin_x);
  titleview = newwin(title_h, COLS, 0, 0);
  cmdlineview = newwin(cmd_h, COLS, title_h + files_h, 0);
  helpview = newwin(LINES, COLS, 0, 0);

  Rect files_rect = {
      .begy = title_h, .begx = 0, .nlines = files_h, .ncols = COLS};
  split_controller.resize(files_rect);
}

void Application::render() {
  if (show_help_menu) {
    draw_help(helpview);
  } else {
    draw_title(titleview);
    draw_cmdline(cmdlineview);
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
    log_printf("Redraw");
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
  if (typing) {
    handle_input_typing(input);
    return;
  }

  switch (input.type) {
  case Input::Type::ENTER:
    log_printf("Opening selected entry");
    // TODO open_selected_entry();
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

  wmove(win, 0, 0);
  wprintw(win, "%s@%s: ", username.c_str(), hostname.c_str());

  if (!split_controller.is_current_split_empty()) {
    fs::path display_path =
        split_controller.get_selected_file_in_current_split();
    fs::path cwd = split_controller.get_dirname_in_current_split();

    const char *home_env = getenv("HOME");
    if (home_env != NULL) {
      fs::path home(home_env);
      // Check if cwd is under home
      auto rel = fs::path();
      try {
        // check if the path start with home?
        std::string cwd_s = cwd.string();
        std::string home_s = home.string();
        if (cwd_s.find(home_s) == 0) {
          // It's under home
          if (cwd_s == home_s) {
            wprintw(win, "~");
          } else {
            wprintw(win, "~/%s", cwd_s.substr(home_s.length() + 1).c_str());
          }
        } else {
          wprintw(win, "%s", cwd_s.c_str());
        }
      } catch (...) {
        wprintw(win, "%s", cwd.c_str());
      }
    } else {
      wprintw(win, "%s", cwd.c_str());
    }

    wprintw(win, "/%s", display_path.filename().c_str());
  }

  wnoutrefresh(win);
}

void Application::draw_cmdline(WINDOW *win) {
  werase(win);
  wmove(win, 0, 0);

  if (!statushidden) {
    wprintw(win, "%s", statusline.c_str());
    wnoutrefresh(win);
    return;
  }

  switch (mode) {
  case Mode::Command:
    wprintw(win, ":%s", cmdline_input.c_str());
    break;
  case Mode::Search:
    wprintw(win, "/%s", cmdline_input.c_str());
    break;
  case Mode::Normal:
    draw_file_stat(win);
    draw_file_index(win);
    break;
  }

  wnoutrefresh(win);
}

void Application::draw_file_stat(WINDOW *win) {
  if (split_controller.is_current_split_empty())
    return;
  const fs::directory_entry &sel =
      split_controller.get_selected_entry_in_current_split();
  FileStat s{};
  get_file_stat(sel, s);
  wattron(win, COLOR_PAIR(Colors::Blue));
  wprintw(win, "%s", s.mode_s.c_str());
  wattroff(win, COLOR_PAIR(Colors::Blue));
  wprintw(win, " %s %s %s", s.owner_name.c_str(), s.group_name.c_str(),
          s.mod_date.c_str());
  std::vector<char> dispsize(32);
  get_human_size(s.size, dispsize.data(), dispsize.size());
  wprintw(win, " %s", dispsize.data());
}

void Application::draw_file_index(WINDOW *win) {
  char s[24] = {0};
  if (split_controller.is_current_split_empty()) {
    snprintf(s, sizeof s, "[0/0]");
  } else {
    snprintf(s, sizeof s, "[%d/%lu]",
             1 + split_controller.get_current_split()->selected,
             split_controller.get_current_split()->size());
  }

  print_right_align(win, 0, s);
}

void Application::draw_help(WINDOW *win) {
  werase(win);
  static const std::array menu_items = {
      "[?] toggle help menu", "[q] quit",         "[j] move down",
      "[k] move up",          "[<ENTER>] select", "[-] go to parent dir",
  };

  for (int i = 0; i < (int)std::size(menu_items); i++) {
    mvwprintw(win, i, 0, menu_items[i]);
  }
  wnoutrefresh(win);
}

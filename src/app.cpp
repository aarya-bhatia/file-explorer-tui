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
  init_pair(5, COLOR_BLACK, COLOR_BLUE);
  resize();
}

void Application::resize() {
  if (LINES < MIN_LINES || COLS < MIN_COLS) {
    log_info("screen is too small (min: %d x %d)", MIN_LINES, MIN_COLS);
    state.running = false;
    return;
  }

  state.resize(LINES, COLS);
  log_info("screen size: %d x %d", LINES, COLS);
  init_views();
}

void Application::init_views() {
  erase();
  refresh();

  views.clear();

  std::unique_ptr<View> title_view = std::make_unique<TitleView>(0, 0, 1, COLS);
  std::unique_ptr<View> cmdline_view =
      std::make_unique<CmdLineView>(LINES - 1, 0, 1, COLS);
  views.push_back(std::move(title_view));
  views.push_back(std::move(cmdline_view));

  if (state.show_preview) {
    std::unique_ptr<FileListView> filelist_view =
        std::make_unique<FileListView>(2, 0, LINES - 2, COLS / 2);
    std::unique_ptr<View> preview_view = std::make_unique<FilePreviewView>(
        2, COLS / 2, LINES - 2, COLS - COLS / 2);
    views.push_back(std::move(filelist_view));
    views.push_back(std::move(preview_view));
  } else {
    std::unique_ptr<FileListView> filelist_view =
        std::make_unique<FileListView>(1, 0, LINES - 2, COLS);
    views.push_back(std::move(filelist_view));
  }

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

Application::~Application() {
  log_info("exiting ncurses");
  endwin();
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
    if (!handle_input(ch)) {
      log_info("Failed to handle action for key 0x%x", ch);
    }
  }
}

bool Application::handle_input(int ch) {
  log_debug("Got input: 0x%0x", ch);

  if (ch == KEY_ENTER || ch == '\n') {
    return on_enter();
  }

  if (state.typing) {
    if (ch == KEY_BACKSPACE || ch == 127 || ch == '\b') {
      if (!state.cmdline_input.empty())
        state.cmdline_input.pop_back();
    } else if (ch == CTRL('u')) {
      state.cmdline_input.clear();
    } else if (isprint(ch)) {
      state.cmdline_input += ch;
    } else {
      log_info("illegal input character");
    }
    return true;
  }

  if (ch == 'q') {
    log_info("Stopping application...");
    state.running = false;
    return true;
  }

  if (ch == '?') {
    log_info("Opening help menu...");
    state.show_help_menu = !state.show_help_menu;
    return true;
  }

  if (ch == 'k') {
    handle_up_key();
    return true;
  }

  if (ch == 'j') {
    handle_down_key();
    return true;
  }

  if (ch == '-') {
    log_info("Opening parent directory...");
    state.open_parent_directory();
    return true;
  }

  if (ch == 'o') {
    log_info("Create file request...");
    return create_file_prompt();
  }

  if (ch == CTRL('l')) {
    log_info("clearing statusline...");
    state.statushidden = !state.statushidden;
    return true;
  }

  return false;
}

void Application::handle_down_key() {
  if (!state.select_next())
    return;

  if (state.selected_entry == 1+state.window_bottom_file_index()) {
    state.scroll_down();
  }
}

void Application::handle_up_key() {
  if (!state.select_prev())
    return;

  if (state.selected_entry == state.window_top_file_index()-1) {
    state.scroll_up();
  }
}

bool Application::create_file_prompt() {
  state.typing = true;
  state.prompt = "Enter file name >";
  state.cmdline_input = "";
  next_callback = std::make_unique<CreateFileCallback>();
  return true;
}

bool Application::on_enter() {
  if (state.typing) {
    log_info("Running callback...");
    if (!next_callback) {
      log_info("ERROR callback is null");
    } else {
      next_callback->run(state);
      state.typing = false;
      state.prompt = "";
      state.cmdline_input = "";
      next_callback = nullptr;
    }
  } else {
    log_info("Opening directory...");
    if (!state.enter_directory()) {
      log_info("Failed to open directory: %s",
               state.get_selected_filename().c_str());
    }
  }
  return true;
}

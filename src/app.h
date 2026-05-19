#pragma once

#include "action_callbacks.h"
#include "split_controller.h"
#include <ncurses.h>
#include <vector>

class Application {
public:
  Application(const fs::path &path);
  ~Application();
  void run();
  void resize();
  void render();

  struct Input {
    enum class Type { KEY, ENTER, BACKSPACE, CONTROL } type;
    int val;
  };

private:
  WINDOW *titleview = NULL;
  WINDOW *cmdlineview = NULL;
  WINDOW *helpview = NULL;
  std::unique_ptr<ActionCallback> next_callback;

  std::string username = "";
  std::string hostname = "";

  bool running = true;
  bool show_dotfiles = false;
  bool show_help_menu = false;
  bool show_preview = false;
  bool statushidden = true;
  bool typing = false;
  std::string cmdline_input;
  std::string statusline;
  enum class Mode { Normal, Command, Search } mode = Mode::Normal;

  SplitController split_controller;

  void init_views();

  Input convert_input(int ch);
  void handle_input(Input &in);
  void handle_input_typing(Input &in);
  void handle_start_typing();
  void handle_finish_typing();
  void handle_user_command(const std::vector<std::string> &tokens);
  void handle_enter_key();
  void handle_input_key(int ch);

  void draw_title(WINDOW*);
  void draw_cmdline(WINDOW*);
  void draw_help(WINDOW*);
  void draw_file_stat(WINDOW *);
  void draw_file_index(WINDOW*);
};

#pragma once

#include "../views/helpview.h"
#include "../views/view.h"
#include "action_callbacks.h"
#include <memory>
#include <ncurses.h>
#include <stdio.h>
#include <vector>

class Application {
public:
  Application(const char *cwd = NULL);
  ~Application();
  void run();

  struct Input {
    enum class Type {KEY, ENTER, BACKSPACE, CONTROL} type;
    int val;
  };

private:
  AppState state;
  std::vector<std::unique_ptr<View>> views;
  std::unique_ptr<HelpView> helpview;
  std::unique_ptr<ActionCallback> next_callback;

  void resize();
  void render();

  Input convert_input(int ch);
  void handle_input(Input &in);
  void handle_input_typing(Input &in);
  void handle_finish_typing();
  void handle_user_command(const std::vector<std::string> &tokens);
  void handle_enter_key();
  void handle_input_key(int ch);

  void init_views();
};

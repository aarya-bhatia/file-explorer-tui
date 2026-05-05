#pragma once

#include "../views/view.h"
#include "../views/helpview.h"
#include <memory>
#include <vector>
#include <ncurses.h>
#include <stdio.h>
#include "action_callbacks.h"

class Application {
public:
  Application(const char *cwd = NULL);
  ~Application();
  void run();

private:
  AppState state;
  std::vector<std::unique_ptr<View>> views;
  std::unique_ptr<HelpView> helpview;
  std::unique_ptr<ActionCallback> next_callback;  

  void resize();
  void render();

  bool handle_input(int ch);
  bool on_enter();
  bool create_file_prompt();
  void handle_down_key();
  void handle_up_key();

  bool create_file_callback();
  void init_views();
};

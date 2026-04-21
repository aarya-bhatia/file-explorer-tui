#pragma once

#include "view.h"
#include <memory>
#include <ncurses.h>
#include <stdio.h>
#include <vector>

class Application {
  AppState state;
  std::vector<std::unique_ptr<View>> views;
  std::unique_ptr<HelpView> helpview;

public:
  static const int MIN_LINES = 20;
  static const int MIN_COLS = 50;

  Application(const char *cwd = NULL);
  ~Application();

  void run();
  void render();
  void handle_input();
  void handle_input_typing(int);
  void handle_input_actions(int);

private:
  void recompute_layout();
};

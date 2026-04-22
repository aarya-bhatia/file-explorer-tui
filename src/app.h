#pragma once

#include "action_handler.h"
#include "scene_manager.h"
#include "view.h"
#include <memory>
#include <ncurses.h>
#include <stdio.h>
#include <vector>

class Application {
public:
  Application(const char *cwd = NULL);
  ~Application();
  void run();

private:
  AppState state;
  ActionHandler action_handler;
  std::unique_ptr<SceneManager> scene_manager;

  void resize();
};

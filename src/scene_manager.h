#pragma once
#include "view.h"
#include "app_state.h"
#include <memory>
#include <vector>

class SceneManager {
  std::vector<std::unique_ptr<View>> views;
  std::unique_ptr<HelpView> helpview;

public:
  SceneManager(const AppState &state, int h, int w);
  void render(const AppState &state);
  int file_view_height;
};

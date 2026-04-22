#pragma once
#include "view.h"
#include <memory>
#include <vector>

class SceneManager {
  std::vector<std::unique_ptr<View>> views;
  std::unique_ptr<HelpView> helpview;
  int height, width;

public:
  SceneManager(int h, int w);
  void render(const AppState &state);
};

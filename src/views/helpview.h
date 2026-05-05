#pragma once
#include "view.h"
class HelpView: public View { 
public:
  HelpView(Rect r) : View(r) {}
  void render(const AppState &state) override;
};


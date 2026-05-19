#pragma once
#include "view.h"
class HeaderView: public View { 
public:
  HeaderView(Rect r) : View(r) {}
  void render(const AppState &state) override;
};


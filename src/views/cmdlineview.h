#pragma once
#include "view.h"

class CmdLineView: public View {
public:
  CmdLineView(Rect r) : View(r) {}
  void render(const AppState &state) override;
};


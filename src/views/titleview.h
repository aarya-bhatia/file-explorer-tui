#pragma once
#include "view.h"

class TitleView : public View {
public:
  TitleView(Rect r);
  void render(const AppState &state) override;
  std::string titleline = "";
private:
  void _init();
};


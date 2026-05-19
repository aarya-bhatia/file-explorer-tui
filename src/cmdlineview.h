#pragma once
#include "view.h"

class CmdLineView : public View
{
public:
  CmdLineView(Rect r) : View(r) {}
  void render(const AppState &state) override;

private:
  void print_file_stat(const AppState &state);
  void print_file_index(const AppState &state);
};

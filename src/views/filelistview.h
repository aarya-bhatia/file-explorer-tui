#pragma once
#include "view.h"

class FileListView : public View {
public:
  FileListView(Rect r) : View(r) {}
  void render(const AppState &state) override;
private:
  int _prev_sel;
  std::string _prev_cwd;
};


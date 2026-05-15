#pragma once
#include "view.h"

#include <filesystem>
namespace fs = std::filesystem;

class FileListView : public View {
public:
  FileListView(Rect r) : View(r), _prev_sel(-1) {}
  void render(const AppState &state) override;
private:
  int _prev_sel;
  fs::path _prev_cwd;
};


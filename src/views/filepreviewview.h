#pragma once
#include "view.h"

class FilePreviewView: public View {
public:
  FilePreviewView(Rect r) : View(r) {}
  void render(const AppState &state) override;
};

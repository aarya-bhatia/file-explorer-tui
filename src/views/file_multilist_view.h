#pragma once
#include "view.h"

class FileMultiListView : public View {
public:
  FileMultiListView(Rect r, int n);
  ~FileMultiListView();
  void render(const AppState &state) override;

private:
  std::vector<WINDOW *> splits;
  // TODO cache-per-column
};

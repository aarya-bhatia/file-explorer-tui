#include "file_multilist_view.h"
#include "viewutil.h"

FileMultiListView::~FileMultiListView() {
  for (int i = 0; i < (int)splits.size(); i++) {
    delwin(splits[i]);
  }
  splits.clear();
}

FileMultiListView::FileMultiListView(Rect r, int n) : View(r) {
  int splitw = r.ncols / n;
  if (splitw == 0) {
    log_printf("ERROR view is too small for %d splits", n);
    // TODO fallback to single column mode here.
    return;
  }

  for (int i = 0; i < n; i++) {
    int begx = i * splitw;
    // For the last split, stretch its width to fill any remainder from division
    // truncation
    int current_width = (i == n - 1) ? (r.ncols - begx) : splitw;

    WINDOW *w = derwin(win, r.nlines, current_width, 0, begx);
    if (!w) {
      log_printf("ERROR: failed to create split %d", i);
      continue;
    }
    splits.push_back(w);
  }
}

void FileMultiListView::render(const AppState &state) {
  if (splits.empty() || state.open_dirs.empty()) return;
  size_t count = std::min(splits.size(), state.open_dirs.size());
  int i = 0;
  for (auto it = state.open_dirs.end() - count; it != state.open_dirs.end();
       ++it) {
    const FileList &d = *it;
    draw_list(splits[i], d.files, d.scroll, d.selected);
    i++;
  }
}

#include "scene_manager.h"
#include <ncurses.h>

SceneManager::SceneManager(const AppState &state, int h, int w) {
  erase();
  refresh();

  views.push_back(std::make_unique<TitleView>(0, 0, 2, w));

  views.push_back(
      std::make_unique<FileListView>(2, 0, state.file_view_height, w / 2));
  views.push_back(std::make_unique<FilePreviewView>(
      2, w / 2, state.file_view_height, w - w / 2));
  views.push_back(std::make_unique<CmdLineView>(h - 1, 0, 1, w));

  helpview = std::make_unique<HelpView>(0, 0, h, w);
}

void SceneManager::render(const AppState &state) {
  if (state.show_help_menu) {
    helpview->render(state);
  } else {
    for (auto &view : views) {
      view->render(state);
    }
  }
  doupdate();
}

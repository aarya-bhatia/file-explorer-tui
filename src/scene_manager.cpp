#include "scene_manager.h"
#include <ncurses.h>

SceneManager::SceneManager(int h, int w) {
  erase();
  refresh();

  views.push_back(std::make_unique<TitleView>(0, 0, 1, w));

  views.push_back(std::make_unique<FileListView>(1, 0, h - 3, w/2));
  views.push_back(std::make_unique<FilePreviewView>(1, w/2, h - 3, w-w/2));

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

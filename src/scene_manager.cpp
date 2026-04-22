#include "scene_manager.h"
#include <ncurses.h>

SceneManager::SceneManager(int h, int w): height(h), width(w)
{
  erase();
  refresh();

  // int header_h = 4;
  // int footer_h = 1;
  // int footer_y = header_h;

  views.push_back(std::make_unique<HeaderView>(0, 0, 1, w));
  views.push_back(std::make_unique<FileListView>(2, 0, h - 5, w));
  views.push_back(std::make_unique<FooterView>(h - 2, 0, 1, w));

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

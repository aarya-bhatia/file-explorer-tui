#pragma once
#include "app_state.h"
#include "util.h"
#include <ncurses.h>

class View {
protected:
  WINDOW *win = NULL;
  int start_y, start_x, height, width;

public:
  View(int y, int x, int h, int w) : start_y(y), start_x(x), height(h), width(w) {
    win = newwin(h, w, y, x);
    wrefresh(win);
  }

  virtual ~View() {
    if (win)
      delwin(win);
  }
  virtual void render(const AppState &state) = 0;
};

class HeaderView : public View {
public:
  HeaderView(int y, int x, int h, int w) : View(y, x, h, w) {}
  void render(const AppState &state) override;
};

class FooterView : public View {
public:
  FooterView(int y, int x, int h, int w) : View(y, x, h, w) {}
  void render(const AppState &state) override;
};

class FileListView : public View {
public:
  FileListView(int y, int x, int h, int w) : View(y, x, h, w) {}
  void render(const AppState &state) override;
};

#pragma once
#include "app_state.h"
#include "util.h"
#include <ncurses.h>

class View {
protected:
  WINDOW *win = NULL;
  bool dirty = true;

  void print_right_align(int y, const char *text);

public:
  View(Rect r);
  virtual ~View();
  virtual void render(const AppState &state) = 0;
  virtual void resize(Rect r);
  void set_dirty(){dirty=true;}

  int width() const { return getmaxx(win); }
  int height() const { return getmaxy(win); }
  int begy() const { return getbegy(win); }
  int begx() const { return getbegx(win); }
};

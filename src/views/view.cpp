#include "view.h"

View::View(Rect r) {
  win = newwin(r.nlines, r.ncols, r.begy, r.begx);
  wrefresh(win);
}

View::~View() {
  if (win)
    delwin(win);
}

void View::resize(Rect r) {
  delwin(win);
  win = newwin(r.nlines, r.ncols, r.begy, r.begx);
  wrefresh(win);
  dirty = true;
}

void View::print_right_align(int y, const char *text) {
  int width = getmaxx(win);
  int text_len = (int)strlen(text);
  if (text_len > width) {
    text_len = width;
  }
  mvwaddnstr(win, y, width - text_len, text, text_len);
}

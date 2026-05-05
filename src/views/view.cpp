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

void View::print_right_align(int y, char *text) {
  int width = getmaxx(win);
  size_t text_len = strlen(text);
  if (text_len > width) {
    text[width] = 0;
    text_len = width;
  }
  mvwprintw(win, y, width - text_len, text);
}

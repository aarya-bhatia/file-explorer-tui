#include "viewutil.h"

void print_right_align(WINDOW *win, int y, const char *text) {
  int width = getmaxx(win);
  int text_len = (int)strlen(text);
  if (text_len > width) {
    text_len = width;
  }
  mvwaddnstr(win, y, width - text_len, text, text_len);
}

void draw_filename(WINDOW *win, const fs::directory_entry &entry) {
  if (entry.is_directory()) {
    wprintw(win, "%s/", entry.path().filename().string().c_str());
  } else if (entry.is_regular_file()) {
    wprintw(win, "%s", entry.path().filename().string().c_str());
  } else if (entry.is_symlink()) {
    wprintw(win, "%s@", entry.path().filename().string().c_str());
  } else {
    wprintw(win, "? %s", entry.path().filename().string().c_str());
  }
}

void draw_list(WINDOW *win, const std::vector<fs::directory_entry> &files,
               int view_scroll, int selected, int selected_color_id) {

  werase(win);
  wmove(win, 0, 0);
  if (files.empty()) {
    wprintw(win, "[Empty]");
    wnoutrefresh(win);
    return;
  }
  int file_view_height = getmaxy(win);
  for (int i = view_scroll;
       i < std::min<int>(file_view_height + view_scroll, files.size()); i++) {
    int y = i - view_scroll;
    wmove(win, y, 0);
    if (i == selected) {
      if (selected_color_id > 0)
        wattron(win, COLOR_PAIR(selected_color_id));
      else
        wattron(win, COLOR_PAIR(0) | A_REVERSE);
      draw_filename(win, files[i]);

      if (selected_color_id > 0)
        wattroff(win, COLOR_PAIR(selected_color_id));
      else
        wattroff(win, COLOR_PAIR(0) | A_REVERSE);

    } else {
      draw_filename(win, files[i]);
    }
  }
  wnoutrefresh(win);
}

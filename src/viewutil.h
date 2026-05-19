#pragma once
#include <filesystem>
#include <ncurses.h>
namespace fs = std::filesystem;

void print_right_align(WINDOW *window, int y, const char *text);

void draw_filename(WINDOW *win, const fs::directory_entry &entry);
void draw_list(WINDOW *win, const std::vector<fs::directory_entry> &files,
               int view_scroll, int selected);

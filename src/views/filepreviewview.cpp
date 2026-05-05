#include "view.h"
#include <cstdio>
#include <cstring>
#include <dirent.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

void print_directory_preview(WINDOW *win, DIR *dp) {
  // int width = getmaxx(win);
  int height = getmaxy(win);

  struct dirent *entry = NULL;
  int y = 0;
  for (int i = 0; i < height; i++) {
    entry = readdir(dp);
    if (!entry)
      break;
    if (entry->d_name[0] == '.')
      continue;
    mvwprintw(win, y, 0, entry->d_name);
    y++;
  }
}

void print_file_preview(WINDOW *win, FILE *fp) {
  int width = getmaxx(win);
  int height = getmaxy(win);

  char *linebuf = (char *)malloc(width + 1);
  for (int i = 0; i < height; i++) {
    size_t bytes_read = fread(linebuf, 1, width, fp);
    if (bytes_read == 0)
      break;

    if(memchr(linebuf, '\0', bytes_read) != NULL) {
      wprintw(win, "<binary>");
      break;
    }

    // Ensure we null-terminate for mvwprintw
    linebuf[bytes_read] = '\0';

    char *ptr = (char *)memchr(linebuf, '\n', bytes_read);
    if (ptr) {
      // Found a newline!
      size_t n = ptr - linebuf;
      *ptr = '\0'; // Truncate string at newline for printing

      // Calculate how many bytes to "rewind"
      long rewind = bytes_read - n - 1;
      fseek(fp, -rewind, SEEK_CUR);
    } else {
      // No newline: print full buffer and skip rest of line in file
      int c;
      while ((c = fgetc(fp)) != '\n' && c != EOF)
        ;
    }

    mvwprintw(win, i, 0, "%s", linebuf);
    if (feof(fp))
      break;
  }

  free(linebuf);
}

void FilePreviewView::render(const AppState &state) {
  werase(win);

  if (state.files.empty()) {
    wnoutrefresh(win);
    return;
  }

  const std::string &filename = state.get_selected_filename();
  std::string abspath = state.cwd + "/" + filename;

  FILE *fp = NULL;
  DIR *dp = NULL;

  switch (state.files[state.selected_entry]->type) {
  case AppState::FileEntry::EntryType::Directory:
    dp = opendir(abspath.c_str());
    if (!dp) {
      mvwprintw(win, 0, 0, "Error: Could not open directory.");
    } else {
      print_directory_preview(win, dp);
      closedir(dp);
      dp = NULL;
    }
    break;
  case AppState::FileEntry::EntryType::File:
  case AppState::FileEntry::EntryType::Symlink:
    fp = fopen(abspath.c_str(), "rb");
    if (!fp) {
      mvwprintw(win, 0, 0, "Error: Could not open file.");
    } else {
      print_file_preview(win, fp);
      fclose(fp);
      fp = NULL;
    }
    break;
  default:
    break;
  }

  wnoutrefresh(win);
}

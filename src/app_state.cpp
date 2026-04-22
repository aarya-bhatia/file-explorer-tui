#include "app_state.h"
#include "util.h"
#include <cstring>
#include <dirent.h>
#include <libgen.h>
#include <memory>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

AppState::FileEntry::EntryType map_to_entry_type(int dirent_type);

AppState::~AppState() {}

AppState::AppState(const char *_cwd) {
  if(!_cwd) {
    if (!init()) {
      log_puts("Failed to initialize AppState");
      running = false;
    }
  } else {
    cwd = std::string(_cwd);
    if(!reload_file_list()) {
      running = false;
    }
  }
}

bool AppState::reload_file_list() {
  files.clear();

  struct dirent *entry = NULL;
  DIR *dirp = opendir(cwd.c_str());
  if (!dirp) {
    perror("opendir");
    return false;
  }

  log_printf("listing files in cwd:%s", cwd.c_str());

  while ((entry = readdir(dirp)) != NULL) {
    if (show_dotfiles == false) {
      if (entry->d_namlen > 0 && entry->d_name[0] != '.') {
        FileEntry::EntryType type = map_to_entry_type(entry->d_type);
        files.emplace_back(
            std::make_unique<FileEntry>(type, std::string(entry->d_name)));
        const auto &fileentry = files.back();
        // log_printf("Added entry %s with type:%d", fileentry->filename.c_str(),
        //            fileentry->type);
      }
    }
  }

  closedir(dirp);
  log_printf("Total files: %ld", files.size());
  return true;
}

bool AppState::update_cwd() {
  char *buf = NULL;
  buf = getcwd(buf, 0);
  if (buf == NULL) {
    perror("getcwd");
    return false;
  }

  cwd = std::string(buf);
  free(buf);
  return true;
}

bool AppState::init() {
  if (!update_cwd())
    return false;
  if (!reload_file_list())
    return false;
  selected_entry = 0;
  return true;
}

AppState::FileEntry::EntryType map_to_entry_type(int dirent_type) {
  switch (dirent_type) {
  case DT_DIR:
    return AppState::FileEntry::Directory;
  case DT_REG:
    return AppState::FileEntry::File;
  case DT_LNK:
    return AppState::FileEntry::Symlink;
  default:
    return AppState::FileEntry::Other;
  }
}
bool AppState::enter_directory() {
  assert(selected_entry < files.size());
  if (files[selected_entry]->type != FileEntry::EntryType::Directory) {
    return false;
  }
  std::string new_dir = cwd + "/" + get_selected_filename();
  return open_directory(new_dir);
}

bool AppState::open_parent_directory() {
  char s[1024];
  strncpy(s, cwd.c_str(), sizeof s - 1);
  s[sizeof(s) - 1] = '\0';
  const char *dname = dirname(s);
  std::string dname_s = dname;
  return open_directory(dname_s);
}

bool AppState::open_directory(std::string &path) {
  struct stat s;
  memset(&s, 0, sizeof s);
  if (lstat(path.c_str(), &s) < 0) {
    perror("lstat");
    log_printf("Failed to open dir: %s", path.c_str());
    return false;
  }
  if (!S_ISDIR(s.st_mode)) {
    log_printf("Not a directory: %s", path.c_str());
    return false;
  }
  cwd = path;
  log_printf("changed cwd: %s", cwd.c_str());
  reload_file_list();
  selected_entry = 0;
  return true;
}

#include "fileutil.h"
#include "util.h"
#include <filesystem>
#include <chrono>

namespace fs = std::filesystem;

void get_file_stat(const std::unique_ptr<File> &file, FileStat &filestat) {
    if (!file) return;

    filestat.size = file->size();
    
    // We can't easily get owner/group from std::filesystem yet (it's not in the standard)
    // So we'll keep the legacy helpers from util.h but use the path from file.
    // However, for now let's just use placeholder or implement it via stat if needed.
    // For this refactor, I'll keep the util.h helpers but pass them the right data.
    
    // In a real app, we might still need a bit of POSIX for owner/group
    struct stat st;
    if (lstat(file->path().c_str(), &st) == 0) {
        filestat.owner_name = get_username(st.st_uid);
        filestat.group_name = get_groupname(st.st_gid);
        
        fs::perms p = fs::status(file->path()).permissions();
        
        filestat.mode_s[0] = file->is_directory() ? 'd' : (file->is_link() ? 'l' : '-');
        filestat.mode_s[1] = ((p & fs::perms::owner_read) != fs::perms::none) ? 'r' : '-';
        filestat.mode_s[2] = ((p & fs::perms::owner_write) != fs::perms::none) ? 'w' : '-';
        filestat.mode_s[3] = ((p & fs::perms::owner_exec) != fs::perms::none) ? 'x' : '-';
        filestat.mode_s[4] = ((p & fs::perms::group_read) != fs::perms::none) ? 'r' : '-';
        filestat.mode_s[5] = ((p & fs::perms::group_write) != fs::perms::none) ? 'w' : '-';
        filestat.mode_s[6] = ((p & fs::perms::group_exec) != fs::perms::none) ? 'x' : '-';
        filestat.mode_s[7] = ((p & fs::perms::others_read) != fs::perms::none) ? 'r' : '-';
        filestat.mode_s[8] = ((p & fs::perms::others_write) != fs::perms::none) ? 'w' : '-';
        filestat.mode_s[9] = ((p & fs::perms::others_exec) != fs::perms::none) ? 'x' : '-';

        // Get modification time
        auto ftime = fs::last_write_time(file->path());
        auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(ftime - fs::file_time_type::clock::now() + std::chrono::system_clock::now());
        std::time_t cftime = std::chrono::system_clock::to_time_t(sctp);
        
        char mbstr[100];
        if (std::strftime(mbstr, sizeof(mbstr), "%b %d %H:%M", std::localtime(&cftime))) {
            filestat.mod_date = mbstr;
        }
    }
}

bool sort_by_name_and_directory(const std::unique_ptr<File> &f1, const std::unique_ptr<File> &f2) {
  if (f1->is_directory() != f2->is_directory()) {
    return f1->is_directory();
  }
  return f1->filename() < f2->filename();
}

bool sort_by_filetype(const std::unique_ptr<File> &f1, const std::unique_ptr<File> &f2) {
  if (f1->is_directory() != f2->is_directory()) {
    return f1->is_directory();
  }
  
  auto ext1 = f1->path().extension();
  auto ext2 = f2->path().extension();

  if (ext1 != ext2) {
    return ext1 < ext2;
  }
  
  return f1->filename() < f2->filename();
}

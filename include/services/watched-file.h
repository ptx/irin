#ifndef _WATCHED_FILE_HEADER_H_
#define _WATCHED_FILE_HEADER_H_

#include <string>

class WatchedFile {
  public:
    WatchedFile(const std::string &file_path, const int file_descriptor);
    void set_kevent_idx(const int kevent_idx) { kevent_idx_ = kevent_idx; };
    void Close();
    void set_file_descriptor(const int fd) { file_descriptor_ = fd; };
    int file_descriptor() { return file_descriptor_; };
    std::string file_path() { return file_path_; };
  private:
    std::string file_path_;
    int file_descriptor_;
    int kevent_idx_;
};

#endif

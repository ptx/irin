#ifndef _WATCHER_HEADER_H_
#define _WATCHER_HEADER_H_

#include <pthread.h>
#include "services/watched-file.h"
#include <set>
#include <string>

class Watcher {
  public:
    Watcher(const pthread_t thread, const std::vector<std::string> file_paths);
    void Run() { pthread_create(&thread_, NULL, run_watcher, (void *) &thread_variables_); };
    void Stop();
    void ReplaceOpenFileDescriptor(int old_fd, int new_fd);
    void AddWatchedFile(std::unique_ptr<WatchedFile> &watched_file);
    std::set<std::unique_ptr<WatchedFile>>& watched_files() { return watched_files_; };
  private:
    struct ThreadVariables {
      Watcher* this_ptr;
      std::vector<std::string> file_paths;
    };
    pthread_t thread_;
    std::set<std::unique_ptr<WatchedFile>> watched_files_;
    ThreadVariables thread_variables_;
    static void* run_watcher(void* thread_variables);
    std::vector<std::string> file_paths_;
    static char *flagstring(int flags);
};

#endif

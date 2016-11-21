#ifndef _WATCHER_HEADER_H_
#define _WATCHER_HEADER_H_

#include <pthread.h>
#include <string>

class Watcher {
  public:
    Watcher(const pthread_t thread, const std::string dir_path);
    void Run();
  private:
    struct ThreadVariables {
      std::string dir_path;
    };
    pthread_t thread_;
    ThreadVariables thread_variables_;
    static void* run_watcher(void* thread_variables);
    std::string dir_path_;
};

#endif

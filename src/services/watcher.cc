#include <boost/format.hpp>
#include <boost/log/trivial.hpp>
#include "services/watcher.h"

Watcher::Watcher(const pthread_t thread, const std::string dir_path) {
  this->thread_ = thread;
  this->dir_path_ = dir_path;
  this->thread_variables_ = ThreadVariables{this->dir_path_};
}

void Watcher::Run() {
  pthread_create(&thread_, NULL, run_watcher, (void *) &thread_variables_);
};

void* Watcher::run_watcher(void* thread_variables) {

  while(true) {
    BOOST_LOG_TRIVIAL(info) << "Inside run_watcher!";
    sleep(2);
  }
  
};

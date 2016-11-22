#include <boost/filesystem.hpp>
#include <boost/format.hpp>
#include <boost/log/trivial.hpp>
#include "services/watcher.h"

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>
#include <errno.h>
#include <inttypes.h>

#define FOREVER while(1)

Watcher::Watcher(const pthread_t thread, const std::vector<std::string> file_paths) {
  this->thread_ = thread;
  for(auto &path : file_paths)
    this->file_paths_.push_back(path);

  this->thread_variables_ = ThreadVariables{this, this->file_paths_};
}

void Watcher::Stop() {
  for(const auto& watched_file: this->watched_files_)
    watched_file->Close();
};

void Watcher::AddWatchedFile(std::unique_ptr<WatchedFile> &watched_file) {
  watched_files_.insert(std::move(watched_file));
};

void Watcher::ReplaceOpenFileDescriptor(int old_fd, int new_fd) {
  for(auto& watched_file : watched_files_)
    if(watched_file->file_descriptor() == old_fd)
      watched_file->set_file_descriptor(new_fd);
};

void* Watcher::run_watcher(void* thread_variables) {
  ThreadVariables *variables;
  variables = (ThreadVariables *) thread_variables;

  int fd_count = 0;
  for(const auto &file : variables->file_paths) {
    int fd = open(file.c_str(), O_EVTONLY);
    if(fd < 0) {
      BOOST_LOG_TRIVIAL(error) << "Error opening " << file;
      exit(1);
    }

    std::unique_ptr<WatchedFile> watched_file(new WatchedFile(file, fd));
    variables->this_ptr->AddWatchedFile(watched_file);
    ++fd_count;
  };

  int kq;
  struct kevent events_to_monitor[fd_count];
  struct kevent event_data[fd_count];
  struct timespec timeout;
  timeout.tv_sec = 0;   
  timeout.tv_nsec = 50000000;
  unsigned int vnode_events;
  int num_files = fd_count;

  if((kq = kqueue()) < 0) {
    BOOST_LOG_TRIVIAL(fatal) << "Could not instantiate kqueue";
  }
  vnode_events = NOTE_DELETE | NOTE_WRITE | NOTE_LINK | NOTE_RENAME;

  fd_count = 0;
  for(auto& watched_file : variables->this_ptr->watched_files()) {
    std::string file_path = watched_file->file_path();
    auto event_fd = watched_file->file_descriptor();
    EV_SET(&events_to_monitor[fd_count], event_fd, EVFILT_VNODE, EV_ADD | EV_CLEAR, 
      vnode_events, 0, &file_path);
    watched_file->set_kevent_idx(fd_count);
    ++fd_count;
  }

  FOREVER {
    auto event_count = kevent(kq, events_to_monitor, fd_count, event_data, num_files, &timeout);

    if (event_count > 0) {
      for(int event_idx = 0;event_idx < event_count;++event_idx) {
        std::string *p = static_cast<std::string *>(event_data[event_idx].udata);
        std::string path = *p;
        auto event_fmt = boost::format("kqueue event: ident[%1%], filter[%2%], flags [%3%], filterflags[%4%] path[%5%]")
          % event_data[event_idx].ident
          % event_data[event_idx].filter
          % event_data[event_idx].flags
          % flagstring(event_data[event_idx].fflags)
          % path;
        BOOST_LOG_TRIVIAL(info) << event_fmt.str();
        if(event_data[event_idx].fflags & NOTE_DELETE) {
          // deletion may commonly occur when the programmer's editor exectues atomic saves. 
          // Under this scenario, the original file is renamed, the new contents of the file
          // are saved to the original file name, and then the "renamed" file is deleted.
          auto exists = boost::filesystem::exists(path);
          if(exists) {
            auto new_event_fd = open(path.c_str(), O_EVTONLY);
            EV_SET(&events_to_monitor[event_idx], new_event_fd, EVFILT_VNODE, EV_ADD | EV_CLEAR, vnode_events, 
              0, p);
            variables->this_ptr->ReplaceOpenFileDescriptor(event_data[event_idx].ident ,new_event_fd);
          }
        } else if(event_data[event_idx].fflags & NOTE_RENAME) {
          // NOTE_RENAME can occur if the programmer's editor has chosen to execute atomic saves,
          // i.e. --> rename existing file, create a new file with the original name, writing
          // the file contents in one pass. 
        } else if(event_data[event_idx].fflags & NOTE_LINK) {
          // open for future chantes
        } else if(event_data[event_idx].fflags & NOTE_WRITE) {
          // open for future chantes
        } else {
          // open for future chantes
        }

        // Reset the timeout.  In case of a signal interrruption, the values may change.
        timeout.tv_sec = 0;         // 0 seconds
        timeout.tv_nsec = 50000000; // 50 milliseconds
      }
    }
  }
};

char * Watcher::flagstring(int flags) {
  static char ret[512];
  char* s = ""; // *or; // = "";

  ret[0]='\0'; // clear the string.
  if (flags & NOTE_DELETE) {strcat(ret,s);strcat(ret,"NOTE_DELETE");s="|";}
  if (flags & NOTE_WRITE) {strcat(ret,s);strcat(ret,"NOTE_WRITE");s="|";}
  if (flags & NOTE_EXTEND) {strcat(ret,s);strcat(ret,"NOTE_EXTEND");s="|";}
  if (flags & NOTE_ATTRIB) {strcat(ret,s);strcat(ret,"NOTE_ATTRIB");s="|";}
  if (flags & NOTE_LINK) {strcat(ret,s);strcat(ret,"NOTE_LINK");s="|";}
  if (flags & NOTE_RENAME) {strcat(ret,s);strcat(ret,"NOTE_RENAME");s="|";}
  if (flags & NOTE_REVOKE) {strcat(ret,s);strcat(ret,"NOTE_REVOKE");s="|";}

  return ret;
}


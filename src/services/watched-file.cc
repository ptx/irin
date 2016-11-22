#include <boost/format.hpp>
#include <boost/log/trivial.hpp>
#include "services/watched-file.h"

WatchedFile::WatchedFile(const std::string &file_path, const int file_descriptor) {
  this->file_path_ = file_path;
  this->file_descriptor_ = file_descriptor;
}

void WatchedFile::Close() {
  close(file_descriptor_);
  BOOST_LOG_TRIVIAL(info) << "Closed File [" << file_path_ << "], FD[" << file_descriptor_ << "]";
  file_descriptor_ = -1;
};

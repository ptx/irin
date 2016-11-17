#include "server/content-service.h"
#include <fstream>
#include <sstream>

ContentService::ContentService(const std::string &pages_dir) {
  this->pages_dir_ = boost::filesystem::path(pages_dir);
};

std::unique_ptr<HttpResponse> ContentService::Handle(const std::unique_ptr<HttpRequest> &request) {
  if(!resource_exists(request->RequestUri())) {
    auto resource_404 = read_404();
    return std::unique_ptr<HttpResponse>(new HttpResponse(HttpStatuses::NotFound, resource_404));
  }

  auto body = read_resource(request->RequestUri());
  std::unique_ptr<HttpResponse> response(new HttpResponse(HttpStatuses::Ok, body));
  return response;
};

bool ContentService::resource_exists(const std::string &resource) {
  boost::filesystem::path full_path( boost::filesystem::current_path() );
  auto full_path_resource = full_path.string() + resource;
  bool exists = boost::filesystem::exists(full_path_resource);
  if(!exists) return false;

  return !boost::filesystem::is_directory(full_path_resource);
};

std::vector<char> ContentService::read_resource(const std::string &resource) {
  boost::filesystem::path full_path( boost::filesystem::current_path() );
  auto full_path_resource = full_path.string() + resource;

  return read_file(full_path_resource);
};

std::vector<char> ContentService::read_404() {
  auto full_path_resource = pages_dir_.string() + boost::filesystem::path::preferred_separator
    + "404.html";
  return read_file(full_path_resource);
};

std::vector<char> ContentService::read_file(const std::string &full_path) {
  std::ifstream input_stream(full_path);
  std::stringstream sstream;
  std::string line;
  while(getline(input_stream, line))
    sstream << line << std::endl;

  auto body_line = sstream.str();
  return std::vector<char>(body_line.begin(), body_line.end());
};

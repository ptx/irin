#ifndef _CONTENT_SERVICE_HEADER_H_
#define _CONTENT_SERVICE_HEADER_H_

#include "boost/filesystem.hpp"
#include "server/http-request.h"
#include "server/http-response.h"
#include <string>
#include <vector>

class ContentService {
  public:
    ContentService(const std::string& pages_dir);
    std::unique_ptr<HttpResponse> Handle(const std::unique_ptr<HttpRequest> &request);
  private:
    boost::filesystem::path pages_dir_;
    bool resource_exists(const std::string &resource);
    std::vector<char> read_resource(const std::string &resource);
    std::vector<char> read_404();
    std::vector<char> read_file(const std::string &full_path);
};

#endif

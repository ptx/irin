#ifndef _IRIN_SERVICE_HEADER_H_
#define _IRIN_SERVICE_HEADER_H_

#include "server/http-response.h"
#include "services/content-service.h"
#include "services/proxy-service.h"
#include <vector>

class IrinService {
  public:
    IrinService(std::vector<std::string> proxy_uris, 
      std::unique_ptr<ContentService> &content_service, 
      std::unique_ptr<ProxyService> &proxy_service);
    std::unique_ptr<HttpResponse> Handle(const std::unique_ptr<HttpRequest> &request);
  private:
    std::vector<std::string> proxy_uris_;
    std::unique_ptr<ContentService> content_service_;
    std::unique_ptr<ProxyService> proxy_service_;
    std::string parse_uri_root(const std::string &request_uri);
};

#endif

#include "services/irin-service.h"

IrinService::IrinService(std::vector<std::string> proxy_uris, 
  std::unique_ptr<ContentService> &content_service, 
  std::unique_ptr<ProxyService> &proxy_service) {
    this->content_service_ = std::move(content_service);
    this->proxy_service_   = std::move(proxy_service);
    this->proxy_uris_.reserve(proxy_uris.size());
    for(const auto elem : proxy_uris)
      this->proxy_uris_.push_back(elem);
};

std::unique_ptr<HttpResponse> IrinService::Handle(const std::unique_ptr<HttpRequest> &request) {
  auto proxy = proxy_uris_[0];
  if(proxy.compare(request->RootUri()) == 0 )
    return proxy_service_->Handle(request);
  
  return content_service_->Handle(request);
};

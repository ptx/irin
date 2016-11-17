#include <boost/format.hpp>
#include <boost/log/trivial.hpp>
#include "server/content-service.h"
#include "server/http-request.h"
#include "server/dispatcher.h"
#include "server/proxy-service.h"
#include <iostream>

Dispatcher::Dispatcher(std::unique_ptr<ContentService> &service, 
  std::unique_ptr<Transporter> &transporter) {
    this->service_     = std::move(service);
    this->transporter_ = std::move(transporter);
};

void Dispatcher::Run() {
  std::unique_ptr<ProxyService> proxy_service(new ProxyService());

  while(true) {
    std::unique_ptr<HttpRequest> request = transporter_->Read();

    std::unique_ptr<HttpResponse> response;
    auto root = request_uri_root(request->RequestUri());
    if(root.compare("/api") == 0)
      response = proxy_service->Handle(request);
    else
      response = service_->Handle(request);

    log_request_response(request->ToString(), response->GetHttpStatus());
    transporter_->Write(response);
  }
};

std::string Dispatcher::request_uri_root(const std::string &uri) {
  bool second_separator_found = false;
  int idx = 0;
  int separator_count = 0;
  size_t uri_size = uri.size();
  while(!second_separator_found && idx < uri_size) {
    if(uri[idx] == '/') {
      ++separator_count;
      if(separator_count == 2)
        second_separator_found = true;
      else
        ++idx;
    } else {
      ++idx;
    }
  }

  if(!second_separator_found)
    return uri;

  return std::string(uri.begin(), uri.begin() + idx);
};

void Dispatcher::log_request_response(const std::string &request_to_string, 
    const HttpStatus &status) {
    auto fmt = boost::format("%1% %2% %3%")
      % request_to_string
      % status.code
      % status.response;
    BOOST_LOG_TRIVIAL(info) << fmt.str();
};

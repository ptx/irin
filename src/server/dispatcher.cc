#include <boost/format.hpp>
#include <boost/log/trivial.hpp>
#include "server/http-request.h"
#include "server/dispatcher.h"
#include "services/content-service.h"
#include "services/proxy-service.h"
#include <iostream>

Dispatcher::Dispatcher(std::unique_ptr<IrinService> &service, 
  std::unique_ptr<Transporter> &transporter) {
    this->service_     = std::move(service);
    this->transporter_ = std::move(transporter);
};

void Dispatcher::Run() {

  while(true) {
    auto optional_request = transporter_->Read();

    std::unique_ptr<HttpResponse> response;
    if(optional_request) {
      std::unique_ptr<HttpRequest> request = std::move(*optional_request);
      response = service_->Handle(request);
      log_request_response(request->ToString(), response->GetHttpStatus());
    } else {
      std::unique_ptr<HttpResponse> bad(new HttpResponse(HttpStatuses::BadRequest));
      response = std::move(bad);
      log_response(std::move(bad));
    }

    transporter_->Write(response);
  }
};

void Dispatcher::log_response(const std::unique_ptr<HttpResponse> response) {

};

void Dispatcher::log_request_response(const std::string &request_to_string, 
    const HttpStatus &status) {
    auto fmt = boost::format("%1% %2% %3%")
      % request_to_string
      % status.code
      % status.response;
    BOOST_LOG_TRIVIAL(info) << fmt.str();
};

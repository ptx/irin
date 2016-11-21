#include <boost/format.hpp>
#include <boost/log/trivial.hpp>
#include "server/http-request.h"
#include "server/dispatcher.h"
#include "services/content-service.h"
#include "services/proxy-service.h"
#include <iostream>

Dispatcher::Dispatcher(pthread_t thread, std::unique_ptr<IrinService> &service, 
  std::unique_ptr<Transporter> &transporter) {
    this->thread_      = thread;
    this->variables_ = ThreadVariables{std::move(service), std::move(transporter)};
};

void Dispatcher::Run() {
  pthread_create(&thread_, NULL, run_dispatcher, (void *) &variables_);
};

void* Dispatcher::run_dispatcher(void* thread_variables) {
  ThreadVariables *variables;
  variables = (ThreadVariables *) thread_variables;
  std::unique_ptr<IrinService> service = std::move(variables->service_);
  std::unique_ptr<Transporter> transporter= std::move(variables->transporter_);

 while(true) {
    auto optional_request = transporter->Read();

    std::unique_ptr<HttpResponse> response;
    if(optional_request) {
      std::unique_ptr<HttpRequest> request = std::move(*optional_request);
      response = service->Handle(request);
      log_request_response(request->ToString(), response->GetHttpStatus());
    } else {
      std::unique_ptr<HttpResponse> bad(new HttpResponse(HttpStatuses::BadRequest));
    }

    transporter->Write(response);
  }
};


void Dispatcher::log_request_response(const std::string &request_to_string, 
    const HttpStatus &status) {
    auto fmt = boost::format("%1% %2% %3%")
      % request_to_string
      % status.code
      % status.response;
    BOOST_LOG_TRIVIAL(info) << fmt.str();
};

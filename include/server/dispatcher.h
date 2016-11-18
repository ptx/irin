#ifndef _DISPATCHER_HEADER_H_
#define _DISPATCHER_HEADER_H_

#include "services/irin-service.h"
#include "server/http-statuses.h"
#include "server/transporter.h"

class Dispatcher {
  public:
    Dispatcher(std::unique_ptr<IrinService> &service, std::unique_ptr<Transporter> &transporter);
    void Run();
  private:
    std::unique_ptr<IrinService> service_;
    std::unique_ptr<Transporter> transporter_;
    void log_request_response(const std::string &request_to_string,
      const HttpStatus &status);
    void log_response(const std::unique_ptr<HttpResponse> response);
};

#endif

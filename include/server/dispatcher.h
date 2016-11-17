#ifndef _DISPATCHER_HEADER_H_
#define _DISPATCHER_HEADER_H_

#include "server/content-service.h"
#include "server/http-statuses.h"
#include "server/transporter.h"

class Dispatcher {
  public:
    Dispatcher(std::unique_ptr<ContentService> &service, std::unique_ptr<Transporter> &transporter);
    void Run();
  private:
    std::unique_ptr<ContentService> service_;
    std::unique_ptr<Transporter> transporter_;
    std::string request_uri_root(const std::string &uri);
    void log_request_response(const std::string &request_to_string,
      const HttpStatus &status);
};

#endif

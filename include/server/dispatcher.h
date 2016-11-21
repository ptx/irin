#ifndef _DISPATCHER_HEADER_H_
#define _DISPATCHER_HEADER_H_

#include <pthread.h>
#include "services/irin-service.h"
#include "server/http-statuses.h"
#include "server/transporter.h"

class Dispatcher {
  public:
    Dispatcher(const pthread_t thread, std::unique_ptr<IrinService> &service, 
        std::unique_ptr<Transporter> &transporter);
    void Run();
  private:
    struct ThreadVariables {
      std::unique_ptr<IrinService> service_;
      std::unique_ptr<Transporter> transporter_;
    };
    pthread_t thread_;
    ThreadVariables variables_;
    static void* run_dispatcher(void* thread_variables);
    std::unique_ptr<IrinService> service_;
    std::unique_ptr<Transporter> transporter_;
    static void log_request_response(const std::string &request_to_string,
      const HttpStatus &status);
};

#endif

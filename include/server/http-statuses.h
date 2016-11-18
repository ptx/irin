#ifndef _HTTP_STATUSES_HEADER_H_
#define _HTTP_STATUSES_HEADER_H_

#include <string>

struct HttpStatus {
  short code;
  std::string response;
};

namespace HttpStatuses {
  const HttpStatus Ok{200, "OK"};
  const HttpStatus BadRequest{400, "BAD REQUEST"};
  const HttpStatus NotFound{404, "NOT FOUND"};
  const HttpStatus InternalServerError{500, "INTERNAL SERVER ERROR"};
  const HttpStatus BadGateway{502, "BAD GATEWAY"};
  static HttpStatus from_code(const std::string &str_code) {
    int code = std::stoi(str_code);
    HttpStatus status;
    switch(code) {
      case 200 : status = Ok;
                 break;
      case 400 : status = BadRequest;
                 break;
      case 404 : status = NotFound;
                 break;
      case 500 : status = InternalServerError;
                 break;
      case 502 : status = BadGateway;
                 break;
      default  : status = InternalServerError;
                 break;
    }
    return status;
  };
}

#endif

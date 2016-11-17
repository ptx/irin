#ifndef _HTTP_STATUSES_HEADER_H_
#define _HTTP_STATUSES_HEADER_H_

#include <string>

struct HttpStatus {
  short code;
  std::string response;
};

namespace HttpStatuses {
  const HttpStatus Ok{200, "OK"};
  const HttpStatus NotFound{404, "NOT FOUND"};
}

#endif

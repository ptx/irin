#ifndef _HTTP_HEADERS_HEADER_H_
#define _HTTP_HEADERS_HEADER_H_

#include <iostream>

struct HttpHeader {
  std::string name;
  std::string value;
};

namespace HttpHeaders {
  const std::string Server        = "Server";
  const std::string ContentType   = "Content-Type";
  const std::string ContentLength = "Content-Length";
  const std::string Connection    = "Connection";
  const std::string Date          = "Date";
  const std::string CacheControl  = "Cache-Control";
  const std::string Status        = "Status";
}

#endif

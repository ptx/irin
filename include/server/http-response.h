#ifndef _HTTP_RESPONSE_HEADER_H_
#define _HTTP_RESPONSE_HEADER_H_

#include <boost/asio.hpp>
#include <boost/optional.hpp>
#include "server/http-statuses.h"
#include "server/http-headers.h"
#include "server/http-versions.h"
#include <string>
#include <vector>

class HttpResponse {
  public:
    HttpResponse(const HttpStatus http_status);
    HttpResponse(const HttpStatus http_status, const std::vector<char>& body);
    HttpResponse(const HttpStatus http_status, const HttpVersion &version, 
      const std::vector<HttpHeader>& headers, const std::vector<char>& body);
    size_t WriteToArray(char array[]);
    HttpStatus GetHttpStatus() { return http_status_; };
  private:
    HttpStatus http_status_;
    HttpVersion http_version_;
    boost::optional<std::vector<char>> body_;
    std::vector<HttpHeader> headers_;
    std::vector<HttpHeader> build_headers();
    std::string build_response_string();
    std::string dateString();
    int body_size();
};

#endif

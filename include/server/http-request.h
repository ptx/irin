#ifndef _HTTP_REQUEST_HEADER_H_
#define _HTTP_REQUEST_HEADER_H_

#include "server/http-headers.h"
#include "server/http-methods.h"
#include "server/http-versions.h"
#include <string>
#include <vector>

class HttpRequest {
  public:
    HttpRequest(std::vector<char> vec);
    HttpRequest(const HttpMethod method, const std::string request_uri,
      const std::vector<HttpHeader> &headers, const HttpVersion version);
    inline HttpMethod Method() { return method_; };
    inline std::string RequestUri() {return request_uri_; };
    inline std::vector<HttpHeader> Headers() { return headers_; };
    inline HttpVersion Version() { return version_; };
    std::string RootUri();
    std::string ToString();
  private:
    std::vector<HttpHeader> headers_;
    HttpMethod method_;
    HttpVersion version_;
    std::string request_uri_;
    void parse_request_line(const std::vector<char> &req_line_chars);
    static std::vector<char> copy_vector_splice(const std::vector<char> &vector, 
        const int start_idx, const int end_idx);
};

#endif

#ifndef _HTTP_REQUEST_HEADER_H_
#define _HTTP_REQUEST_HEADER_H_

#include "server/http-methods.h"
#include "server/http-versions.h"
#include <string>
#include <vector>

class HttpRequest {
  public:
    HttpRequest(std::vector<char> vec);
    inline HttpMethod Method() { return method_; };
    inline std::string RequestUri() {return request_uri_; };
    std::string ToString();
  private:
    HttpMethod method_;
    HttpVersion version_;
    std::string request_uri_;
    void parse_request_line(const std::vector<char> &req_line_chars);
    static std::vector<char> copy_vector_splice(const std::vector<char> &vector, 
        const int start_idx, const int end_idx);
};

#endif

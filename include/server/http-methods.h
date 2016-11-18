#ifndef _HTTP_METHODS_HEADER_H_
#define _HTTP_METHODS_HEADER_H_

#include <string>

struct HttpMethod {
  std::string to_string;
};

namespace HttpMethods {
  const HttpMethod Get{"GET"};
  const HttpMethod Head{"HEAD"};
  const HttpMethod Post{"POST"};
  const HttpMethod Put{"PUT"};
  const HttpMethod Delete{"DELETE"};
  const HttpMethod Trace{"TRACE"};
  const HttpMethod Patch{"PATCH"};

  static HttpMethod from_string(std::string &str) {
    HttpMethod method;
    if(str.compare(HttpMethods::Get.to_string) == 0) method = HttpMethods::Get;
    else if(str.compare(HttpMethods::Head.to_string) == 0) method = HttpMethods::Head;
    else if(str.compare(HttpMethods::Post.to_string) == 0) method = HttpMethods::Post;
    else if(str.compare(HttpMethods::Put.to_string) == 0) method = HttpMethods::Put;
    else if(str.compare(HttpMethods::Delete.to_string) == 0) method = HttpMethods::Delete;
    else if(str.compare(HttpMethods::Trace.to_string) == 0) method = HttpMethods::Trace;
    else if(str.compare(HttpMethods::Patch.to_string) == 0) method = HttpMethods::Patch;
    else throw new std::out_of_range("terribel exception");

    return method;
  };
};

#endif

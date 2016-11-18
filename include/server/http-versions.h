#ifndef _HTTP_VERSIONS_HEADER_H_
#define _HTTP_VERSIONS_HEADER_H_

#include <string>

struct HttpVersion {
  std::string to_string;
};

namespace HttpVersions {
  const HttpVersion HttpVersion_1_0{"HTTP/1.0"};
  const HttpVersion HttpVersion_1_1{"HTTP/1.1"};
  const HttpVersion HttpVersion_2_0{"HTTP/2.0"};

  static HttpVersion from_string(std::string &str) {
    HttpVersion version;
    if(str.compare(HttpVersions::HttpVersion_1_0.to_string) == 0) 
      version = HttpVersions::HttpVersion_1_0;
    else if(str.compare(HttpVersions::HttpVersion_1_1.to_string) == 0) 
      version = HttpVersions::HttpVersion_1_1;
    else if(str.compare(HttpVersions::HttpVersion_2_0.to_string) == 0) 
      version = HttpVersions::HttpVersion_2_0;
    else throw new std::out_of_range("terribel exception");

    return version;
  };
};

#endif

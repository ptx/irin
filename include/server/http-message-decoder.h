#ifndef _HTTP_MESSAGE_DECODER_H_
#define _HTTP_MESSAGE_DECODER_H_

#include <boost/optional.hpp>
#include "server/http-headers.h"
#include "server/http-methods.h"
#include "server/http-request.h"
#include "server/http-statuses.h"
#include "server/http-versions.h"
#include <vector>

class HttpMessageDecoder {
  public:
    HttpMessageDecoder(const std::vector<char> &chars);
    boost::optional<std::unique_ptr<HttpRequest>> DecodeRequest();
    boost::optional<int> DecodeContentLength();
    boost::optional<HttpStatus> DecodeHttpStatus();
    boost::optional<std::vector<HttpHeader>> DecodeHeaders();
    boost::optional<HttpVersion> DecodeVersion();
  private:
    std::vector<char> message_chars_;
    std::string parse_header_line(size_t &start_idx);
    std::vector<HttpHeader> parse_headers(size_t &start_idx);
    std::string& request_line_elem(const std::string &request_line, const int idx);
    std::vector<std::string> split_line(const std::string &line, char delim);
    HttpHeader decode_header_string(const std::string &header_string);
};

#endif

#include <boost/algorithm/string.hpp>
#include <boost/log/trivial.hpp>
#include "exceptions/decoding-exception.h"
#include "server/http-message-decoder.h"
#include "server/http-statuses.h"
#include <sstream>

HttpMessageDecoder::HttpMessageDecoder(const std::vector<char> &chars) {
  this->message_chars_.reserve(chars.size());
  for(const char elem : chars)
    this->message_chars_.push_back(elem);
};

boost::optional<std::unique_ptr<HttpRequest>> HttpMessageDecoder::DecodeRequest() {
  boost::optional<std::unique_ptr<HttpRequest>> http_request;
  try {
    size_t idx = 0;
    std::string request_line = parse_header_line(idx);
    auto method  = HttpMethods::from_string(request_line_elem(request_line, 0));
    auto uri     = request_line_elem(request_line, 1);
    auto version = HttpVersions::from_string(request_line_elem(request_line, 2));
    auto headers = parse_headers(idx);

    std::unique_ptr<HttpRequest> ptr(new HttpRequest(method, uri, headers, version));
    http_request = std::move(ptr);
  } catch (const DecodingException &e) {
    BOOST_LOG_TRIVIAL(error) << e.what();
    return boost::none;
  }
  return http_request;
};

boost::optional<int> HttpMessageDecoder::DecodeContentLength() {
  int content_length = 0;
  try {
    size_t idx = 0;
    std::string request_line = parse_header_line(idx);
    auto headers = parse_headers(idx);
    bool content_header_found = false;
    size_t header_count = headers.size();
    size_t i = 0;
    while(!content_header_found && i < header_count) {
      if(headers[i].name.compare(HttpHeaders::ContentLength) == 0 )
        content_header_found = true;
      else
        ++i;
    }
    if(!content_header_found)
      throw new DecodingException("Content-Length header not found");
    content_length = std::stoi(headers[i].value);

  } catch (const DecodingException &e) {
    BOOST_LOG_TRIVIAL(error) << e.what();
    return boost::none;
  }

  return content_length;
};

boost::optional<HttpStatus> HttpMessageDecoder::DecodeHttpStatus() {
  HttpStatus status;
  try {
    size_t idx = 0;
    std::string request_line = parse_header_line(idx);
    status = HttpStatuses::from_code(request_line_elem(request_line, 1));
  } catch (const DecodingException &e) {
    BOOST_LOG_TRIVIAL(error) << e.what();
    return boost::none;
  }

  return status;
};

boost::optional<std::vector<HttpHeader>> HttpMessageDecoder::DecodeHeaders() {
  std::vector<HttpHeader> headers;
  try {
    size_t idx = 0;
    std::string request_line = parse_header_line(idx);
    headers = parse_headers(idx);
  } catch (const DecodingException &e) {
    BOOST_LOG_TRIVIAL(error) << e.what();
    return boost::none;
  }

  return headers;
};

boost::optional<HttpVersion> HttpMessageDecoder::DecodeVersion() {
  HttpVersion version;
  try {
    size_t idx = 0;
    std::string request_line = parse_header_line(idx);
    version = HttpVersions::from_string(request_line_elem(request_line, 0));
  } catch (const DecodingException &e) {
    BOOST_LOG_TRIVIAL(error) << e.what();
    return boost::none;
  }

  return version;
};

std::vector<HttpHeader> HttpMessageDecoder::parse_headers(size_t &idx) {
  std::vector<HttpHeader> headers;
  
  size_t message_size = message_chars_.size();
  size_t start_idx = idx;
  bool end_of_headers = false;
  while(!end_of_headers) {
    if(idx > message_size)
      throw new DecodingException("<CLRF> header line not found");
    if(message_chars_[idx] == '\r' && message_chars_[idx + 1] == '\n')
      end_of_headers = true;
    else {
      std::string header_line = parse_header_line(idx);
      HttpHeader header = decode_header_string(header_line);
      headers.push_back(header);
    }
  }

  return headers;
};

std::string HttpMessageDecoder::parse_header_line(size_t &idx) {
  size_t message_size = message_chars_.size();
  size_t start_idx = idx;
  if(start_idx > message_size )
    throw new DecodingException("Foo Bar");

  bool carriage_return_found = false;
  bool newline_found = false;
  while(idx < message_size && !carriage_return_found && !newline_found) {
    if(message_chars_[idx] == '\r') {
      carriage_return_found = true;
      if(!newline_found)
        ++idx;
    } else if(message_chars_[idx] == '\n') {
      newline_found = true;
      if(!carriage_return_found)
        ++idx;
    } else {
      ++idx;
    }
  }
  if(!carriage_return_found && !newline_found)
    throw new DecodingException("<CRLF> for Header Line not found");
  
  ++idx;
  return std::string(message_chars_.begin() + start_idx, message_chars_.begin() + idx - 2);
}

std::string& HttpMessageDecoder::request_line_elem(const std::string &request_line,
  const int idx) {
    auto elements = split_line(request_line, ' ');
    return elements[idx];
};

std::vector<std::string> HttpMessageDecoder::split_line(
  const std::string &line, char delim) {
    std::vector<std::string> elems;
    std::stringstream ss;
    ss.str(line);
    std::string elem;
    while(std::getline(ss, elem, delim))
      elems.push_back(elem);
  
    return elems;
};

HttpHeader HttpMessageDecoder::decode_header_string(const std::string &header_string) {
  auto elements = split_line(header_string, ':');
  auto key = elements[0];
  std::string value = "";
  for(size_t idx = 1;idx < elements.size();++idx)
    value += elements[idx] + ":";
  value.pop_back();

  boost::trim(key);
  boost::trim(value);
  return HttpHeader { key, value };
};

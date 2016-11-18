#include <boost/format.hpp>
#include "server/http-methods.h"
#include "server/http-request.h"
#include <iostream>

HttpRequest::HttpRequest(const HttpMethod method, const std::string request_uri,
  const std::vector<HttpHeader> &headers, const HttpVersion http_version) {
    this->method_ = method;
    this->request_uri_ = request_uri;
    this->headers_ = headers;
    this->version_ = http_version;
};

HttpRequest::HttpRequest(std::vector<char> req) {
  bool eol = false;
  int idx = 0;
  while(!eol) {
    if(idx > req.size()) {
      eol = true;
    } else {
      char ch = req[idx];
      if(ch == '\r' || ch =='\n')
        eol = true;
      else
        ++idx;
    }
  }
  auto request_line_vector = copy_vector_splice(req, 0, idx);
  parse_request_line(request_line_vector);
};

std::string HttpRequest::RootUri() {
  std::string uri = request_uri_;
  bool second_separator_found = false;
  int idx = 0;
  int separator_count = 0;
  size_t uri_size = uri.size();
  while(!second_separator_found && idx < uri_size) {
    if(uri[idx] == '/') {
      ++separator_count;
      if(separator_count == 2)
        second_separator_found = true;
      else
        ++idx;
    } else {
      ++idx;
    }
  }

  if(!second_separator_found)
    return uri;

  return std::string(uri.begin(), uri.begin() + idx);
};

void HttpRequest::parse_request_line(const std::vector<char> &req_line_chars) {

  // parse out request method
  int idx = 0;
  bool whitespace_found = false;
  while(!whitespace_found) {
    if(idx > req_line_chars.size())
      throw new std::out_of_range("asdf");
    if(req_line_chars[idx] == ' ')
      whitespace_found = true;
    else
      ++idx;
  }
  std::string method_str(req_line_chars.begin(), req_line_chars.begin() + idx);
  this->method_  = HttpMethods::from_string(method_str);

  // parse out request_uri
  whitespace_found = false;
  ++idx;
  int start_idx = idx;
  while(!whitespace_found) {
    if(idx > req_line_chars.size())
      throw new std::out_of_range("asdf");
    if(req_line_chars[idx] == ' ')
      whitespace_found = true;
    else
      ++idx;
  }
  this->request_uri_ = std::string(req_line_chars.begin() + start_idx, 
      req_line_chars.begin() + idx);

  // parse out http version
  std::string version_str(req_line_chars.begin() + idx + 1, req_line_chars.end());
  this->version_ = HttpVersions::from_string(version_str);
};

std::vector<char> HttpRequest::copy_vector_splice(const std::vector<char> &vector,
  const int start_idx, const int end_idx) {
  std::vector<char> return_vector;
  return_vector.reserve(end_idx - start_idx);
  for(int idx = start_idx;idx < end_idx;++idx) {
    char elem = vector[idx];
    return_vector.push_back(elem);
  }

  return return_vector;
};

std::string HttpRequest::ToString() {
  auto fmt = boost::format("%1% %2%")
    % method_.to_string
    % request_uri_;
  return fmt.str();
};

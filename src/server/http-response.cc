#include <boost/asio.hpp>
#include <boost/format.hpp>
#include <boost/log/trivial.hpp>
#include <boost/optional.hpp>
#include <ctime>
#include "server/http-response.h"
#include "server/http-headers.h"
#include <locale>

HttpResponse::HttpResponse(const HttpStatus http_status) {
  this->http_status_ = http_status;
  this->body_ = boost::none;
};

HttpResponse::HttpResponse(const HttpStatus http_status, const std::vector<char>& body) {
  this->http_status_ = http_status;
  this->body_ = body;
};

std::string HttpResponse::dateString() {
  auto locale = std::locale("");
  auto now = std::time(nullptr);
  auto timeinfo = std::localtime(&now);
  char buffer[80];
  std::strftime(buffer,80,"%a, %d %b %G %H:%M:%S %Z", timeinfo);
  std::string str(buffer);
  return str;
};

std::vector<HttpHeader> HttpResponse::build_headers() {
  auto status_fmt = boost::format("%1% %2%")
    % this->http_status_.code 
    % this->http_status_.response;

  auto server         = HttpHeader{HttpHeaders::Server, "Irin"};
  auto date           = HttpHeader{HttpHeaders::Date, this->dateString()};
  auto content_type   = HttpHeader{HttpHeaders::ContentType, "text/html; chatset=utf-8"};
  auto status         = HttpHeader{HttpHeaders::Status, status_fmt.str()};
  auto cache_control  = HttpHeader{HttpHeaders::CacheControl, "no-cache"};
  auto body_length    = this->body_size();
  auto body_len_str   = (body_length != 0) ? std::to_string(body_length + 1) : "0"; // trailing NULL
  auto content_length = HttpHeader{HttpHeaders::ContentLength, body_len_str};
  auto connection     = HttpHeader{HttpHeaders::Connection, "close"};

  std::vector<HttpHeader> headers;
  headers.reserve(7);
  headers.push_back(server);
  headers.push_back(date);
  headers.push_back(content_type);
  headers.push_back(status);
  headers.push_back(cache_control);
  headers.push_back(content_length);
  headers.push_back(connection);
  return headers;
};

size_t HttpResponse::WriteToArray(char array[]) {
  std::string response_string = build_response_string();

  size_t num_bytes = 0;
  auto string_size = response_string.size(); 
  while(num_bytes < string_size) {
    array[num_bytes] = response_string[num_bytes];
    ++num_bytes;
  }
  ++num_bytes;
  array[num_bytes] = '\0';

  return num_bytes;
};

std::string HttpResponse::build_response_string() {
  auto headers = this->build_headers();
  auto response_line_fmt = boost::format("HTTP/1.1 %1% %2%\r\n")
    % this->http_status_.code 
    % this->http_status_.response;
  auto header_str = response_line_fmt.str();
  for(const auto &header : headers) {
    auto line = header.name + ": " + header.value + "\r\n";
    header_str += line;
  }
  header_str += "\r\n";
  return header_str + this->body();
};

std::string HttpResponse::body() {
  std::string body = "";
  try {
    std::vector<char> body = this->body_.value();
    std::string fbod(body.begin(), body.end());
    return fbod;
  } catch (const boost::bad_optional_access&) {
    return "";
  }
  return body;
};

int HttpResponse::body_size() {
  int size = 0;
  try {
    std::vector<char> body = this->body_.value();
    size = body.size();
  } catch (const boost::bad_optional_access&) {
    size = 0;
  }
  return size;
};

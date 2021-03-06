#ifndef _PROXY_SERVICE_HEADER_H_
#define _PROXY_SERVICE_HEADER_H_

#include "boost/asio.hpp"
#include "server/http-request.h"
#include "server/http-response.h"
#include <string>

using boost::asio::ip::tcp;

class ProxyService {
  public:
    ProxyService(const std::string &host, const std::string &port);
    std::unique_ptr<HttpResponse> Handle(const std::unique_ptr<HttpRequest> &http_request);
  private:
    boost::asio::io_service io_service_;
    tcp::resolver resolver_;
    tcp::socket socket_;
    void build_request(boost::asio::streambuf &tcp_request, const
      std::unique_ptr<HttpRequest> &http_request);
    std::vector<char> decode_response(const boost::asio::streambuf &tcp_response);
    static const size_t kReadBufferSize = 1000000;
    char read_buffer_[kReadBufferSize];
    std::string proxy_host_;
    std::string proxy_port_;
};

#endif

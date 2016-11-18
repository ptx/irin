#ifndef _TRANSPORTER_HEADER_H_
#define _TRANSPORTER_HEADER_H_

#include <boost/asio.hpp>
#include "server/http-request.h"
#include "server/http-response.h"
#include "server/http-methods.h"
#include <vector>

using boost::asio::ip::tcp;

class Transporter {
  public:
    Transporter(short port);
    std::unique_ptr<HttpRequest> Read();
    void Write(const std::unique_ptr<HttpResponse> &response);
  private:
    short port_;
    char data_[2048];
    boost::asio::io_service io_service_;
    tcp::acceptor acceptor_;
    tcp::socket socket_;
    std::vector<char> slice_read_chars(size_t num_chars);
    static const int kBufferSize = 7340032; // 7 MB
    char write_buffer_[kBufferSize];
};

#endif

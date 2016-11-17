#include <boost/asio.hpp>
#include "server/http-request.h"
#include "server/transporter.h"
#include <iostream>

using boost::asio::ip::tcp;

Transporter::Transporter(short port) : 
  acceptor_(io_service_, tcp::endpoint(tcp::v4(), port)),
  socket_(io_service_) {
    this->port_ = port;
  for(int i = 0;i < kBufferSize;++i)
    write_buffer_[i] = ' ';
};

std::unique_ptr<HttpRequest> Transporter::Read() {
  acceptor_.accept(socket_);
  boost::system::error_code error;
  size_t length = socket_.read_some(boost::asio::buffer(data_), error);
  std::vector<char> request_bytes = slice_read_chars(length);
  std::unique_ptr<HttpRequest> req(new HttpRequest(request_bytes));
  return req;
};

void Transporter::Write(const std::unique_ptr<HttpResponse> &response) {
  boost::system::error_code shutdown_error;
  boost::system::error_code ignored_error;

  // write response to the wire
  auto num_bytes    = response->WriteToArray(write_buffer_);
  auto const_buffer = boost::asio::const_buffers_1(write_buffer_, num_bytes);
  boost::asio::write(socket_, const_buffer, ignored_error);

  // close off connection
  socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both, shutdown_error);
  boost::system::error_code socket_close_err;
  socket_.close(socket_close_err);
};

std::vector<char> Transporter::slice_read_chars(size_t num_chars) {
  std::vector<char> chars;
  chars.reserve(num_chars);
  for(int idx = 0;idx < num_chars;++idx)
    chars.push_back(data_[idx]);
  return chars;
};

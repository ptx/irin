#include "boost/format.hpp"
#include <boost/log/trivial.hpp>
#include "server/http-message-decoder.h"
#include "services/proxy-service.h"

ProxyService::ProxyService() : resolver_(io_service_), socket_(io_service_) {
  for(size_t i = 0;i < kReadBufferSize;++i)
    read_buffer_[i] = ' ';
};

std::unique_ptr<HttpResponse> ProxyService::Handle(const 
  std::unique_ptr<HttpRequest> &http_request) {
    // prepare connection
    tcp::resolver::query query("127.0.0.1", "9000");
    boost::asio::io_service io;
    tcp::resolver resolv(io);
    tcp::resolver::iterator endpoint_iterator = resolv.resolve(query);
    tcp::socket socket(io);
    boost::asio::connect(socket, endpoint_iterator);

    // build raw request
    boost::asio::streambuf tcp_request;
    build_request(tcp_request, http_request);

    // Send the request.
    boost::asio::write(socket, tcp_request);

    // read headers
    boost::asio::streambuf response;
    auto bytes_read = boost::asio::read_until(socket, response, "\r\n\r\n");
    std::istream is(&response);
    is.readsome(read_buffer_, bytes_read);
    std::vector<char> header_chars;
    for(size_t i = 0;i < bytes_read;++i)
      header_chars.push_back(read_buffer_[i]);


    std::unique_ptr<HttpResponse> http_response;
    std::unique_ptr<HttpMessageDecoder> header_decoder(new HttpMessageDecoder(header_chars));
    auto o_content_length = header_decoder->DecodeContentLength(); 
    if(o_content_length) {
      int content_length = *o_content_length;
      is.readsome(read_buffer_, content_length); 
      std::vector<char> body_chars;
      for(size_t i = 0;i < content_length;++i)
        body_chars.push_back(read_buffer_[i]);

      auto o_http_status = header_decoder->DecodeHttpStatus();
      auto o_http_version = header_decoder->DecodeVersion();
      auto o_headers = header_decoder->DecodeHeaders();
      if(o_http_status && o_http_version && o_headers) {
        auto status  = *o_http_status;
        auto version = *o_http_version;
        auto headers = *o_headers;
        std::unique_ptr<HttpResponse> response(new HttpResponse(status, version,
          headers, body_chars));
        http_response = std::move(response);
      } else {
        std::unique_ptr<HttpResponse> bad_gateway(new HttpResponse(HttpStatuses::BadGateway));
        http_response = std::move(bad_gateway);
      }
    } else {
      std::unique_ptr<HttpResponse> bad_gateway(new HttpResponse(HttpStatuses::BadGateway));
      http_response = std::move(bad_gateway);
    }

    boost::system::error_code shutdown_error;
    boost::system::error_code ignored_error;
    socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, shutdown_error);
    boost::system::error_code socket_close_err;
    socket.close(socket_close_err);

    return http_response;
};

void ProxyService::build_request(boost::asio::streambuf &tcp_request, const
  std::unique_ptr<HttpRequest> &http_request) {

    std::ostream request_stream(&tcp_request);

    auto request_line_fmt = boost::format("%1% %2% %3%\r\n")
      % http_request->Method().to_string
      % http_request->RequestUri()
      % http_request->Version().to_string;
    request_stream << request_line_fmt.str();

    auto headers = http_request->Headers();
    for(const auto header : headers) {
      auto fmt = boost::format("%1%: %2%\r\n") % header.name % header.value;
      request_stream << fmt.str();
    }
    request_stream << "\r\n";
};

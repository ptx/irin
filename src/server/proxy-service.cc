#include "boost/format.hpp"
#include <boost/log/trivial.hpp>
#include "server/proxy-service.h"

std::unique_ptr<HttpResponse> ProxyService::Handle(const 
  std::unique_ptr<HttpRequest> &http_request) {
    BOOST_LOG_TRIVIAL(info) << "Entering ProxyService::Handle()";

    boost::asio::streambuf tcp_request;
    build_request(tcp_request, http_request);

    tcp::resolver::query query("127.0.0.1", "9000");
  //auto resolved = resolver_.resolve(query);
  //std::cout << "queried" << std::endl;
  //boost::asio::connect(socket_.lowest_layer(), resolver_.resolve(query));
  //std::cout << "connected" << std::endl;
  //socket_.lowest_layer().set_option(tcp::no_delay(true));

  boost::asio::io_service io;
  tcp::resolver resolv(io);
  tcp::resolver::iterator endpoint_iterator = resolv.resolve(query);
  tcp::socket socket(io);
  boost::asio::connect(socket, endpoint_iterator);


  // Send the request.
  boost::asio::write(socket, tcp_request);
  boost::asio::streambuf response;
  boost::asio::read_until(socket, response, "\r\n");

  // read status line
  std::istream response_stream(&response);
  std::string http_version;
  response_stream >> http_version;
  unsigned int status_code;
  response_stream >> status_code;
  std::string status_message;
  std::cout << http_version << " " << status_code << " " << status_message << std::endl;

  if (response.size() > 0)
    std::cout << &response;
  // read headers
  boost::asio::read_until(socket, response, "\r\n\r\n");
  std::string header;
  while (std::getline(response_stream, header) && header != "\r")
    std::cout << header << "\n";
  std::cout << "\n";

  // read the rest
  boost::system::error_code error;
  while (boost::asio::read(socket, response, boost::asio::transfer_at_least(1), error))
    std::cout << &response;

  std::unique_ptr<HttpResponse> resp(new HttpResponse(HttpStatuses::NotFound));
  return resp;
};

void ProxyService::build_request(boost::asio::streambuf &tcp_request, const
  std::unique_ptr<HttpRequest> &http_request) {

    std::ostream request_stream(&tcp_request);
    //auto formatter = boost::format("POST /api/status.json_token=%2% HTTP/1.1\r\n") % kFacebookGraphVersion % access_token_;

    request_stream << "GET /api/status.json HTTP/1.1\r\n";
    request_stream << "User-Agent: irin-proxy\r\n";
    request_stream << "Content-Type: application/json\r\n";
   // request_stream << "Content-Length: " << json_string_.length() << "\r\n";
    request_stream << "Connection: close\r\n\r\n";
};

#include <boost/format.hpp>
#include <boost/log/trivial.hpp>
#include "boost/program_options.hpp"
#include <csignal>
#include <pthread.h>
#include "server/dispatcher.h"
#include "server/transporter.h"
#include "services/watcher.h"
#include "services/content-service.h"
#include "services/irin-service.h"
#include "services/proxy-service.h"


std::string build_log_info(const int &port, const std::string &dir);

namespace {
  static const size_t kSuccess = 0;
  static const size_t kErrorInCommandLine = 1;
  static const size_t kErrorUnhandledException = 2;
  static const std::string kVersionNumber = "0.0.2";
}

pthread_t server_thread;
pthread_t watcher_thread;

void signal_handler(int sig_num) {
  BOOST_LOG_TRIVIAL(fatal) << "SIGTERM received, shutting down....";

  void* result;
  pthread_join(server_thread, &result);
  pthread_join(watcher_thread, &result);

  exit(0);
};

int main(int argc, char** argv) {
  signal(SIGINT, signal_handler);

  try {
    namespace po = boost::program_options;
    po::options_description desc("Options");
    desc.add_options()
      ("help,h", "Print help message")
      ("directory,d", po::value<std::string>()->required(), "Required Absolute PATH for the pages directory")
      ("version,v", "Print the version number")
      ("proxy-host", po::value<std::string>()->required(), "Required host to proxy")
      ("proxy-port", po::value<std::string>()->required(), "required port to proxy")
      ("port,p", "Port on which traffic will be served. DEFAULT: 8000");
    po::variables_map vm;
    try {
      po::store(po::parse_command_line(argc, argv, desc), vm);
      if(vm.count("proxy-host") && vm.count("proxy-port") && vm.count("directory")) {
        std::string proxy_host = vm["proxy-host"].as<std::string>();
        std::string proxy_port = vm["proxy-port"].as<std::string>();
        std::string pages_dir = vm["directory"].as<std::string>();
        int port = 8000;

        BOOST_LOG_TRIVIAL(info) << build_log_info(port, pages_dir);

        std::vector<std::string> proxy_uris;
        proxy_uris.push_back("/api");
        std::unique_ptr<ContentService> content_service(new ContentService(pages_dir));
        std::unique_ptr<ProxyService> proxy_service(new ProxyService(proxy_host, proxy_port));
        std::unique_ptr<IrinService> service(new IrinService(proxy_uris, content_service,
          proxy_service));
        std::unique_ptr<Transporter> transporter(new Transporter(port));
        std::unique_ptr<Dispatcher> dispatcher(new Dispatcher(server_thread, service, transporter));
        std::unique_ptr<Watcher> watcher(new Watcher(watcher_thread, "public"));

        dispatcher->Run();
        watcher->Run();
        while(true) {
          // await SIGTERM
        }
      } else if(vm.count("proxy-host") && vm.count("proxy-port") && vm.count("directory") && 
          vm.count("port")) {
          std::string proxy_host = vm["proxy-host"].as<std::string>();
          std::string proxy_port = vm["proxy-port"].as<std::string>();
          std::string pages_dir = vm["directory"].as<std::string>();
          int port = vm["port"].as<int>();

          BOOST_LOG_TRIVIAL(info) << build_log_info(port, pages_dir);
          std::vector<std::string> proxy_uris;
          proxy_uris.push_back("/api");
          std::unique_ptr<ContentService> content_service(new ContentService(pages_dir));
          std::unique_ptr<ProxyService> proxy_service(new ProxyService(proxy_host, proxy_port));
          std::unique_ptr<IrinService> service(new IrinService(proxy_uris, content_service,
            proxy_service));
          std::unique_ptr<Transporter> transporter(new Transporter(port));
          std::unique_ptr<Dispatcher> dispatcher(new Dispatcher(server_thread, service, transporter));

          dispatcher->Run(); 
      } else if(vm.count("help")) {
        std::cout << "irin - a simple webserver and transpiler to help build React applications"
          << std::endl;
        std::cout << std::endl;
        std::cout << desc << std::endl;
        return kSuccess;
      } else if(vm.count("version")) {
        std::cout << "version " << kVersionNumber << std::endl;
        std::cout << "built " << __DATE__ << " " << __TIME__ << std::endl;
        return kSuccess;
      }
      po::notify(vm);
    } catch(po::error& error) {
      std::cerr << "ERROR: " << error.what() << std::endl << std::endl;
      std::cerr << desc << std::endl;
      return kErrorInCommandLine;
    }
  } catch (std::exception &ex) {
    BOOST_LOG_TRIVIAL(fatal) << ex.what();
    return kErrorUnhandledException;
  }

  return kSuccess;
};

std::string build_log_info(const int &port, const std::string &dir) {
  auto info_fmt = boost::format("Serving on port %1% with pages at %2%")
    % port % dir;
  return info_fmt.str();
};

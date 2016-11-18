#include <boost/format.hpp>
#include <boost/log/trivial.hpp>
#include "boost/program_options.hpp"
#include "server/dispatcher.h"
#include "server/transporter.h"

std::string build_log_info(const int &port, const std::string &dir);

namespace {
  const size_t kSuccess = 0;
  const size_t kErrorInCommandLine = 1;
  const size_t kErrorUnhandledException = 2;
  const std::string kVersionNumber = "0.0.1";
}

int main(int argc, char** argv) {

  try {
    namespace po = boost::program_options;
    po::options_description desc("Options");
    desc.add_options()
      ("help,h", "Print help message")
      ("directory,d", po::value<std::string>()->required(), "Required Absolute PATH for the pages directory")
      ("version,v", "Print the version number")
      ("port,p", "Port on which traffic will be served. DEFAULT: 8000");
    po::variables_map vm;
    try {
      po::store(po::parse_command_line(argc, argv, desc), vm);
      if(vm.count("directory")) {
        std::string pages_dir = vm["directory"].as<std::string>();
        int port = 8000;

        BOOST_LOG_TRIVIAL(info) << build_log_info(port, pages_dir);
        std::unique_ptr<ContentService> service(new ContentService(pages_dir));
        std::unique_ptr<Transporter> transporter(new Transporter(port));
        std::unique_ptr<Dispatcher> dispatcher(new Dispatcher(service, transporter));

        dispatcher->Run();
      } else if(vm.count("directory") && vm.count("port")) {
        std::string pages_dir = vm["directory"].as<std::string>();
        int port = vm["port"].as<int>();

        BOOST_LOG_TRIVIAL(info) << build_log_info(port, pages_dir);
        std::unique_ptr<ContentService> service(new ContentService(pages_dir));
        std::unique_ptr<Transporter> transporter(new Transporter(port));
        std::unique_ptr<Dispatcher> dispatcher(new Dispatcher(service, transporter));

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

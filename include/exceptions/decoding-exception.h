#ifndef _DECODING_EXCEPTION_HEADER_H_
#define _DECODING_EXCEPTION_HEADER_H_ 

#include <exception>
#include <iostream>

class DecodingException : public std::runtime_error {
  public:
    DecodingException(std::string reason) : std::runtime_error(reason) {};
    virtual const char* what() const throw() { return std::runtime_error::what(); };
};

#endif

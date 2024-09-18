#pragma once
#include <iostream>

#define LOG \
  (std::clog << __FILE__ << ":" << __LINE__ << " " << __FUNCTION__ << ": ")

#define FATAL(msg)                                                           \
  {                                                                          \
    throw std::runtime_error(std::string{} + __FILE__ + ":" +                \
                             std::to_string(__LINE__) + " " + __FUNCTION__ + \
                             ": " + msg);                                    \
  }
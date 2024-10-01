#pragma once
#include <iostream>

#define LOG \
  (std::clog << __FILE__ << ":" << __LINE__ << " " << __FUNCTION__ << "(): ")

#define LOGf(fmt, ...)                              \
  {                                                 \
    int sz = snprintf(NULL, 0, fmt, __VA_ARGS__);   \
    std::vector<char> str(sz + 2);                  \
    snprintf(str.data(), sz + 1, fmt, __VA_ARGS__); \
    LOG << str.data() << "\n";                      \
  }

#define FATAL(msg)                                                           \
  {                                                                          \
    throw std::runtime_error(std::string{} + __FILE__ + ":" +                \
                             std::to_string(__LINE__) + " " + __FUNCTION__ + \
                             "(): " + msg);                                  \
  }

// The top four bits of a byte
static uint8_t topFour(uint8_t x) {
  return (x & 0xf0) >> 4;
}
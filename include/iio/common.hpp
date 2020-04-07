#ifndef IIO_COMMON_HPP_
#define IIO_COMMON_HPP_

#include "compiler.hpp"

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <utility>

namespace iio {

IIO_CONSTEXPR bool littlendian() {
#ifdef _WIN32
  return true;
#else
  return __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__;
#endif
}
IIO_CONSTEXPR bool bigendian() {
#ifdef _WIN32
  return false;
#else
  return __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__;
#endif
}
template <typename T>
IIO_CONSTEXPR inline void swap_endian(T *f, std::uint8_t len = 1) {
  for (char *c = reinterpret_cast<char *>(f); len--; c += sizeof(T)) {
    if
      IIO_CONSTEXPR(sizeof(T) == 2) { std::swap(c[0], c[1]); }
    else if
      IIO_CONSTEXPR(sizeof(T) == 4) {
        std::swap(c[0], c[3]);
        std::swap(c[1], c[2]);
      }
    else if
      IIO_CONSTEXPR(sizeof(T) == 8) {
        std::swap(c[0], c[7]);
        std::swap(c[1], c[6]);
        std::swap(c[2], c[5]);
        std::swap(c[3], c[4]);
      }
  }
}

template <typename T> inline bool fwrite(FILE *fd, const T *buf) {
  return std::fwrite(buf, sizeof(T), 1, fd) == 1;
}
template <typename T> inline bool fwrite(FILE *fd, const T buf) {
  return fwrite(fd, &buf);
}
template <typename T> IIO_CONSTEXPR inline std::size_t channel_bytes() {
  return sizeof(T);
}
template <typename T>
IIO_CONSTEXPR inline std::size_t pixel_bytes(const std::size_t &channels) {
  return channels * sizeof(T);
}
template <typename T>
IIO_CONSTEXPR inline std::size_t image_bytes(const std::size_t &width,
                                             const std::size_t &height,
                                             const std::size_t &channels) {
  return width * height * channels * sizeof(T);
}
} // namespace iio

#endif // IIO_COMMON_HPP_

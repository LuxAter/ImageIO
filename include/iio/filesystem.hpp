#ifndef IIO_FILESYSTEM_HPP_
#define IIO_FILESYSTEM_HPP_

#include <cstdarg>
#include <cstdio>
#include <filesystem>

#include "compiler.hpp"
#include "spec.hpp"

namespace iio {
namespace fs {

FILE *fopen(const std::string_view &filename, bool create_dir = true);
FILE *fopen(const std::string_view &filename, const std::string_view &ext,
            bool create_dir = true);

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

inline bool fseek(FILE *file, std::int64_t offset, int whence) {
#ifdef _MSC_VER
  return _fseeki64(file, __int64(offset), whence) != -1;
#else
  return fseeko(file, offset, whence) != -1;
#endif
}
inline std::uint64_t ftell(FILE *file) {
#ifdef _MSC_VER
  return _ftelli64(file);
#else
  return ftello(file);
#endif
}

template <typename T> inline bool fwrite(FILE *fd, const T *buf) {
  return std::fwrite(buf, sizeof(T), 1, fd) == 1;
}
inline bool fprint_raw(FILE *__restrict __s, const char *__restrict __format,
                       ...) {
  char buf[255];
  std::va_list args;
  va_start(args, __format);
  int ret = std::vsnprintf(buf, 255, __format, args);
  va_end(args);
  return ret > 0 && std::fwrite(buf, sizeof(buf), 1, __s) == 1;
}
template <typename T>
inline bool fwrite_raw_binary(FILE *fd, const T *data, const std::size_t stride,
                              const ImageSpec &spec) {
  std::uint8_t val;
  bool res = true;
  for (std::size_t x = 0; x < spec.width && res;) {
    val = 0;
    for (int bit = 8 * sizeof(T) - 1; bit >= 0 && x < spec.width && res;
         x++, bit--) {
      val += (data[x * stride] ? (1 << bit) : 0);
    }
    res &= (sizeof(std::uint8_t) ==
            std::fwrite(&val, sizeof(std::uint8_t), 1, fd));
  }
  return res;
}
template <typename T>
inline bool fwrite_raw(FILE *fd, const T *data, const std::size_t stride,
                       const ImageSpec &spec, std::size_t max_val) {
  std::uint8_t byte;
  std::size_t pixel, val;
  bool res = true;
  for (std::size_t x = 0; x < spec.width && res; ++x) {
    pixel = x * stride;
    for (std::uint8_t c = 0; c < spec.channels && res; ++c) {
      val = data[pixel + c];
      val = val * max_val / std::numeric_limits<T>::max();
      if (sizeof(T) == 2) {
        byte = static_cast<std::uint8_t>(val >> 8);
        res &= (sizeof(std::uint8_t) ==
                std::fwrite(&byte, sizeof(std::uint8_t), 1, fd));
        byte = static_cast<std::uint8_t>(val & 0xff);
        res &= (sizeof(std::uint8_t) ==
                std::fwrite(&byte, sizeof(std::uint8_t), 1, fd));
      } else {
        byte = static_cast<std::uint8_t>(val);
        res &= (sizeof(std::uint8_t) ==
                std::fwrite(&byte, sizeof(std::uint8_t), 1, fd));
      }
    }
  }
  return res;
}
} // namespace fs
} // namespace iio

#endif // IIO_FILESYSTEM_HPP_

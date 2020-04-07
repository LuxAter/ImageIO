#ifndef IIO_WRITER_HPP_
#define IIO_WRITER_HPP_

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <limits>
#include <memory>
#include <string_view>
#include <type_traits>
#include <utility>
#include <cstdarg>

#include "compiler.hpp"
#include "filesystem.hpp"
#include "spec.hpp"
#include "type_convert.hpp"

namespace iio {

template <typename Native> class ImageWriter {
public:
  ImageWriter(const std::string_view &filename, const ImageSpec &spec)
      : fp(fopen(filename)), spec(spec) {}
  virtual ~ImageWriter() {}
  virtual const char *format_name() const = 0;
  virtual bool is_valid() const { return fp != nullptr; }

  virtual bool open() = 0;
  virtual bool close() = 0;

  virtual bool write_scanline_native(const std::size_t &y,
                                     const Native *data) = 0;
  virtual bool write_tile_native(const std::size_t &x, const std::size_t &y,
                                 const Native *data) = 0;
  virtual bool write_scanlines_native(const std::size_t &y_start,
                                      const std::size_t &y_end,
                                      const Native *data) {
    bool res = true;
    for (std::size_t y = y_start; y < y_end && res; ++y) {
      res &= write_scanline_native(y, &data[y * spec.width * spec.channels]);
    }
    return res;
  }
  virtual bool write_image_native(const Native *data) {
    bool res = true;
    for (std::size_t y = 0; y < spec.height && res; ++y) {
      res &= write_scanline_native(y, &data[y * spec.width * spec.channels]);
    }
    return res;
  }

  template <typename T>
  inline
      typename std::enable_if<std::is_convertible<T, Native>::value, bool>::type
      write_scanline(const std::size_t &y, const T *data) {
    Native *native_data = convert<T, Native>(spec.width * spec.channels, data);
    bool result = write_scanline_native(y, native_data);
    cfree(data, native_data);
    return result;
  }
  template <typename T>
  inline
      typename std::enable_if<std::is_convertible<T, Native>::value, bool>::type
      write_scanlines(const std::size_t &y, const T *data) {
    Native *native_data = convert<T, Native>(spec.width * spec.channels, data);
    bool result = write_scanlines_native(y, native_data);
    cfree(data, native_data);
    return result;
  }

  template <typename T>
  inline
      typename std::enable_if<std::is_convertible<T, Native>::value, bool>::type
      write_tile(const std::size_t &x, const std::size_t &y, const T *data) {
    Native *native_data = convert<T, Native>(
        spec.tile_width * spec.tile_height * spec.channels, data);
    bool result = write_tile_native(x, y, native_data);
    cfree(data, native_data);
    return result;
  }

  template <typename T>
  inline
      typename std::enable_if<std::is_convertible<T, Native>::value, bool>::type
      write_image(const T *data) {
    Native *native_data =
        convert<T, Native>(spec.height * spec.width * spec.channels, data);
    bool result = write_image_native(native_data);
    cfree(data, native_data);
    return result;
  }

protected:
  FILE *fp;
  const ImageSpec spec;
};

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
inline bool fprint(FILE *__restrict __s, const char *__restrict __format, ...) {
  std::va_list args;
  va_start(args, __format);
  int ret = std::vfprintf(__s, __format, args);
  va_end(args);
  return ret > 0;
}
inline bool fwrite_raw_binary(FILE *fd, const std::uint8_t *data,
                              const std::size_t stride, const ImageSpec &spec) {
  std::uint8_t val;
  bool res = true;
  for (std::size_t x = 0; x < spec.width && res;) {
    val = 0;
    for (int bit = 7; bit >= 0 && x < spec.width && res; x++, bit--) {
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

} // namespace iio

#endif /* end of include guard: IIO_WRITER_HPP_ */

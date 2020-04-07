#ifndef IIO_WRITER_HPP_
#define IIO_WRITER_HPP_

#include <cstdarg>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <limits>
#include <memory>
#include <string_view>
#include <type_traits>
#include <utility>

#include "compiler.hpp"
#include "filesystem.hpp"
#include "spec.hpp"
#include "type_convert.hpp"

#define WRITE_IMAGE_FUNC(ft)                                                   \
  template <typename T, std::uint8_t Channels, typename U>                     \
  inline bool write_##ft(const std::string_view &filename,                     \
                         const ImageSpec &spec, const U *data) {               \
    std::unique_ptr<ImageWriter<T>> writer =                                   \
        open_##ft<T, Channels>(filename, spec);                                \
    if (writer == nullptr)                                                     \
      return false;                                                            \
    writer->write_image(data);                                                 \
    return true;                                                               \
  }                                                                            \
  template <typename T, std::uint8_t Channels, typename U>                     \
  inline bool write_##ft(const std::string_view &filename,                     \
                         const std::size_t &img_width,                         \
                         const std::size_t &img_height, const U *data) {       \
    std::unique_ptr<ImageWriter<T>> writer =                                   \
        open_##ft<T, Channels>(filename, ImageSpec(img_width, img_height));    \
    if (writer == nullptr)                                                     \
      return false;                                                            \
    writer->write_image(data);                                                 \
    return true;                                                               \
  }

namespace iio {

template <typename Native> class ImageWriter {
public:
  ImageWriter(const std::string_view &filename, const ImageSpec &spec)
      : fp(fs::fopen(filename)), spec(spec) {}
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

} // namespace iio

#endif /* end of include guard: IIO_WRITER_HPP_ */

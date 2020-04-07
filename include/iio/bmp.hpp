#ifndef IIO_BMP_HPP_
#define IIO_BMP_HPP_

#include <cstdint>
#include <memory>
#include <string_view>
#include <type_traits>

#include "writer.hpp"

namespace iio {

class BmpWriter : public ImageWriter<std::uint8_t> {
public:
  BmpWriter(const std::string_view &filename, const ImageSpec &spec)
      : ImageWriter<std::uint8_t>(filename, spec) {
    open();
  }
  ~BmpWriter() { close(); }

  virtual const char *format_name() const IIO_OVERRIDE { return "bmp"; }

  virtual bool open() IIO_OVERRIDE;
  virtual bool close() IIO_OVERRIDE;
  virtual bool write_scanline_native(const std::size_t &y,
                                     const std::uint8_t *data) IIO_OVERRIDE;
  virtual bool write_tile_native(const std::size_t &x, const std::size_t &y,
                                 const std::uint8_t *data) IIO_OVERRIDE;
};

template <typename T, std::uint8_t Channels>
inline typename std::enable_if<std::is_same<T, std::uint8_t>::value &&
                                   (Channels == 1 || Channels == 3 ||
                                    Channels == 4),
                               std::unique_ptr<BmpWriter>>::type
open_bmp(const std::string_view &filename, const ImageSpec &spec) {
  return std::make_unique<BmpWriter>(filename, spec);
}
} // namespace iio

#endif // IIO_BMP_HPP_

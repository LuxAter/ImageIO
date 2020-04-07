#ifndef IIO_HPP_
#define IIO_HPP_

#include <cstdint>
#include <memory>
#include <string_view>

#include "spec.hpp"
#include "writer.hpp"

#include "bmp.hpp"
#include "pnm.hpp"

namespace iio {

#define OPEN_WRITER_FT(ft)                                                     \
  for (std::size_t i = 0; i < ft##_extension_count; ++i) {                     \
    if (ft##_extensions[i] == fext)                                            \
      writer = open_##ft<T, Channels>(filename, spec);                         \
  }

template <typename T = std::uint8_t, std::uint8_t Channels = 3>
std::unique_ptr<ImageWriter<T>> open_image(const std::string_view &filename,
                                           const ImageSpec &spec) {
  std::string_view fext = filename.substr(filename.rfind('.') + 1);
  std::unique_ptr<ImageWriter<T>> writer = nullptr;
  OPEN_WRITER_FT(bmp);
  OPEN_WRITER_FT(pnm);
  return writer;
}

template <typename T = std::uint8_t, std::uint8_t Channels = 3, typename U>
inline bool write_image(const std::string_view &filename, const ImageSpec &spec,
                        const U *data) {
  std::unique_ptr<ImageWriter<T>> writer =
      open_image<T, Channels>(filename, spec);
  if (writer == nullptr)
    return false;
  writer->write_image(data);
  return true;
}
template <typename T = std::uint8_t, std::uint8_t Channels = 3, typename U>
inline bool write_image(const std::string_view &filename,
                        const std::size_t &img_width,
                        const std::size_t &img_height, const U *data) {
  std::unique_ptr<ImageWriter<T>> writer =
      open_image<T, Channels>(filename, ImageSpec(img_width, img_height));
  if (writer == nullptr)
    return false;
  writer->write_image(data);
  return true;
}

} // namespace iio

#endif // IIO_HPP_

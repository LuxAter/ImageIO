#ifndef IIO_BMP_HPP_
#define IIO_BMP_HPP_

#include <cassert>
#include <cstdint>
#include <functional>
#include <string_view>
#include <type_traits>

#include "common.hpp"
#include "type_convert.hpp"

namespace iio {

bool write_bmp_uint8(const std::string_view &filename, const std::size_t &width,
                     const std::size_t &height, const std::uint8_t &channels,
                     const std::uint8_t *data);

template <typename T = std::uint8_t, typename U = std::uint8_t>
typename std::enable_if<std::is_convertible<U, T>::value &&
                            std::is_same<T, std::uint8_t>::value,
                        bool>::type
write_bmp(const std::string_view &filename, const std::size_t &width,
          const std::size_t &height, const std::uint8_t &channels,
          const U *data) {
  assert(channels == 1 || channels == 3 || channels == 4);
  assert(image_bytes<T>(width, height, channels) < 1UL << 32);
  T *converted_data = convert<U, T>(width * height * channels, data);
  bool result =
      write_bmp_uint8(filename, width, height, channels, converted_data);
  free(converted_data);
  return result;
}
} // namespace iio

#endif // IIO_BMP_HPP_

#ifndef IIO_HPP_
#define IIO_HPP_

#include <cstdint>
#include <functional>
#include <string_view>
#include <type_traits>

namespace iio {

template <typename T = std::uint8_t, typename U = std::uint8_t>
typename std::enable_if<std::is_convertible<U, T>::value, bool>::type
write_ppm(const std::string_view &filename, const std::size_t &width,
          const std::size_t &height, const U *data) {}
template <typename T = std::uint8_t, typename U = std::uint8_t>
typename std::enable_if<std::is_convertible<U, T>::value, bool>::type
write_jpeg(const std::string_view &filename, const std::size_t &width,
           const std::size_t &height, const U *data) {}
template <typename T = std::uint8_t, typename U = std::uint8_t>
typename std::enable_if<std::is_convertible<U, T>::value, bool>::type
write_exr(const std::string_view &filename, const std::size_t &width,
          const std::size_t &height, const U *data) {}
template <typename T = std::uint8_t, typename U = std::uint8_t>
typename std::enable_if<std::is_convertible<U, T>::value, bool>::type
write_png(const std::string_view &filename, const std::size_t &width,
          const std::size_t &height, const U *data) {}
template <typename T = std::uint8_t, typename U = std::uint8_t>
typename std::enable_if<std::is_convertible<U, T>::value, bool>::type
write_tiff(const std::string_view &filename, const std::size_t &width,
           const std::size_t &height, const U *data) {}

template <typename T = std::uint8_t, typename U = std::uint8_t>
bool write_image(const std::string_view &filename, const std::size_t &width,
                 const std::size_t &height, const U *data) {}

} // namespace iio

#endif // IIO_HPP_

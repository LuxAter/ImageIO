#ifndef IIO_TYPE_CONVERT_HPP_
#define IIO_TYPE_CONVERT_HPP_

#include <cstdlib>
#include <type_traits>

namespace iio {
template <typename From, typename To>
typename std::enable_if<std::is_convertible<From, To>::value, To *>::type
convert(const std::size_t &size, const From *in_data) {
  To *out_data = static_cast<To *>(std::malloc(size * sizeof(To)));
  for (std::size_t i = 0; i < size; ++i) {
    out_data[i] = static_cast<To>(in_data[i]);
  }
  return out_data;
}
} // namespace iio

#endif // IIO_TYPE_CONVERT_HPP_

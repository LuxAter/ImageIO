#ifndef IIO_TYPE_CONVERT_HPP_
#define IIO_TYPE_CONVERT_HPP_

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <type_traits>

namespace iio {
template <typename From, typename To>
typename std::enable_if<std::is_same<From, To>::value, To *>::type
convert(const std::size_t &, const From *in_data) {
  return in_data;
}
template <typename From, typename To>
typename std::enable_if<std::is_same<To, std::uint8_t>::value &&
                            std::is_floating_point<From>::value,
                        To *>::type
convert(const std::size_t &size, const From *in_data) {
  To *out_data = static_cast<To *>(std::malloc(size * sizeof(To)));
  for (std::size_t i = 0; i < size; ++i) {
    out_data[i] = static_cast<To>(in_data[i] * 0xff);
  }
  return out_data;
}
template <typename From, typename To>
typename std::enable_if<std::is_same<To, std::uint16_t>::value &&
                            std::is_floating_point<From>::value,
                        To *>::type
convert(const std::size_t &size, const From *in_data) {
  To *out_data = static_cast<To *>(std::malloc(size * sizeof(To)));
  for (std::size_t i = 0; i < size; ++i) {
    out_data[i] = static_cast<To>(in_data[i] * 0xffff);
  }
  return out_data;
}
template <typename From, typename To>
typename std::enable_if<std::is_same<To, std::uint32_t>::value &&
                            std::is_floating_point<From>::value,
                        To *>::type
convert(const std::size_t &size, const From *in_data) {
  To *out_data = static_cast<To *>(std::malloc(size * sizeof(To)));
  for (std::size_t i = 0; i < size; ++i) {
    out_data[i] = static_cast<To>(in_data[i] * 0x00ffffff);
  }
  return out_data;
}
template <typename From, typename To>
typename std::enable_if<std::is_convertible<From, To>::value &&
                            !std::is_same<From, To>::value &&
                            !(std::is_same<To, std::uint8_t>::value &&
                              std::is_floating_point<From>::value) &&
                            !(std::is_same<To, std::uint16_t>::value &&
                              std::is_floating_point<From>::value) &&
                            !(std::is_same<To, std::uint32_t>::value &&
                              std::is_floating_point<From>::value),
                        To *>::type
convert(const std::size_t &size, const From *in_data) {
  To *out_data = static_cast<To *>(std::malloc(size * sizeof(To)));
  for (std::size_t i = 0; i < size; ++i) {
    out_data[i] = static_cast<To>(in_data[i]);
  }
  return out_data;
}

template <typename From, typename To> void cfree(const From *a, To *b) {
  if (reinterpret_cast<const void *>(a) != reinterpret_cast<const void *>(b))
    free(b);
}
} // namespace iio

#endif // IIO_TYPE_CONVERT_HPP_

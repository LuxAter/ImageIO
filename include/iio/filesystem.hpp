#ifndef IIO_FILESYSTEM_HPP_
#define IIO_FILESYSTEM_HPP_

#include <cstdio>
#include <filesystem>

namespace iio {
FILE *fopen(const std::string_view &filename, bool create_dir = true);
FILE *fopen(const std::string_view &filename, const std::string_view &ext,
           bool create_dir = true);
} // namespace iio

#endif // IIO_FILESYSTEM_HPP_

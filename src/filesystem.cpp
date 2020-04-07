#include "iio/filesystem.hpp"
#include <filesystem>

FILE *iio::fopen(const std::string_view &filename, bool create_dir) {
  std::filesystem::path filepath(filename);
  filepath = std::filesystem::absolute(filepath);
  if (create_dir && !std::filesystem::is_directory(filepath.parent_path())) {
    std::filesystem::create_directories(filepath.parent_path());
  }
  return std::fopen(filepath.c_str(), "wb");
}
FILE *iio::fopen(const std::string_view &filename, const std::string_view &ext,
                 bool create_dir) {
  std::filesystem::path filepath(filename);
  filepath = std::filesystem::absolute(filepath);
  filepath.replace_extension(ext);
  if (create_dir && !std::filesystem::is_directory(filepath.parent_path())) {
    std::filesystem::create_directories(filepath.parent_path());
  }
  return std::fopen(filepath.c_str(), "wb");
}

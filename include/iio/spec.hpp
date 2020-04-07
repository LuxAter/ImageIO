#ifndef IIO_SPEC_HPP_
#define IIO_SPEC_HPP_

#include <cstdint>

namespace iio {
struct ImageSpec {
  ImageSpec(const std::size_t &xres, const std::size_t &yres,
            const std::size_t &channels = 3) noexcept
      : width(xres), height(yres), channels(channels), tile_width(0),
        tile_height(0) {}

  template <typename Native> inline std::size_t channel_bytes() const noexcept {
    return sizeof(Native);
  }
  template <typename Native> inline std::size_t pixel_bytes() const noexcept {
    return channels * sizeof(Native);
  }
  template <typename Native>
  inline std::size_t scanline_bytes() const noexcept {
    return width * channels * sizeof(Native);
  }
  template <typename Native> inline std::size_t tile_bytes() const noexcept {
    return tile_width * tile_height * channels * sizeof(Native);
  }
  template <typename Native> inline std::size_t image_bytes() const noexcept {
    return width * height * channels * sizeof(Native);
  }
  inline std::size_t scanline_pixels() const noexcept { return width; }
  inline std::size_t tile_pixels() const noexcept {
    return tile_width * tile_height;
  }
  inline std::size_t image_pixels() const noexcept { return width * height; }

  std::size_t width, height, channels;
  std::size_t tile_width, tile_height;
};
} // namespace iio

#endif /* end of include guard: IIO_SPEC_HPP_ */

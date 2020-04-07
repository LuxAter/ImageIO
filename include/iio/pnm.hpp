#ifndef IIO_PNM_HPP_
#define IIO_PNM_HPP_

#include <cstdint>
#include <memory>
#include <string_view>
#include <type_traits>
#include <vector>

#include "compiler.hpp"
#include "writer.hpp"

namespace iio {

constexpr inline const char *pnm_extensions[] = {"ppm", "pgm", "pbm", "pnm"};
constexpr inline std::size_t pnm_extension_count =
    sizeof(iio::pnm_extensions) / sizeof(iio::pnm_extensions[0]);

template <typename Native> class PnmWriter : public ImageWriter<Native> {
public:
  PnmWriter(const std::string_view &filename, const ImageSpec &spec,
            const std::uint8_t bits_per_pixel = sizeof(Native) * 8)
      : ImageWriter<Native>(filename, spec), bits_per_pixel(bits_per_pixel) {
    if (bits_per_pixel == 1) {
      pnm_type = 4;
    } else if (spec.channels == 1) {
      pnm_type = 5;
    } else {
      pnm_type = 6;
    }
    open();
  }
  ~PnmWriter() { close(); }

  virtual const char *format_name() const IIO_OVERRIDE { return "pnm"; }

  virtual bool open() IIO_OVERRIDE {
    if (!this->is_valid())
      return false;
    std::fprintf(this->fp, "P%u %lu %lu ", pnm_type, this->spec.width,
                 this->spec.height);
    if (pnm_type != 4) {
      std::fprintf(this->fp, "%d\n", (1 << bits_per_pixel) - 1);
    }
    image_start = fs::ftell(this->fp);
    return true;
  }
  virtual bool close() IIO_OVERRIDE {
    if (!this->is_valid())
      return true;
    std::fclose(this->fp);
    this->fp = nullptr;
    return true;
  }
  virtual bool write_scanline_native(const std::size_t &y,
                                     const Native *data) IIO_OVERRIDE {
    if (!this->is_valid())
      return false;
    if (!fs::fseek(this->fp,
                   image_start +
                       y * this->spec.template scanline_bytes<Native>(),
                   SEEK_SET))
      return false;
    if (pnm_type == 4) {
      return fs::fwrite_raw_binary(this->fp, data, this->spec.channels,
                                   this->spec);
    } else {
      return fs::fwrite_raw(this->fp, data, this->spec.channels, this->spec,
                            (1 << bits_per_pixel) - 1);
    }
  }
  virtual bool write_tile_native(const std::size_t &, const std::size_t &,
                                 const Native *) IIO_OVERRIDE {
    return false;
  }

private:
  std::uint8_t bits_per_pixel;
  std::uint8_t pnm_type;
  std::uint8_t image_start;
};

template <typename T, std::uint8_t Channels>
inline typename std::enable_if<(std::is_same<T, std::uint8_t>::value ||
                                std::is_same<T, std::uint16_t>::value) &&
                                   (Channels == 1 || Channels == 3),
                               std::unique_ptr<PnmWriter<T>>>::type
open_pnm(const std::string_view &filename, const ImageSpec &spec) {
  ImageSpec new_spec(spec);
  new_spec.channels = Channels;
  return std::make_unique<PnmWriter<T>>(filename, new_spec);
}
template <typename T, std::uint8_t Channels>
inline typename std::enable_if<(!std::is_same<T, std::uint8_t>::value &&
                                !std::is_same<T, std::uint16_t>::value) ||
                                   (Channels != 1 && Channels != 3),
                               std::unique_ptr<PnmWriter<T>>>::type
open_pnm(const std::string_view &, const ImageSpec &) {
  return nullptr;
}

WRITE_IMAGE_FUNC(pnm);
} // namespace iio

#endif // IIO_PNM_HPP_

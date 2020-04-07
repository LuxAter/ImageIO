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

class PnmWriter : public ImageWriter<std::uint8_t> {
public:
  PnmWriter(const std::string_view &filename, const ImageSpec &spec,
            const std::uint8_t bits_per_pixel = sizeof(std::uint8_t) * 8)
      : ImageWriter<std::uint8_t>(filename, spec),
        bits_per_pixel(bits_per_pixel) {
    if (bits_per_pixel == 1) {
      pnm_type = 1;
    } else if (spec.channels == 1) {
      pnm_type = 2;
    } else {
      pnm_type = 3;
    }
    open();
  }
  ~PnmWriter() { close(); }

  virtual const char *format_name() const IIO_OVERRIDE { return "pnm"; }

  virtual bool open() IIO_OVERRIDE;
  virtual bool close() IIO_OVERRIDE;
  virtual bool write_scanline_native(const std::size_t &y,
                                     const std::uint8_t *data) IIO_OVERRIDE;
  virtual bool write_tile_native(const std::size_t &x, const std::size_t &y,
                                 const std::uint8_t *data) IIO_OVERRIDE {
    return false;
  }

  constexpr static const char *extensions[] = {"ppm", "pgm", "pbm", "pnm"};
  constexpr static std::size_t extension_count =
      sizeof(iio::PnmWriter::extensions) /
      sizeof(iio::PnmWriter::extensions[0]);

private:
  std::uint8_t bits_per_pixel;
  std::uint8_t pnm_type;

  std::vector<std::uint8_t> tilebuffer;
};

template <typename T, std::uint8_t Channels>
inline typename std::enable_if<std::is_same<T, std::uint8_t>::value &&
                                   (Channels == 1 || Channels == 3),
                               std::unique_ptr<PnmWriter>>::type
open_pnm(const std::string_view &filename, const ImageSpec &spec) {
  ImageSpec new_spec(spec);
  new_spec.channels = Channels;
  return std::make_unique<PnmWriter>(filename, new_spec);
}
} // namespace iio

#endif // IIO_PNM_HPP_

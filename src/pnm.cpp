#include "iio/pnm.hpp"

#include <cassert>
#include <cstdint>
#include <memory>
#include <string_view>
#include <type_traits>

#include "writer.hpp"

bool iio::PnmWriter::open() {
  if (!is_valid())
    return false;
  fprint(fp, "P%lu %lu\n", spec.width, spec.height);
  if (pnm_type != 1) {
    fprint(fp, "%lu\n", (1 << bits_per_pixel) - 1);
  }
  if (spec.tile_width != 0 && spec.tile_height != 0)
    tilebuffer.resize(spec.image_bytes<std::uint8_t>());
  return true;
}
bool iio::PnmWriter::close() {
  if (!is_valid())
    return true;
  bool result = true;
  if (spec.tile_width != 0 && spec.tile_height != 0) {
    assert(tilebuffer.size() != 0);
    result &= write_scanlines_native(0, spec.height, &tilebuffer[0]);
    std::vector<std::uint8_t>().swap(tilebuffer);
  }
  std::fclose(fp);
  fp = nullptr;
  return true;
}
bool iio::PnmWriter::write_scanline_native(const std::size_t &y,
                                           const std::uint8_t *data) {
  if (!is_valid())
    return false;
  if (pnm_type == 1) {
    return fwrite_raw_binary(fp, data, 1, spec);
  } else {
    return fwrite_raw(fp, data, 1, spec, (1 << bits_per_pixel) - 1);
  }
}

#ifndef IIO_BMP_HPP_
#define IIO_BMP_HPP_

#include <cstdint>
#include <memory>
#include <string_view>
#include <type_traits>
#include <vector>

#include "filesystem.hpp"
#include "writer.hpp"

namespace iio {

constexpr inline const char *bmp_extensions[] = {"bmp"};
constexpr inline std::size_t bmp_extension_count =
    sizeof(iio::bmp_extensions) / sizeof(iio::bmp_extensions[0]);

constexpr inline std::uint8_t bmp_header_size = 14;
constexpr inline std::uint8_t bmp_windows_v3 = 40;
constexpr inline std::uint16_t bmp_magic_bm = 0x4D42;

template <typename Native> class BmpWriter : public ImageWriter<Native> {
public:
  BmpWriter(const std::string_view &filename, const ImageSpec &spec)
      : ImageWriter<Native>(filename, spec),
        scanline_size(((spec.width * spec.channels) + 3) & ~3) {
    assert(spec.template image_bytes<Native>() + bmp_header_size +
               bmp_windows_v3 <
           std::int64_t(1) << 32);
    open();
  }
  ~BmpWriter() { close(); }

  virtual const char *format_name() const IIO_OVERRIDE { return "bmp"; }

  virtual bool open() IIO_OVERRIDE {
    std::uint16_t magic = bmp_magic_bm;
    std::uint32_t data_size = static_cast<std::size_t>(this->spec.width) *
                              this->spec.height * this->spec.channels;
    std::uint32_t fsize = data_size + bmp_header_size + bmp_windows_v3;
    std::uint32_t offset = bmp_header_size + bmp_windows_v3;
    std::uint16_t res = 0;
    if (fs::bigendian()) {
      fs::swap_endian(&magic);
      fs::swap_endian(&fsize);
      fs::swap_endian(&offset);
    }
    if (!fs::fwrite(this->fp, &magic) || !fs::fwrite(this->fp, &fsize) ||
        !fs::fwrite(this->fp, &res) || !fs::fwrite(this->fp, &res) ||
        !fs::fwrite(this->fp, &offset)) {
      return false;
    }
    std::uint32_t size = bmp_windows_v3;
    std::uint32_t iwidth = this->spec.width;
    std::uint32_t iheight = this->spec.height;
    std::uint16_t cplanes = 1;
    std::uint32_t compression = 0;
    std::uint16_t bpp = this->spec.channels << 3;
    std::uint32_t isize =
        this->spec.width * this->spec.height * this->spec.channels;
    std::uint32_t hres = 0;
    std::uint32_t vres = 0;
    std::uint32_t cpalete = 0;
    std::uint32_t important = 0;
    if (fs::bigendian()) {
      fs::swap_endian(&size);
      fs::swap_endian(&iwidth);
      fs::swap_endian(&iheight);
      fs::swap_endian(&cplanes);
      fs::swap_endian(&compression);
      fs::swap_endian(&bpp);
      fs::swap_endian(&isize);
      fs::swap_endian(&hres);
      fs::swap_endian(&vres);
      fs::swap_endian(&cpalete);
      fs::swap_endian(&important);
    }
    if (!fs::fwrite(this->fp, &size) || !fs::fwrite(this->fp, &iwidth) ||
        !fs::fwrite(this->fp, &iheight) || !fs::fwrite(this->fp, &cplanes) ||
        !fs::fwrite(this->fp, &bpp) || !fs::fwrite(this->fp, &compression) ||
        !fs::fwrite(this->fp, &isize) || !fs::fwrite(this->fp, &hres) ||
        !fs::fwrite(this->fp, &vres) || !fs::fwrite(this->fp, &cpalete) ||
        !fs::fwrite(this->fp, &important)) {
      return false;
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
  virtual bool write_scanline_native(const std::size_t &iy,
                                     const Native *data) IIO_OVERRIDE {
    if (!this->is_valid())
      return false;
    std::size_t y = this->spec.height - iy - 1;
    std::size_t scanline_offset = y * scanline_size;
    fs::fseek(this->fp, image_start + scanline_offset, SEEK_SET);
    std::vector<std::uint8_t> buf;
    buf.reserve(scanline_size);
    buf.assign(reinterpret_cast<const std::uint8_t *>(data),
               reinterpret_cast<const std::uint8_t *>(data) +
                   this->spec.template scanline_bytes<Native>());
    buf.resize(scanline_size, 0);
    if (this->spec.channels >= 3) {
      for (std::size_t i = 0; i < buf.size() - 2; i += this->spec.channels) {
        std::swap(buf[i], buf[i + 2]);
      }
    }
    return fwrite(&buf[0], 1, buf.size(), this->fp) == buf.size();
  }
  virtual bool write_tile_native(const std::size_t &, const std::size_t &,
                                 const Native *) IIO_OVERRIDE {
    return false;
  }

private:
  std::uint32_t scanline_size;
  std::uint8_t image_start;
};

template <typename T, std::uint8_t Channels>
inline typename std::enable_if<(std::is_same<T, std::uint8_t>::value) &&
                                   (Channels == 3 || Channels == 4),
                               std::unique_ptr<BmpWriter<T>>>::type
open_bmp(const std::string_view &filename, const ImageSpec &spec) {
  ImageSpec new_spec(spec);
  new_spec.channels = Channels;
  return std::make_unique<BmpWriter<T>>(filename, spec);
}
template <typename T, std::uint8_t Channels>
inline typename std::enable_if<(!std::is_same<T, std::uint8_t>::value) ||
                                   (Channels != 3 && Channels != 4),
                               std::unique_ptr<BmpWriter<T>>>::type
open_bmp(const std::string_view &, const ImageSpec &) {
  return nullptr;
}
WRITE_IMAGE_FUNC(bmp);

} // namespace iio

#endif // IIO_BMP_HPP_

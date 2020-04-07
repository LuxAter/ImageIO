#include "iio/bmp.hpp"

#include "common.hpp"
#include "compiler.hpp"
#include "iio/filesystem.hpp"

static IIO_CONSTEXPR uint16_t BMP_HEADER_SIZE = 14;

static IIO_CONSTEXPR uint16_t OS2_V1 = 12;
static IIO_CONSTEXPR uint16_t WINDOWS_V3 = 40;
static IIO_CONSTEXPR uint16_t WINDOWS_V4 = 108;
static IIO_CONSTEXPR uint16_t WINDOWS_V5 = 124;

static IIO_CONSTEXPR uint16_t MAGIC_BM = 0x4D42;
static IIO_CONSTEXPR uint16_t MAGIC_BA = 0x4142;
static IIO_CONSTEXPR uint16_t MAGIC_CI = 0x4943;
static IIO_CONSTEXPR uint16_t MAGIC_CP = 0x5043;
static IIO_CONSTEXPR uint16_t MAGIC_PT = 0x5450;

bool iio::write_bmp_uint8(const std::string_view &filename,
                          const std::size_t &width, const std::size_t &height,
                          const std::uint8_t &channels,
                          const std::uint8_t *data) {
  FILE *fd = open(filename, "bmp");
  if (fd == nullptr)
    return false;

  uint16_t magic = MAGIC_BM;
  uint32_t data_size = static_cast<uint32_t>(width) * height * channels;
  uint32_t file_size = data_size + BMP_HEADER_SIZE + WINDOWS_V3;
  uint32_t fsize = file_size;
  uint16_t res1 = 0;
  uint16_t res2 = 0;
  uint32_t offset = BMP_HEADER_SIZE + WINDOWS_V3;

  if (bigendian()) {
    swap_endian(&magic);
    swap_endian(&fsize);
    swap_endian(&offset);
  }

  if (!fwrite(fd, &magic) || !fwrite(fd, &fsize) || !fwrite(fd, &res1) ||
      !fwrite(fd, &res2) || !fwrite(fd, &offset)) {
    std::fclose(fd);
    return false;
  }

  uint32_t size = WINDOWS_V3;
  uint32_t iwidth = width;
  uint32_t iheight = height;
  uint16_t cplanes = 1;
  uint16_t bpp = channels << 3;
  uint32_t compression = 0;
  uint32_t isize = width * height * channels;
  uint32_t hres = width;
  uint32_t vres = height;
  uint32_t cpalete = 0;
  uint32_t important = 0;

  if (bigendian()) {
    swap_endian(&size);
    swap_endian(&iwidth);
    swap_endian(&iheight);
    swap_endian(&cplanes);
    swap_endian(&bpp);
    swap_endian(&compression);
    swap_endian(&isize);
    swap_endian(&hres);
    swap_endian(&vres);
    swap_endian(&cpalete);
    swap_endian(&important);
  }

  if (!fwrite(fd, &size) || !fwrite(fd, &iwidth) || !fwrite(fd, &iheight) ||
      !fwrite(fd, &cplanes) || !fwrite(fd, &bpp) || !fwrite(fd, &compression) ||
      !fwrite(fd, &isize) || !fwrite(fd, &hres) || !fwrite(fd, &vres) ||
      !fwrite(fd, &cpalete) || !fwrite(fd, &important)) {
    std::fclose(fd);
    return false;
  }

  std::uint32_t padded_scanline_size = ((width * channels) + 3) & ~3;

  for (std::size_t dy = 0; dy < height; ++dy) {
    std::size_t y = height - dy - 1;
    std::vector<std::uint8_t> buf;
    buf.reserve(padded_scanline_size);
    buf.assign(data[y * width], data[(y + 1) * width]);
    buf.resize(padded_scanline_size, 0);
    if (channels >= 3) {
      for (std::size_t i = 0; i < buf.size() - 2; i += channels) {
        std::swap(buf[i], buf[i + 2]);
      }
    }
    std::size_t byte_count = fwrite(&buf[0], 1, buf.size(), fd);
    if (byte_count != buf.size())
      return false;
  }

  std::fclose(fd);
  fd = nullptr;
  return true;
}

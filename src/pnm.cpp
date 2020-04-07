#include "iio/pnm.hpp"

#include "common.hpp"
#include "compiler.hpp"
#include "iio/filesystem.hpp"

bool iio::write_pnm_uint8(const std::string_view &filename,
                          const std::size_t &width, const std::size_t &height,
                          const std::uint8_t &channels,
                          const std::uint8_t *data) {
  FILE *fd = open(filename, "pnm");
  if (fd == nullptr)
    return false;

  fwrite(fd, 'P');
  if(channels == 1) fwrite(fd, 5);
  else fwrite(fd, 6);
  fwrite(fd, '\n');
  fwrite(fd, &width);
  fwrite(fd, ' ');
  fwrite(fd, &height);
  fwrite(fd, 0xff);

  std::fclose(fd);
  fd = nullptr;
  return true;
}

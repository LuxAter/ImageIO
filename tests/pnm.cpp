#include <catch2/catch.hpp>

#include "iio/pnm.hpp"
#include "iio/writer.hpp"

#define IMG_SIZE 10

TEST_CASE("pnm") {
  SECTION("rgb") {
    float img[IMG_SIZE][IMG_SIZE][3];
    for (std::size_t y = 0; y < IMG_SIZE; ++y) {
      for (std::size_t x = 0; x < IMG_SIZE; ++x) {
        img[y][x][0] = 1.0;
        img[y][x][1] = 0.2;
        img[y][x][2] = 0.5;
      }
    }
    auto output = iio::open_pnm<std::uint8_t, 3>(
        "ppm_test.ppm", iio::ImageSpec(IMG_SIZE, IMG_SIZE));
    output->write_image(&img[0][0][0]);
  }
}

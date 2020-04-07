#include <catch2/catch.hpp>

#include "iio/bmp.hpp"
#include "iio/writer.hpp"

#define IMG_SIZE 1000

static float img[IMG_SIZE][IMG_SIZE][3];

TEST_CASE("bmp") {
  for (std::size_t y = 0; y < IMG_SIZE; ++y) {
    for (std::size_t x = 0; x < IMG_SIZE; ++x) {
      img[y][x][0] = y / static_cast<float>(IMG_SIZE);
      img[y][x][1] = 0.2;
      img[y][x][2] = x / static_cast<float>(IMG_SIZE);
    }
  }
  SECTION("rgb8") {
    REQUIRE(iio::write_bmp<std::uint8_t, 3>("rgb8.bmp", IMG_SIZE, IMG_SIZE,
                                            &img[0][0][0]) == true);
  }
}

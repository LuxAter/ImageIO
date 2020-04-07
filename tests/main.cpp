#define CATCH_CONFIG_MAIN
#define CATCH_CONFIG_ENABLE_BENCHMARKING

#include <catch2/catch.hpp>

#include "iio/iio.hpp"

#define IMG_SIZE 5000

static float img[IMG_SIZE][IMG_SIZE][3];

TEST_CASE("generic") {
  for (std::size_t y = 0; y < IMG_SIZE; ++y) {
    for (std::size_t x = 0; x < IMG_SIZE; ++x) {
      img[y][x][0] = 0.2;
      img[y][x][1] = y / static_cast<float>(IMG_SIZE);
      img[y][x][2] = x / static_cast<float>(IMG_SIZE);
    }
  }
  SECTION("ppm8") {
    REQUIRE(iio::write_image<std::uint8_t, 3>("generic8.ppm", IMG_SIZE,
                                              IMG_SIZE, &img[0][0][0]) == true);
  }
  SECTION("ppm16") {
    REQUIRE(iio::write_image<std::uint16_t, 3>(
                "generic16.ppm", IMG_SIZE, IMG_SIZE, &img[0][0][0]) == true);
  }
  SECTION("bmp8") {
    REQUIRE(iio::write_image<std::uint8_t, 3>("generic8.bmp", IMG_SIZE,
                                              IMG_SIZE, &img[0][0][0]) == true);
  }
}

// end catch_default_main.hpp

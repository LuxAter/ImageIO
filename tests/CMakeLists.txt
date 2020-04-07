# ##############################################################################
# SOURCE FILES
# ##############################################################################
set(SOURCES main.cpp pnm.cpp)

# ##############################################################################
# TARGET
# ##############################################################################
add_executable(unit-tests ${SOURCES})
target_include_directories(unit-tests PUBLIC ${ImageIO_SOURCE_DIR}/tests)
set_target_properties(unit-tests PROPERTIES CXX_STANDARD 17
                      CXX_STANDARD_REQUIRED YES CXX_EXTENSIONS OFF
                      RUNTIME_OUTPUT_DIRECTORY_DEBUG "${ImageIO_BINARY_DIR}")
                    target_link_libraries(unit-tests iio::iio Catch2::Catch2)

add_test(NAME unit-tests COMMAND unit-tests)

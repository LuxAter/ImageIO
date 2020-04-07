 
# ##############################################################################
# PROJECT
# ##############################################################################
cmake_minimum_required(VERSION 3.11.0 FATAL_ERROR)
project(ImageIO VERSION 3.0.0)
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  set(ImageIO_MAIN_PROJECT TRUE)
endif()

# ##############################################################################
# BUILD MODE
# ##############################################################################
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE
      "Debug"
      CACHE STRING "Build Mode" FORCE)
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release")
endif()
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)
  if(EXISTS "${CMAKE_CURRENT_BINARY_DIR}/compile_commands.json")
    execute_process(
      COMMAND ${CMAKE_COMMAND} -E copy_if_different
              ${CMAKE_CURRENT_BINARY_DIR}/compile_commands.json
              ${CMAKE_CURRENT_SOURCE_DIR}/compile_commands.json)
  endif()
endif()

# ##############################################################################
# OPTIONS
# ##############################################################################
option(ImageIO_BUILD_DOCS "Compiles documentation" ${ImageIO_MAIN_PROJECT})
option(ImageIO_BUILD_TESTS "Compiles Unit Test exectuable" ${ImageIO_MAIN_PROJECT})
if(NOT CMAKE_SYSTEM_NAME STREQUAL "Windows")
  option(ImageIO_BUILD_SHARED_LIBS "Compile shared libraries" TRUE)
  option(ImageIO_BUILD_STATIC_LIBS "Compile static libraries" FALSE)
else()
  option(ImageIO_BUILD_STATIC_LIBS "Compile static libraries" TRUE)
  set(ImageIO_BUILD_SHARED_LIBS FALSE)
endif()

option(ImageIO_STATIC_ANALYSIS "Use static analysis tools" ${ImageIO_MAIN_PROJECT})
set(ImageIO_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR})
set(ImageIO_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

# ##############################################################################
# COMPILE FLAGS
# ##############################################################################
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -DNDEBUG")
if(NOT MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wextra")
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DNOMINMAX")
endif()

# ##############################################################################
# STATIC ANALYSIS
# ##############################################################################
if(ImageIO_STATIC_ANALYSIS OR CMAKE_BUILD_TYPE STREQUAL "Release")
  find_program(CLANG_TIDY_EXE "clang-tidy")
  if(CLANG_TIDY_EXE)
    message(STATUS "Found clang-tidy ${CLANG_TIDY_EXE}")
    set(CLANG_TIDY_CMD "${CLANG_TIDY_EXE}" "--checks=*,-modernize-use-trailing-return-type")
    if(ImageIO_FIX)
      list(APPEND CLANG_TIDY_CMD "--fix")
    endif()
  else()
    message(STATUS "Could not find clang-tidy") 
  endif()
  find_program(CPPCHECK_EXE "cppcheck")
  if(CPPCHECK_EXE)
    message(STATUS "Found cppcheck ${CPPCHECK_EXE}")
    set(CPPCHECK_CMD "${CPPCHECK_EXE}" "--force" "--std=c++17" "--enable=warning" "--enable=style" "--enable=performance" "--enable=portability")
  else()
    message(STATUS "Could not find cppcheck") 
  endif()
endif()

# ##############################################################################
# DEPENDENCIES
# ##############################################################################
include(WriteCompilerDetectionHeader)
add_subdirectory(external)
set(ImageIO_COMPILERS MSVC GNU Clang Intel AppleClang)
write_compiler_detection_header(
  FILE
  ${CMAKE_CURRENT_SOURCE_DIR}/include/iio/compiler.hpp
  PREFIX
  IIO 
  COMPILERS
  ${ImageIO_COMPILERS}
  FEATURES
  cxx_constexpr
  cxx_deleted_functions
  cxx_extern_templates
  cxx_final
  cxx_noexcept
  cxx_override
  cxx_alignas
  cxx_alignof
  cxx_nullptr
  cxx_static_assert
  cxx_attribute_deprecated
  cxx_thread_local)

# ##############################################################################
# SOURCE FILES
# ##############################################################################
set(SOURCES src/iio.cpp src/filesystem.cpp src/writer.cpp src/pnm.cpp)

# ##############################################################################
# STATIC LIBRARY
# ##############################################################################
if(SPECULA_BUILD_STATIC_LIBS)
  add_library(iio-static STATIC ${SOURCES})
  add_library(iio::static ALIAS iio-static)

  target_include_directories(iio-static PUBLIC ${ImageIO_SOURCE_DIR}/include)
  target_include_directories(iio-static PRIVATE ${ImageIO_SOURCE_DIR}/include/iio)

  set_target_properties(iio-static PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED YES CXX_EXTENSIONS OFF)
  set_target_properties(iio-static PROPERTIES CXX_CLANG_TIDY "${CLANG_TIDY_CMD}" CXX_CPPCHECK "${CPPCHECK_CMD}")

  target_link_libraries(iio-static PUBLIC)
  install(TARGETS iio-static ARCHIVE DESTINATION lib)
endif()

# ##############################################################################
# SHARED LIBRARY
# ##############################################################################
if(ImageIO_BUILD_SHARED_LIBS)
  add_library(iio-shared SHARED ${SOURCES})
  add_library(iio::shared ALIAS iio-shared)

  target_include_directories(iio-shared PUBLIC ${ImageIO_SOURCE_DIR}/include)
  target_include_directories(iio-shared PRIVATE ${ImageIO_SOURCE_DIR}/include/iio)

  set_target_properties(iio-shared PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED YES CXX_EXTENSIONS OFF)
  set_target_properties(iio-shared PROPERTIES CXX_CLANG_TIDY "${CLANG_TIDY_CMD}" CXX_CPPCHECK "${CPPCHECK_CMD}")

  target_link_libraries(iio-shared PUBLIC)
  install(TARGETS iio-shared LIBRARY DESTINATION lib)
endif()

# ##############################################################################
# ALIASED LIBRARY
# ##############################################################################
if(ImageIO_BUILD_SHARED_LIBS)
  add_library(iio::iio ALIAS iio-shared)
elseif(ImageIO_BUILD_STATIC_LIBS)
  add_library(iio::iio ALIAS iio-static)
endif()



# ##############################################################################
# TEST EXECUTABLE 
# ##############################################################################
if(ImageIO_BUILD_TESTS)
  enable_testing()
  add_subdirectory(tests)
endif()

# ##############################################################################
# DOCUMENTATION
# ##############################################################################
if(ImageIO_BUILD_DOCS)
  find_package(Doxygen)
  if(Doxygen_FOUND)
    add_subdirectory(docs)
  endif()
endif()

# ##############################################################################
# PACKAGING
# ##############################################################################
if(CMAKE_BUILD_TYPE STREQUAL "Release")
  set(CPACK_GENERATOR "TGZ;ZIP")
  set(CPACK_PACKAGE_VENDOR "LuxAter")
  set(CPACK_PACKAGE_NAME "${PROJECT_NAME}")
  set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
  set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
  set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
  set(CPACK_RESOURCE_FILE_README ${CMAKE_CURRENT_SOURCE_DIR}/README.md)
  set(CPACK_RESOURCE_FILE_LICENSE ${CMAKE_CURRENT_SOURCE_DIR}/LICENSE)
  set(CPACK_PACKAGE_FILE_NAME "${PROJECT_NAME}-${CMAKE_SYSTEM_NAME}")
  set(CPACK_MONOLITHIC_INSTALL TRUE)
  include(CPack)
endif()

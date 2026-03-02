cmake_minimum_required(VERSION 3.14)
project(utf8 VERSION 1.0.0 LANGUAGES C)

set(CMAKE_C_STANDARD 99)

# Static library
add_library(utf8_static STATIC src/utf8.c)
target_include_directories(utf8_static PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>)
set_target_properties(utf8_static PROPERTIES OUTPUT_NAME utf8)

# Shared library
add_library(utf8_shared SHARED src/utf8.c)
target_include_directories(utf8_shared PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>)
set_target_properties(utf8_shared PROPERTIES
  OUTPUT_NAME utf8
  VERSION ${PROJECT_VERSION}
  SOVERSION ${PROJECT_VERSION_MAJOR})

# Install
include(GNUInstallDirs)

install(TARGETS utf8_static utf8_shared
  EXPORT utf8Targets
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

install(FILES include/utf8.h
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(EXPORT utf8Targets
  FILE utf8Targets.cmake
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/utf8)

# CMake package config
include(CMakePackageConfigHelpers)

configure_package_config_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/cmake/utf8Config.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/utf8Config.cmake
  INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/utf8)

write_basic_package_version_file(
  ${CMAKE_CURRENT_BINARY_DIR}/utf8ConfigVersion.cmake
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY SameMajorVersion)

install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/utf8Config.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/utf8ConfigVersion.cmake
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/utf8)

# pkg-config
set(PREFIX ${CMAKE_INSTALL_PREFIX})
set(LIBDIR ${CMAKE_INSTALL_FULL_LIBDIR})
set(INCLUDEDIR ${CMAKE_INSTALL_FULL_INCLUDEDIR})
configure_file(utf8.pc.in utf8.pc @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/utf8.pc
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig)

# Tests (optional, requires facts library)
option(UTF8_BUILD_TESTS "Build tests (requires ../facts)" OFF)
if(UTF8_BUILD_TESTS)
  enable_testing()
  add_executable(utf8_facts src/utf8_facts.c ../facts/src/facts.c)
  target_link_libraries(utf8_facts PRIVATE utf8_static m)
  target_include_directories(utf8_facts PRIVATE ../facts/include)
  add_test(NAME utf8_facts COMMAND utf8_facts)
endif()

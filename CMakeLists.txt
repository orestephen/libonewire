cmake_minimum_required(VERSION 3.0)

project(libonewire)

add_library(libonewire STATIC)
target_sources(libonewire PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/source/onewire.c
)
target_include_directories(libonewire PUBLIC ${CMAKE_CURRENT_LIST_DIR}/source)
target_compile_options(libonewire PUBLIC -Wall)

if(ONEWIRE_DBG)
  target_compile_definitions(libonewire PUBLIC ONEWIRE_DBG)
endif()

if(ONEWIRE_TEST)
  enable_testing()
  add_subdirectory(tests)
endif()

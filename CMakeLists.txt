cmake_minimum_required(VERSION 3.1...3.27)

project(
  NoCopyRingFifo
  VERSION 1.0
  LANGUAGES CXX)

# CMAKE_ variables
set(CMAKE_CXX_STANDARD 23)

# Find packages

add_library(FifoTestLib)
target_sources(FifoTestLib PRIVATE "${PROJECT_SOURCE_DIR}/include/no_copy_ring_fifo/no_copy_ring_fifo.h")
set_target_properties(FifoTestLib PROPERTIES LINKER_LANGUAGE CXX)

add_executable(FifoTest)
target_sources(FifoTest PRIVATE "${PROJECT_SOURCE_DIR}/src/fifo_test.cpp")
target_include_directories(FifoTest PRIVATE "${PROJECT_SOURCE_DIR}/include/no_copy_ring_fifo/")

target_link_libraries(FifoTest PRIVATE FifoTestLib)
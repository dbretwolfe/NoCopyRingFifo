cmake_minimum_required(VERSION 3.1...3.27)

project(
  NoCopyRingFifoTest
  VERSION 1.0
  LANGUAGES CXX)

add_executable(FifoTest)
target_sources(FifoTest PRIVATE "fifo_test.cpp")
target_include_directories(FifoTest PRIVATE "../no_copy_ring_fifo/")

target_link_libraries(FifoTest PRIVATE NoCopyRingFifo)

target_compile_features(NoCopyRingFifo PUBLIC cxx_std_23)
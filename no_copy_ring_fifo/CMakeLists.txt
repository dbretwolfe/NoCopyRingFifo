cmake_minimum_required(VERSION 3.1...3.27)

project(
  NoCopyRingFifoLib
  VERSION 1.0
  LANGUAGES CXX)

# Find packages

add_library(NoCopyRingFifo)
target_sources(NoCopyRingFifo PRIVATE "no_copy_ring_fifo.h")
set_target_properties(NoCopyRingFifo PROPERTIES LINKER_LANGUAGE CXX)

target_compile_features(NoCopyRingFifo PUBLIC cxx_std_23)
cmake_minimum_required(VERSION 3.1...3.27)

project(
  NoCopyRingFifoProj
  VERSION 1.0
  LANGUAGES CXX)

add_library(NoCopyRingFifo INTERFACE)
target_include_directories(NoCopyRingFifo INTERFACE ./)
set_target_properties(NoCopyRingFifo PROPERTIES LINKER_LANGUAGE CXX)
cmake_minimum_required(VERSION 3.15)
project(znc_fish_module LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_library(fish MODULE fish.cpp)

# Ensure output name is fish.so (no lib prefix)
set_target_properties(fish PROPERTIES OUTPUT_NAME "fish" PREFIX "" SUFFIX ".so")

# OpenSSL for crypto usage inside module
find_package(OpenSSL REQUIRED)
target_link_libraries(fish PRIVATE OpenSSL::Crypto)

# Locate ZNC include ROOT, so includes like <znc/IRCNetwork.h> work
find_path(ZNC_INCLUDE_ROOT
  NAMES znc/IRCNetwork.h
  PATHS /usr/include /usr/local/include
)
if(NOT ZNC_INCLUDE_ROOT)
  message(FATAL_ERROR "ZNC headers not found. Please install znc-dev.")
endif()
message(STATUS "Using ZNC_INCLUDE_ROOT=${ZNC_INCLUDE_ROOT}")

target_include_directories(fish PRIVATE ${ZNC_INCLUDE_ROOT})

# Common defines
if(UNIX)
  target_compile_definitions(fish PRIVATE _LARGEFILE_SOURCE _FILE_OFFSET_BITS=64)
endif()

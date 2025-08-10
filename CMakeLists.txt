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
target_link_libraries(fish PRIVATE OpenSSL::Crypto OpenSSL::SSL)

# ZNC headers (provided by znc-dev on Debian/Ubuntu under /usr/include/znc)
# We don't link against a ZNC library; unresolved symbols are resolved at module load time by ZNC.
target_include_directories(fish PRIVATE /usr/include/znc)

# Common defines
if(UNIX)
  target_compile_definitions(fish PRIVATE _LARGEFILE_SOURCE _FILE_OFFSET_BITS=64)
endif()

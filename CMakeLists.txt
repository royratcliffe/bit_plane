cmake_minimum_required(VERSION 3.25)
project(bit_plane)

add_library(bit_plane
    inc/blt.hxx
    src/blt.cxx
    inc/bit_plane.hxx
    src/bit_plane.cxx
)

# Set the include directories for the phase_align library.
target_include_directories(bit_plane
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/inc>
        $<INSTALL_INTERFACE:include>
)

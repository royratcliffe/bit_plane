cmake_minimum_required(VERSION 3.25)
project(bit_plane)

set(CMAKE_CXX_CLANG_TIDY clang-tidy)

add_library(bit_plane
    inc/raster/scan.hxx
    inc/raster/rop.hxx
    inc/raster/blt.hxx
    src/raster/blt.cxx
    inc/raster/phase_align.hxx
    src/raster/phase_align.cxx
    inc/raster/bit_plane.hxx
    src/raster/bit_plane.cxx
)

# Set the include directories for the library.
target_include_directories(bit_plane
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/inc>
        $<INSTALL_INTERFACE:include>
)

# Add a CTest executable for running all tests.
# This will be used to run the tests defined in the test sources.
# The test sources will be compiled into a test executable.
include(CTest)
create_test_sourcelist(test_sources
    test_runner.c
    test/pat.cxx
)

# Add a test executable that links against the library.
# The test executable will be built from the test sources.
# It will include the library and any necessary test files.
add_executable(test_runner
    ${test_sources}
)
target_link_libraries(test_runner PRIVATE bit_plane)

add_test(NAME pat COMMAND test_runner test/pat)

# https://cmake.org/cmake/help/latest/module/FindDoxygen.html
# https://www.mcternan.me.uk/mscgen/
find_package(Doxygen OPTIONAL_COMPONENTS dot mscgen dia)
if(DOXYGEN_FOUND)
    set(OPTIMIZE_OUTPUT_FOR_CXX YES)
    set(DOXYGEN_SOURCE_BROWSER YES)
    doxygen_add_docs(doxy ${PROJECT_SOURCE_DIR}/inc ${PROJECT_SOURCE_DIR}/src)
endif()

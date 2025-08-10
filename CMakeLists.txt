cmake_minimum_required(VERSION 3.25)
project(bit_plane)

add_library(bit_plane
    inc/blt.hxx
    src/blt.cxx
    inc/phase_align.hxx
    src/phase_align.cxx
    inc/bit_plane.hxx
    src/bit_plane.cxx
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

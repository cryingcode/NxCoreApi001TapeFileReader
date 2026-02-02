# NxCoreApi001TapeFileReader/NxCoreApiTapeFileReader/CMakeLists.txt 

cmake_minimum_required(VERSION 3.25)

#[[
CMake reads this file to construct the project build system.

Run the following in a terminal window from the `NxCoreApiTapeFileReader/` directory to build and install:

# 1. Configure + generate build system (creates/updates the 'build' folder)
rm -rf build && mkdir build
cmake -S . -B build -G Ninja -DCMAKE_VERBOSE_MAKEFILE=ON     # Or skip -G if CMake defaults to Ninja

# Alternative if you want to force Debug/Release or other settings:
# cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_VERBOSE_MAKEFILE=ON

# 2. Build the project (parallel build using all cores)
cmake --build build --parallel

# Or more explicit (same effect):
# cmake --build build -j$(nproc)

# 3. Install (usually needs sudo if installing to /usr or /usr/local)
sudo cmake --install build                        # Installs to /usr/local by default (use -DCMAKE_INSTALL_PREFIX=/path during cmake .. to change)

ls -lR /usr/local/lib/extern                      # should show your nested structure now
sudo ldconfig                                     # To update library cache
ldconfig -p | grep "libnx.so"                     # To verify library cache

If you want a different prefix: cmake .. -DCMAKE_INSTALL_PREFIX=/opt/myproject
For local install (no sudo): cmake .. -DCMAKE_INSTALL_PREFIX=$PWD/install then "cmake --install ."

Expected output from the install step:
sudo cmake --install build
[sudo] password for crymoney: 
-- Install configuration: ""
-- Installing: /usr/local/bin/nxcore-tape-reader
-- Set non-toolchain portion of runtime path of "/usr/local/bin/nxcore-tape-reader" to "$ORIGIN/../lib/extern/nxcore"
-- Installing: /usr/local/lib/libtapefilereaderproject.a
-- Up-to-date: /usr/local/lib/extern
-- Installing: /usr/local/lib/extern/nxcore
-- Installing: /usr/local/lib/extern/nxcore/libnx.so
-- Up-to-date: /usr/local/include
-- Installing: /usr/local/lib/cmake/TapeFileReaderProject/TapeFileReaderProjectTargets.cmake
-- Installing: /usr/local/lib/cmake/TapeFileReaderProject/TapeFileReaderProjectTargets-noconfig.cmake
-- Installing: /usr/local/lib/cmake/TapeFileReaderProject/TapeFileReaderProjectConfigVersion.cmake

Executable application is in the build target directory:  build/nxcore-tape-reader
Executable application is installed as:  nxcore-tape-reader
]]


project(TapeFileReaderProject VERSION 1.0.0
        DESCRIPTION "C++ Nanex NxCore Tape File Reader"
        LANGUAGES CXX
)

# ── Function definitions ──────────────────────────────────────────────

# Helper: Automatically discover and add all subdirs under src/ 
# that contain .h / .hpp files as PRIVATE include directories.
function(add_discovered_src_include_paths TARGET)
    if(NOT TARGET ${TARGET})
        return()
    endif()

    file(GLOB_RECURSE ALL_HEADERS CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp"
    )

    set(INCLUDE_DIRS "")
    foreach(header IN LISTS ALL_HEADERS)
        get_filename_component(dir "${header}" DIRECTORY)
        list(APPEND INCLUDE_DIRS "${dir}")
    endforeach()

    if(INCLUDE_DIRS)
        list(REMOVE_DUPLICATES INCLUDE_DIRS)
        # Sort for nicer -I order in verbose output (optional but readable)
        list(SORT INCLUDE_DIRS)
        target_include_directories(${TARGET}
            PRIVATE
                ${INCLUDE_DIRS}
        )
        # Optional: show what got added during configure (comment out later)
        # message(STATUS "Auto-included dirs for ${TARGET}: ${INCLUDE_DIRS}")
    else()
        message(WARNING "No headers found under src/ — no auto-include paths added for ${TARGET}")
    endif()
endfunction()

# ── Import libnx.so ────────────────────────────────────────────────────
if(NOT TARGET NxCore::nx)
    set(NXCORE_LIB "${CMAKE_CURRENT_SOURCE_DIR}/extern/nxcore/libnx.so")

    if(NOT EXISTS "${NXCORE_LIB}")
        message(FATAL_ERROR "libnx.so missing at ${NXCORE_LIB}")
    endif()

    add_library(NxCore::nx SHARED IMPORTED GLOBAL)
    set_target_properties(NxCore::nx PROPERTIES
        IMPORTED_LOCATION "${NXCORE_LIB}"
        INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/src/srclib/nxcore"
    )

    mark_as_advanced(NXCORE_LIB)
endif()

# ── Global settings ────────────────────────────────────────────────────

# Require out-of-source builds
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)
set(CMAKE_DISABLE_SOURCE_CHANGES ON)
if("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
  message(SEND_ERROR "In-source builds are not allowed.")
endif()

set(CMAKE_CXX_STANDARD 17)    # Ubuntu g++ 13.3.0 is fully compliant with C++17
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)    # prefer -std=c++17 over -std=gnu+

# Generate compile_commands.json (helpful for clangd, YouCompleteMe, etc.)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Nice place for custom CMake modules (if you ever need them)
list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

# ── Main library (only created if there are actual source files besides main.cpp) ──

file(GLOB_RECURSE POTENTIAL_LIB_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cxx"
)

# Remove main.cpp (it belongs to the executable)
list(FILTER POTENTIAL_LIB_SOURCES EXCLUDE REGEX ".*[/\\\\]main\\.(cpp|cc|cxx)$")

# Optional: print what we found (uncomment during debugging)
# message(STATUS "Potential library sources: ${POTENTIAL_LIB_SOURCES}")

set(BUILD_LIBRARY OFF)
if(POTENTIAL_LIB_SOURCES)
    set(BUILD_LIBRARY ON)
endif()

if(BUILD_LIBRARY)
    add_library(tapefilereaderproject)
    add_library(TapeFileReaderProject::tapefilereaderproject ALIAS tapefilereaderproject)

    target_sources(tapefilereaderproject PRIVATE ${POTENTIAL_LIB_SOURCES})

    # Set PUBLIC (for the API) and PRIVATE (internal use) header include directories
    target_include_directories(tapefilereaderproject
      PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
      PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    )

    # Optional: compile definitions, options, etc.
    target_compile_definitions(tapefilereaderproject PRIVATE TAPEFILEREADERPROJECT_DEBUG)
    target_compile_options(tapefilereaderproject PRIVATE -Wall -Wextra -Wpedantic)

    message(STATUS "Building library 'tapefilereaderproject' with ${POTENTIAL_LIB_SOURCES}")
else()
    message(STATUS "No library sources found (only main.cpp?) → skipping library creation")
endif()

# ── Main executable (application) ──────────────────────────────────────
add_executable(nxcore-tape-reader)
add_executable(TapeFileReaderProject::app ALIAS nxcore-tape-reader)
target_sources(nxcore-tape-reader PRIVATE src/main.cpp)

if(BUILD_LIBRARY)
    target_link_libraries(nxcore-tape-reader PRIVATE TapeFileReaderProject::tapefilereaderproject)
endif()

# Link NxCore immediately after targets exist
target_link_libraries(nxcore-tape-reader PRIVATE NxCore::nx)
if(BUILD_LIBRARY)
    target_link_libraries(tapefilereaderproject PRIVATE NxCore::nx)
endif()

set_target_properties(nxcore-tape-reader PROPERTIES
    INSTALL_RPATH "$ORIGIN/../lib/extern/nxcore"
    BUILD_RPATH   "${CMAKE_CURRENT_SOURCE_DIR}/extern/nxcore"   # local development
)

# ── Auto-discover include paths ───────────────
add_discovered_src_include_paths(nxcore-tape-reader)
if(BUILD_LIBRARY)
    add_discovered_src_include_paths(tapefilereaderproject)
endif()

# ── Link NxCore library to both targets ────────────────────────────────
# This must come AFTER the targets are created
target_link_libraries(nxcore-tape-reader PRIVATE NxCore::nx)

if(BUILD_LIBRARY)
    target_link_libraries(tapefilereaderproject PRIVATE NxCore::nx)
endif()

# If you have other common dependencies (e.g. threads, extern libs), add them here unconditionally
# target_link_libraries(nxcore-tape-reader PRIVATE ... )

# ── Tests (CppUnit) ────────────────────────────────────────────────────
option(TAPEFILEREADERPROJECT_BUILD_TESTS "Build unit tests" ON)

if(TAPEFILEREADERPROJECT_BUILD_TESTS)
  enable_testing()

  find_package(CppUnit QUIET)

  if(NOT CppUnit_FOUND)
    find_path(CppUnit_INCLUDE_DIR cppunit/Test.h)
    find_library(CppUnit_LIBRARY NAMES cppunit)
    if(CppUnit_INCLUDE_DIR AND CppUnit_LIBRARY)
        set(CppUnit_FOUND TRUE)
    endif()

    if(CppUnit_FOUND)
      set(CPPUNIT_INCLUDE_DIRS ${CPPUNIT_INCLUDE_DIR})
      set(CPPUNIT_LIBRARIES    ${CPPUNIT_LIBRARY})
      # Optional: hide cache variables if found
      #mark_as_advanced(CPPUNIT_INCLUDE_DIR CPPUNIT_LIBRARY)
    endif()
  endif()

  if(NOT CppUnit_FOUND)
    message(WARNING "CppUnit not found → tests will be disabled")
  else()
    set(TEST_DIR "${CMAKE_CURRENT_SOURCE_DIR}/tests")

    file(GLOB_RECURSE potential_tests
        "${TEST_DIR}/*.cpp"
        "${TEST_DIR}/*.cc"
        "${TEST_DIR}/*.cxx"
    )

    list(LENGTH potential_tests count)
    if(count GREATER 0)
        message(STATUS "Building ${count} test file(s) from ${TEST_DIR}")
        add_executable(tapefilereaderproject-tests ${potential_tests})
        add_executable(TapeFileReaderProject::tests ALIAS tapefilereaderproject-tests)

        # If using GLOB, no need for redundant target_sources unless adding extras
        # target_sources(tapefilereaderproject-tests
        #   PRIVATE
        #     tests/test_main.cpp
        #     tests/test_example.cpp
        #     # tests/submodule/...
        # )

        target_link_libraries(tapefilereaderproject-tests
          PRIVATE
            TapeFileReaderProject::tapefilereaderproject
            ${CPPUNIT_LIBRARIES}
        )
        target_include_directories(tapefilereaderproject-tests
          PRIVATE
            ${CPPUNIT_INCLUDE_DIRS}
        )

        # Register with CTest
        add_test(
          NAME
            tapefilereaderproject-unit-tests
          COMMAND tapefilereaderproject-tests
        )
    else()
        message(STATUS "No test sources found → skipping")
    endif()
  endif()
endif()

# ── Install (optional but recommended) ─────────────────────────────────
include(GNUInstallDirs)

install(TARGETS nxcore-tape-reader
  EXPORT TapeFileReaderProjectTargets
  RUNTIME  DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(BUILD_LIBRARY)
    install(TARGETS tapefilereaderproject
      EXPORT TapeFileReaderProjectTargets
      LIBRARY  DESTINATION ${CMAKE_INSTALL_LIBDIR}
      ARCHIVE  DESTINATION ${CMAKE_INSTALL_LIBDIR}
      INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    )
endif()

install(DIRECTORY
    ${CMAKE_CURRENT_SOURCE_DIR}/extern/
    DESTINATION lib/extern
    FILES_MATCHING
        PATTERN "*.so"
        PATTERN "*.so.*"          # versioned symlinks .so.1 .so.1.2.3 etc.
    PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE
                GROUP_READ             GROUP_EXECUTE
                WORLD_READ             WORLD_EXECUTE
    # Optional: exclude anything you don't want
    # PATTERN "*/tests/*" EXCLUDE
    # PATTERN "*/CMakeLists.txt" EXCLUDE
)

install(DIRECTORY include/
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
  FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp"
)

# Very basic CMake package config (allows find_package(TapeFileReaderProject))
include(CMakePackageConfigHelpers)

write_basic_package_version_file(
  "${PROJECT_BINARY_DIR}/TapeFileReaderProjectConfigVersion.cmake"
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY SameMajorVersion
)

install(EXPORT TapeFileReaderProjectTargets
  FILE TapeFileReaderProjectTargets.cmake
  NAMESPACE TapeFileReaderProject::
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/TapeFileReaderProject
)

install(FILES
  "${PROJECT_BINARY_DIR}/TapeFileReaderProjectConfigVersion.cmake"
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/TapeFileReaderProject
)

# ── End of CMakeLists.txt ──────────────────────────────────────────────

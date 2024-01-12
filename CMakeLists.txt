cmake_minimum_required(VERSION 3.5)

project(rustex LANGUAGES CXX VERSION 1.0.0 DESCRIPTION "Rust-style mutex type for C++" HOMEPAGE_URL "https://github.com/dragazo/rustex")

include(GNUInstallDirs)

string(REPLACE "/${CMAKE_LIBRARY_ARCHITECTURE}" "" CMAKE_INSTALL_LIBDIR_ARCHIND "${CMAKE_INSTALL_LIBDIR}")

set(RUSTEX_MAIN_PROJECT OFF)
if (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    set(RUSTEX_MAIN_PROJECT ON)
endif()

option(RUSTEX_BUILD_TESTS "Build tests" ON)

set(CMAKE_CONFIG_DEST "${CMAKE_INSTALL_LIBDIR_ARCHIND}/cmake/${PROJECT_NAME}")

add_library(rustex INTERFACE)
target_include_directories(rustex INTERFACE
	$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>
	$<INSTALL_INTERFACE:include>)

if(RUSTEX_MAIN_PROJECT)
    include(CMakePackageConfigHelpers)
    install(FILES rustex.h DESTINATION include)
    install(TARGETS rustex EXPORT rustex-targets)
    install(EXPORT rustex-targets
        FILE rustex-config.cmake
        NAMESPACE rustex::
        DESTINATION lib/cmake/rustex)

    write_basic_package_version_file("${CMAKE_CURRENT_BINARY_DIR}/rustex-config-version.cmake" COMPATIBILITY SameMajorVersion ARCH_INDEPENDENT)
    install(FILES "${CMAKE_CURRENT_BINARY_DIR}/rustex-config-version.cmake" DESTINATION lib/cmake/rustex)
endif()

if (RUSTEX_BUILD_TESTS)
    add_executable(rustex_test test/test.cpp)
    target_link_libraries(rustex_test rustex)
    set_property(TARGET rustex_test PROPERTY CXX_STANDARD 17)
endif()

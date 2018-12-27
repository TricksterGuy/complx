cmake_minimum_required(VERSION 3.5)
# Ignore warnings about rpath behavior on OS X
cmake_policy(SET CMP0042 NEW)
project(complx-tools)

# Add code coverage stuff.
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

option(ENABLE_PLUGINS "Enable Official Plugins" ON)
option(ENABLE_LC3EDIT "Build lc3edit" OFF)
option(ENABLE_AS2OBJ "Build as2obj" ON)
option(ENABLE_COMPLX "Build complx" ON)
option(ENABLE_DEV "Build dev version" OFF)
option(ENABLE_TESTS "Enable testing" OFF)
option(ENABLE_COVERAGE "Enable testing coverage" OFF)
option(ENABLE_PYLC3 "Enable python bindings to liblc3" OFF)
option(ENABLE_REPLAY "Enable replay setup from pylc3 tests" ON)

if(ENABLE_TESTS)
    enable_testing()
endif(ENABLE_TESTS)

# Require C++11 build
if(NOT CMAKE_CXX_FLAGS MATCHES "-std=(c|gnu)\\+\\+17")
    message(STATUS "This project requires C++17. Adding -std=c++17 to CXXFLAGS.")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17" CACHE STRING "Flags used by the compiler during all build types." FORCE)
endif()

# Additional debug flags
set(CMAKE_CXX_FLAGS_DEBUG "-g -pg")
# Install directories
set(CMAKE_INSTALL_LIBDIR lib CACHE STRING "where to install libraries/plugins")
set(CMAKE_INSTALL_BINDIR bin CACHE STRING "where to install programs")

if(ENABLE_COVERAGE)
    include(CodeCoverage)
    append_coverage_compiler_flags()
endif(ENABLE_COVERAGE)


# Set default warning flags
set(PROJECT_WARNING_FLAGS "-Wall -Wno-deprecated -Wno-deprecated-declarations" CACHE STRING "Compiler warning flags to include")
mark_as_advanced(PROJECT_WARNING_FLAGS)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${PROJECT_WARNING_FLAGS}")

# Default to release build if not specified
if(NOT CMAKE_BUILD_TYPE)
    message(STATUS "Setting build type to 'Release' as none was specified.")
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release"
    "MinSizeRel" "RelWithDebInfo")
endif(NOT CMAKE_BUILD_TYPE)

if(CMAKE_BUILD_TYPE MATCHES "Debug")
    set(CMAKE_EXE_LINKER_FLAGS "-fprofile-arcs -ftest-coverage")
endif(CMAKE_BUILD_TYPE MATCHES "Debug")

add_definitions(-DPREFIX=${CMAKE_INSTALL_PREFIX})
add_definitions(-DSO_SUFFIX="${CMAKE_SHARED_LIBRARY_SUFFIX}")

if(ENABLE_COMPLX)
    set(wxWidgets_USE_LIBS stc xrc xml html adv net core base aui propgrid)
    find_package(wxWidgets REQUIRED)
    set(CMAKE_SKIP_RPATH TRUE)
    include(${wxWidgets_USE_FILE})
endif(ENABLE_COMPLX)

include_directories(${complx-tools_SOURCE_DIR}/liblc3)
include_directories(${complx-tools_SOURCE_DIR}/logging)

set(SRC_LIBLC3
    liblc3/ExpressionEvaluator.cpp
    liblc3/lc3_assemble.cpp
    liblc3/lc3.cpp
    liblc3/lc3_debug.cpp
    liblc3/lc3_execute.cpp
    liblc3/lc3_expressions.cpp
    liblc3/lc3_os.cpp
    liblc3/lc3_params.cpp
    liblc3/lc3_parser.cpp
    liblc3/lc3_plugin.cpp
    liblc3/lc3_runner.cpp
    liblc3/lc3_symbol.cpp
)

set(SRC_LOGGING
    logging/logger.cpp
)

set(SRC_AS2OBJ
    as2obj/main.cpp
)

add_library(
    lc3
    SHARED
    ${SRC_LIBLC3}
)

target_link_libraries(
    lc3
    dl
)

add_library(
    logger
    SHARED
    ${SRC_LOGGING}
)

if(ENABLE_AS2OBJ)
    add_executable(
        as2obj
        ${SRC_AS2OBJ}
    )

    target_link_libraries(
        as2obj
        lc3
    )

    install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/as2obj DESTINATION ${CMAKE_INSTALL_BINDIR})
endif(ENABLE_AS2OBJ)

install(TARGETS lc3 LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} RUNTIME DESTINATION ${CMAKE_INSTALL_LIBDIR})

if(ENABLE_REPLAY)
    add_subdirectory(replay)
endif(ENABLE_REPLAY)

if(ENABLE_PLUGINS)
    add_subdirectory(plugins)
endif(ENABLE_PLUGINS)

if(ENABLE_LC3EDIT)
    add_subdirectory(lc3edit)
endif(ENABLE_LC3EDIT)

if(ENABLE_COMPLX)
    add_subdirectory(complx)
endif(ENABLE_COMPLX)

if(ENABLE_TESTS)
    add_subdirectory(tests)
endif(ENABLE_TESTS)

if(ENABLE_PYLC3)
    add_subdirectory(pylc3)
endif(ENABLE_PYLC3)

# uninstall target
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)

add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)

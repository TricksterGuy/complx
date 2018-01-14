cmake_minimum_required(VERSION 2.6)
if(CMAKE_VERSION VERSION_EQUAL 3.0 OR CMAKE_VERSION VERSION_GREATER 3.0)
    # Ignore warnings about rpath behavior on OS X
    cmake_policy(SET CMP0042 NEW)
endif()
project(complx-tools)

# Add code coverage stuff.
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

option(ENABLE_PLUGINS "Enable Official Plugins" ON)
option(ENABLE_LC3EDIT "Build lc3edit" OFF)
option(ENABLE_COMP "Build comp" OFF)
option(ENABLE_COMPLX "Build complx" ON)
option(ENABLE_LC3TEST "Build lc3test" ON)
option(ENABLE_DEV "Build dev version" OFF)
option(ENABLE_TESTS "Enable testing" OFF)
option(ENABLE_COVERAGE "Enable testing coverage" OFF)

if(ENABLE_TESTS)
    enable_testing()
endif(ENABLE_TESTS)

# Require C++11 build
if(NOT CMAKE_CXX_FLAGS MATCHES "-std=(c|gnu)\\+\\+11")
    message(STATUS "This project requires C++11. Adding -std=c++11 to CXXFLAGS.")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11" CACHE STRING "Flags used by the compiler during all build types." FORCE)
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

if(ENABLE_COMPLX OR ENABLE_LC3TEST OR ENABLE_TESTS)
set(wxWidgets_USE_LIBS stc xrc xml html adv net core base)
find_package(wxWidgets REQUIRED)
set(CMAKE_SKIP_RPATH TRUE)
include(${wxWidgets_USE_FILE})
endif(ENABLE_COMPLX OR ENABLE_LC3TEST OR ENABLE_TESTS)

if(CMAKE_HOST_WIN32)
    # Needed for htons
    set(LIBLC3_EXTRA_LIBS ws2_32)
    # Windows doesn't set include directories correctly?
    #include_directories("C:/msys/1.0/local/lib/wx/include/msw-unicode-3.0")
    #include_directories("C:/msys/1.0/local/include/wx-3.0")
endif(CMAKE_HOST_WIN32)

include_directories(${complx-tools_SOURCE_DIR}/lc3test)
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
    liblc3/lc3_test.cpp
    liblc3/lc3_cmp.cpp
)

set(SRC_LOGGING
    logging/logger.cpp
)

set(SRC_AS2OBJ
    as2obj/main.cpp
)

set(SRC_LC3RUNNER
    lc3runner/main.cpp
)

add_library(
    lc3
    SHARED
    ${SRC_LIBLC3}
)

target_link_libraries(
    lc3
    dl
    ${LIBLC3_EXTRA_LIBS}
)

add_library(
    logger
    ${SRC_LOGGING}
)

add_executable(
    as2obj
    ${SRC_AS2OBJ}
)

target_link_libraries(
    as2obj
    lc3
)

add_executable(
    lc3runner
    ${SRC_LC3RUNNER}
)

target_link_libraries(
    lc3runner
    lc3
)

install(TARGETS lc3 LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} RUNTIME DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/as2obj DESTINATION ${CMAKE_INSTALL_BINDIR})
install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/lc3runner DESTINATION ${CMAKE_INSTALL_BINDIR})

if(ENABLE_PLUGINS)
    add_subdirectory(plugins)
endif(ENABLE_PLUGINS)

if(ENABLE_LC3EDIT)
    add_subdirectory(lc3edit)
endif(ENABLE_LC3EDIT)

if(ENABLE_COMP)
    add_subdirectory(comp)
endif(ENABLE_COMP)

if(ENABLE_COMPLX)
    add_subdirectory(complx)
endif(ENABLE_COMPLX)

if(ENABLE_LC3TEST)
	add_subdirectory(lc3test)
endif(ENABLE_LC3TEST)

if(ENABLE_TESTS)
    add_subdirectory(tests)
endif(ENABLE_TESTS)

# uninstall target
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)

add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)

cmake_minimum_required(VERSION 2.6)
if(CMAKE_VERSION VERSION_EQUAL 3.0 OR CMAKE_VERSION VERSION_GREATER 3.0)
    # Ignore warnings about rpath behavior on OS X
    cmake_policy(SET CMP0042 NEW)
endif()
project(complx-tools)
enable_testing()

option(ENABLE_PLUGINS "Enable Official Plugins" ON)
option(ENABLE_LC3EDIT "Build lc3edit" OFF)
option(ENABLE_COMP "Build comp" OFF)
option(ENABLE_DEV "Build dev version" OFF)

if(ENABLE_PLUGINS)
    add_subdirectory(plugins)
endif(ENABLE_PLUGINS)

if(ENABLE_LC3EDIT)
    add_subdirectory(lc3edit)
endif(ENABLE_LC3EDIT)

if(ENABLE_COMP)
    add_subdirectory(comp)
endif(ENABLE_COMP)

add_subdirectory(tests)

# Require C++11 build
if(NOT CMAKE_CXX_FLAGS MATCHES "-std=(c|gnu)\\+\\+11")
    message(STATUS "This project requires C++11. Adding -std=c++11 to CXXFLAGS.")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11" CACHE STRING "Flags used by the compiler during all build types." FORCE)
endif()

# Additional debug flags
set(CMAKE_CXX_FLAGS_DEBUG "-g -pg")

# Set default warning flags
set(PROJECT_WARNING_FLAGS "-Wall" CACHE STRING "Compiler warning flags to include")
mark_as_advanced(PROJECT_WARNING_FLAGS)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${PROJECT_WARNING_FLAGS}")

# Default to release build if not specified
if(NOT CMAKE_BUILD_TYPE)
    message(STATUS "Setting build type to 'Release' as none was specified.")
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release"
    "MinSizeRel" "RelWithDebInfo")
endif(NOT CMAKE_BUILD_TYPE)

if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    add_definitions(-DLINUX)
endif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    add_definitions(-DLINUX)
endif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")

add_definitions(-DPREFIX=${CMAKE_INSTALL_PREFIX})
add_definitions(-DSO_SUFFIX="${CMAKE_SHARED_LIBRARY_SUFFIX}")
set(wxWidgets_USE_LIBS stc xrc xml html adv gl net core base)
find_package(wxWidgets REQUIRED)
set(CMAKE_SKIP_RPATH TRUE)
include(${wxWidgets_USE_FILE})

include_directories(${complx-tools_SOURCE_DIR}/liblc3)
include_directories(${complx-tools_SOURCE_DIR}/lc3test)

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

set(SRC_AS2OBJ
    as2obj/main.cpp
)

set(SRC_LC3TEST
    lc3test/main.cpp
    lc3test/XmlTestParser.cpp
)

set(SRC_LC3RUNNER
    lc3runner/main.cpp
)

set(SRC_COMPLX
    complx/AddressDialog.cpp
    complx/AddressDialogDecl.cpp
    complx/BreakpointDialog.cpp
    complx/BreakpointDialogDecl.cpp
    complx/CallStackDialog.cpp
    complx/CallStackDialogDecl.cpp
    complx/CallSubroutineDialog.cpp
    complx/CallSubroutineDialogDecl.cpp
    complx/ComplxApp.cpp
    complx/ComplxFrame.cpp
    complx/ComplxFrameDecl.cpp
    complx/DebugInfoDialog.cpp
    complx/DebugInfoDialogDecl.cpp
    complx/DebugInfoList.cpp
    complx/GridCellBinaryRenderer.cpp
    complx/GridCellInfoRenderer.cpp
    complx/LC3Console.cpp
    complx/LC3ConsoleDecl.cpp
    complx/LC3RunThread.cpp
    complx/MemoryGrid.cpp
    complx/MemoryView.cpp
    complx/MemoryViewContainer.cpp
    complx/MemoryViewFrame.cpp
    complx/MemoryViewFrameDecl.cpp
    complx/RunForDialog.cpp
    complx/RunForDialogDecl.cpp
    complx/RunTestDialog.cpp
    complx/RunTestDialogDecl.cpp
    complx/WatchpointDialog.cpp
    complx/WatchpointDialogDecl.cpp
    complx/icon32.xpm

    lc3test/XmlTestParser.cpp
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

add_executable(
    complx
    ${SRC_COMPLX}
)

target_link_libraries(
    complx
    lc3
    ${wxWidgets_LIBRARIES}
)

add_executable(
    lc3test
    ${SRC_LC3TEST}
)

target_link_libraries(
    lc3test
    lc3
    ${wxWidgets_LIBRARIES}
)

install(TARGETS lc3 LIBRARY DESTINATION lib)
install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/complx DESTINATION bin)
install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/as2obj DESTINATION bin)
install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/lc3runner DESTINATION bin)
install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/lc3test DESTINATION bin)


install(FILES doc/complx-tools.pdf DESTINATION share/doc/complx-tools)
install(FILES doc/ComplxChangeLog.txt DESTINATION share/doc/complx-tools)
install(FILES doc/PattPatelAppA.pdf DESTINATION share/doc/complx-tools)
install(FILES doc/complx-tips.txt DESTINATION share/doc/complx-tools)
install(FILES complx.png DESTINATION share/icons)
install(FILES complx.desktop DESTINATION share/applications)

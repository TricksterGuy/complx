include_directories(${complx-tools_SOURCE_DIR}/comp)

include(FindPkgConfig)

find_package(BISON)
find_package(FLEX)
pkg_search_module(NCURSES REQUIRED ncurses)

BISON_TARGET(
    CompParser
    comp.y
    ${CMAKE_CURRENT_BINARY_DIR}/comp.tab.cpp
    COMPILE_FLAGS -t
)

FLEX_TARGET(
    CompScanner
    comp.l
    ${CMAKE_CURRENT_BINARY_DIR}/comp.cpp
    COMPILE_FLAGS --header-file=${CMAKE_CURRENT_BINARY_DIR}/comp.hpp
)
ADD_FLEX_BISON_DEPENDENCY(CompScanner CompParser)

include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${NCURSES_INCLUDEDIR})

set(SRC_COMP
    main.cpp
    commands.cpp
    curses.cpp
    windows.cpp
)

add_executable(
    comp
    ${SRC_COMP}
    ${BISON_CompParser_OUTPUTS}
    ${FLEX_CompScanner_OUTPUTS}
)

target_link_libraries(
    comp
    lc3
    ${FLEX_LIBRARIES}
    ${NCURSES_LIBRARIES}
)

install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/comp DESTINATION ${CMAKE_INSTALL_BINDIR})


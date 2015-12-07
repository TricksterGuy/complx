include_directories(${complx-tools_SOURCE_DIR}/liblc3)
set(CMAKE_SKIP_RPATH TRUE)

set(SRC_COMP
    main.cpp
    comp.hpp
    comp.cpp
)

add_executable(
    comp
    ${SRC_COMP}
)

target_link_libraries(
    comp
    lc3
)

install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/comp DESTINATION ${CMAKE_INSTALL_BINDIR})


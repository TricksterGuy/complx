include_directories(${complx-tools_SOURCE_DIR}/liblc3)
include_directories(${complx-tools_SOURCE_DIR}/logging)

set(SRC_LC3TEST
    main.cpp
    XmlTestParser.cpp
    tinyxml2.cpp
    TextResultEmitter.cpp
    XmlResultEmitter.cpp
)

add_executable(
    lc3test
    ${SRC_LC3TEST}
)

target_link_libraries(
    lc3test
    lc3
)

install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/lc3test DESTINATION ${CMAKE_INSTALL_BINDIR})

set(SRC_AS2OBJ
    main.cpp
)

add_executable(
    as2obj
    ${SRC_AS2OBJ}
)

target_link_libraries(
    as2obj
    lc3
)

install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/as2obj DESTINATION ${CMAKE_INSTALL_BINDIR})

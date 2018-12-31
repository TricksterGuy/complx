set(LIBLC3_EXTRA_LIBS "")

if(CMAKE_HOST_WIN32)
    # Needed for htons
    set(LIBLC3_EXTRA_LIBS ws2_32)
endif(CMAKE_HOST_WIN32)

set(SRC_LIBLC3
    ExpressionEvaluator.cpp
    lc3_assemble.cpp
    lc3.cpp
    lc3_debug.cpp
    lc3_execute.cpp
    lc3_expressions.cpp
    lc3_os.cpp
    lc3_params.cpp
    lc3_parser.cpp
    lc3_plugin.cpp
    lc3_runner.cpp
    lc3_symbol.cpp
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

install(TARGETS lc3 LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} RUNTIME DESTINATION ${CMAKE_INSTALL_LIBDIR})

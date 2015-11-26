option(ENABLE_UDIV "Enable the UDIV Trap Plugin" ON)
include_directories(${complx-tools_SOURCE_DIR}/liblc3)
set(CMAKE_SKIP_RPATH TRUE)

if(ENABLE_UDIV)

set(SRC_UDIV
    udiv/udiv.cpp
)

add_library(
    lc3_udiv
    SHARED
    ${SRC_UDIV}
)

target_link_libraries(
    lc3_udiv
    lc3
)

install(TARGETS lc3_udiv LIBRARY DESTINATION lib)

endif(ENABLE_UDIV)


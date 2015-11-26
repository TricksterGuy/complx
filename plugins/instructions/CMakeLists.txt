option(ENABLE_MULTIPLY "Enable the Multiply Instruction Plugin" ON)
include_directories(${complx-tools_SOURCE_DIR}/liblc3)

if(ENABLE_MULTIPLY)

set(SRC_MULTIPLY
    multiply/multiply.cpp
)

add_library(
    lc3_multiply
    SHARED
    ${SRC_MULTIPLY}
)

target_link_libraries(
    lc3_multiply
    lc3
)

install(TARGETS lc3_multiply LIBRARY DESTINATION lib)

endif(ENABLE_MULTIPLY)


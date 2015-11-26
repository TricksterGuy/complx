option(ENABLE_BWLCD "Enable the Black White LCD Plugin" ON)
option(ENABLE_COLORLCD "Enable the Color LCD Plugin" ON)
include_directories(${complx-tools_SOURCE_DIR}/liblc3)

set(wxWidgets_USE_LIBS stc xrc xml html adv gl net core base)
find_package(wxWidgets REQUIRED)
execute_process(COMMAND sh "${wxWidgets_CONFIG_EXECUTABLE}" --cxxflags)
include(${wxWidgets_USE_FILE})
set(CMAKE_SKIP_RPATH TRUE)

if(ENABLE_BWLCD)

set(SRC_BWLCD
    bwlcd/bwlcd.cpp
    bwlcd/bwlcdgui.cpp
)

add_library(
    lc3_bwlcd
    SHARED
    ${SRC_BWLCD}
)

target_link_libraries(
    lc3_bwlcd
    lc3
    ${wxWidgets_LIBRARIES}
)

install(TARGETS lc3_bwlcd LIBRARY DESTINATION lib)

endif(ENABLE_BWLCD)

if(ENABLE_COLORLCD)

set(SRC_COLORLCD
    colorlcd/colorlcd.cpp
    colorlcd/colorlcdgui.cpp
)

add_library(
    lc3_colorlcd
    SHARED
    ${SRC_COLORLCD}
)

target_link_libraries(
    lc3_colorlcd
    lc3
    ${wxWidgets_LIBRARIES}
)

install(TARGETS lc3_colorlcd LIBRARY DESTINATION lib)

endif (ENABLE_COLORLCD)


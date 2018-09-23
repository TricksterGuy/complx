option(ENABLE_BWLCD "Enable the Black White LCD Plugin" ON)
option(ENABLE_COLORLCD "Enable the Color LCD Plugin" ON)
option(ENABLE_PINGER "Enable the Pinger Plugin" ON)
include_directories(${complx-tools_SOURCE_DIR}/liblc3)

if(ENABLE_BWLCD AND ENABLE_COMPLX)

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

install(TARGETS lc3_bwlcd LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} RUNTIME DESTINATION ${CMAKE_INSTALL_LIBDIR})

endif(ENABLE_BWLCD AND ENABLE_COMPLX)

if(ENABLE_COLORLCD AND ENABLE_COMPLX)

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

install(TARGETS lc3_colorlcd LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} RUNTIME DESTINATION ${CMAKE_INSTALL_LIBDIR})

endif (ENABLE_COLORLCD AND ENABLE_COMPLX)

if(ENABLE_PINGER)

set(SRC_PINGER
    pinger/pinger.cpp
)

add_library(
    lc3_pinger
    SHARED
    ${SRC_PINGER}
)

target_link_libraries(
    lc3_pinger
    lc3
    ${wxWidgets_LIBRARIES}
)

install(TARGETS lc3_pinger LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} RUNTIME DESTINATION ${CMAKE_INSTALL_LIBDIR})

endif (ENABLE_PINGER)

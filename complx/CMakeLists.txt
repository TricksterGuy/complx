include_directories(${complx-tools_SOURCE_DIR}/lc3test)
include_directories(${complx-tools_SOURCE_DIR}/liblc3)
include_directories(${complx-tools_SOURCE_DIR}/logging)

if(CMAKE_HOST_UNIX)
    include(FindPkgConfig)
    pkg_check_modules(GLIB2 REQUIRED glib-2.0)
    include_directories(${GLIB2_INCLUDE_DIRS})
endif(CMAKE_HOST_UNIX)

set(SRC_COMPLX
    data/Lc3BinaryDisplayData.cpp
    data/MemoryViewBinaryDataRenderer.cpp
    data/MemoryViewInfoDataRenderer.cpp
    data/MemoryViewDataModel.cpp
    data/ProcessStatusRegisterProperty.cpp
    data/RegisterProperty.cpp
    gen/ComplxFrameDecl.cpp
    util/GuiConstants.cpp
    util/ValidationHelper.cpp
    ComplxApp.cpp
    ComplxFrame.cpp
    MemoryView.cpp
)

add_executable(
    complx
    ${SRC_COMPLX}
)

target_link_libraries(
    complx
    lc3
    logger
    ${wxWidgets_LIBRARIES}
    ${GLIB2_LIBRARIES}
)

install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/complx DESTINATION ${CMAKE_INSTALL_BINDIR})

install(FILES ../doc/complx-tools.pdf DESTINATION share/doc/complx-tools)
install(FILES ../doc/ComplxChangeLog.txt DESTINATION share/doc/complx-tools)
install(FILES ../doc/PattPatelAppA.pdf DESTINATION share/doc/complx-tools)
install(FILES ../doc/complx-tips.txt DESTINATION share/doc/complx-tools)
install(FILES ../complx.png DESTINATION share/icons)
install(FILES ../complx.desktop DESTINATION share/applications)

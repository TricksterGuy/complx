include_directories(${complx-tools_SOURCE_DIR}/lc3test)
include_directories(${complx-tools_SOURCE_DIR}/liblc3)
include_directories(${complx-tools_SOURCE_DIR}/logging)

if(CMAKE_HOST_UNIX)
    include(FindPkgConfig)
    pkg_check_modules(GLIB2 REQUIRED glib-2.0)
    include_directories(${GLIB2_INCLUDE_DIRS})
endif(CMAKE_HOST_UNIX)

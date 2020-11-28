#
# External dependencies
#

if(UNIX)
    include(FindPkgConfig)
    pkg_check_modules(GLIB2 REQUIRED glib-2.0)
endif(UNIX)

find_package(wxWidgets REQUIRED aui propgrid stc adv core base)
include(${wxWidgets_USE_FILE})

#
# Executable name and options
#

# Target name
set(target complx)

# Exit here if required dependencies are not met
if (NOT wxWidgets_FOUND)
    message(STATUS "Program ${target} skipped: wxWidgets not found")
    return()
else()
    message(STATUS "Program ${target}")
endif()

#
# Sources
#

set(sources
    ComplxApp.cpp
    ComplxFrame.cpp
    MemoryView.cpp
    data/Lc3BinaryDisplayData.cpp
    data/MemoryViewBinaryDataRenderer.cpp
    data/MemoryViewInfoDataRenderer.cpp
    data/MemoryViewDataModel.cpp
    data/ProcessStatusRegisterProperty.cpp
    data/RegisterProperty.cpp
    gen/ComplxFrameDecl.cpp
    util/GuiConstants.cpp
    util/ValidationHelper.cpp
)


#
# Create executable
#

# Build executable
add_executable(${target}
    MACOSX_BUNDLE
    ${sources}
)

# Create namespaced alias
add_executable(${META_PROJECT_NAME}::${target} ALIAS ${target})


#
# Project options
#

set_target_properties(${target}
    PROPERTIES
    ${DEFAULT_PROJECT_OPTIONS}
    FOLDER "${IDE_FOLDER}"
)


#
# Include directories
#

target_include_directories(${target}
    PRIVATE
    ${DEFAULT_INCLUDE_DIRECTORIES}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${wxWidgets_INCLUDE_DIRS}
    ${GLIB2_INCLUDE_DIRS}
)


#
# Libraries
#

target_link_libraries(${target}
    PRIVATE
    ${DEFAULT_LIBRARIES}
    ${wxWidgets_LIBRARIES}
    ${GLIB2_LIBRARIES}
    ${META_PROJECT_NAME}::lc3
    ${META_PROJECT_NAME}::logging
)


#
# Compile definitions
#

target_compile_definitions(${target}
    PRIVATE
    ${DEFAULT_COMPILE_DEFINITIONS}
)


#
# Compile options
#

target_compile_options(${target}
    PRIVATE
    ${DEFAULT_COMPILE_OPTIONS}
)


#
# Linker options
#

target_link_libraries(${target}
    PRIVATE
    ${DEFAULT_LINKER_OPTIONS}
)


#
# Target Health
#

perform_health_checks(
    ${target}
    ${sources}
)

generate_coverage_report(${target})


#
# Deployment
#

# Executable
install(TARGETS ${target}
    RUNTIME DESTINATION ${INSTALL_BIN} COMPONENT examples
    BUNDLE  DESTINATION ${INSTALL_BIN} COMPONENT examples
)

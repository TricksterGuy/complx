
#
# Configuration for all sub-projects
#

# Generate version-header
configure_file(version.h.in ${CMAKE_CURRENT_BINARY_DIR}/include/${META_PROJECT_NAME}/${META_PROJECT_NAME}-version.h @ONLY)


#
# Sub-projects
#

option(OPTION_BUILD_PLUGINS "Build LC-3 Plugins" ON)

# Libraries
set(IDE_FOLDER "")
add_subdirectory(lc3)
add_subdirectory(logging)
add_subdirectory(lc3_replay)

# Plugins
if(OPTION_BUILD_PLUGINS)
    set(IDE_FOLDER "Plugins")
    add_subdirectory(plugins)
endif(OPTION_BUILD_PLUGINS)

# Programs
set(IDE_FOLDER "Programs")
add_subdirectory(lc3as)
#add_subdirectory(lc3edit)
add_subdirectory(complx)

# Bindings
#add_subdirectory(pylc3)

# Tests
if(OPTION_BUILD_TESTS)
    set(IDE_FOLDER "Tests")
    add_subdirectory(tests)
endif()


#
# Deployment
#

# Deploy generated headers
install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/include/${META_PROJECT_NAME} DESTINATION include COMPONENT dev)

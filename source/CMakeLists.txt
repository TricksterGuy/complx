
#
# Configuration for all sub-projects
#

# Generate version-header
configure_file(version.h.in ${CMAKE_CURRENT_BINARY_DIR}/include/${META_PROJECT_NAME}/${META_PROJECT_NAME}-version.h @ONLY)


#
# Sub-projects
#

option(OPTION_BUILD_PLUGINS "Build LC-3 Plugins" ON)
option(OPTION_BUILD_COMPLX "Build complx" ON)
option(OPTION_BUILD_PYLC3 "Build pyLC3" ON)
option(OPTION_BUILD_LC3AS "Build lc3as" ON)

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
if(OPTION_BUILD_LC3AS)
    add_subdirectory(lc3as)
endif(OPTION_BUILD_LC3AS)
if(OPTION_BUILD_LC3EDIT)
    add_subdirectory(lc3edit)
endif(OPTION_BUILD_LC3EDIT)
if(OPTION_BUILD_COMPLX)
    add_subdirectory(complx)
endif(OPTION_BUILD_COMPLX)

# Bindings
if(OPTION_BUILD_PYLC3)
    add_subdirectory(pylc3)
endif(OPTION_BUILD_PYLC3)

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

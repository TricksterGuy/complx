
#
# External dependencies
#

find_package(${META_PROJECT_NAME} REQUIRED HINTS "${CMAKE_CURRENT_SOURCE_DIR}/../../../")

#
# Executable name and options
#

# Target name
set(target lc3_plugins_test)
message(STATUS "Test ${target}")


#
# Sources
#

set(sources
    lc3_plugins_test.cpp
)


#
# Create executable
#

# Build executable
add_executable(${target}
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
    ${Boost_INCLUDE_DIRS}
    ${PROJECT_BINARY_DIR}/source/include
)


#
# Libraries
#

target_link_libraries(${target}
    PRIVATE
    ${DEFAULT_LIBRARIES}
    ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
    ${META_PROJECT_NAME}::lc3

)


#
# Compile definitions
#

target_compile_definitions(${target}
    PRIVATE
    ${DEFAULT_COMPILE_DEFINITIONS}
    PLUGIN_INSTALL_DIR="${CMAKE_BINARY_DIR}"
    BOOST_TEST_DYN_LINK=1
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

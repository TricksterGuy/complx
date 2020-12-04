#
# Options
#

option(OPTION_ENABLE_RANDOM_PLUGIN "Enable the Random Value Plugin" ON)


#
# Library name and options
#

# Target name
set(target lc3_random)

# Set API export file and macro
string(MAKE_C_IDENTIFIER ${target} target_id)
string(TOUPPER ${target_id} target_id)
set(feature_file         "include/${target}/${target}_features.h")
set(export_file          "include/${target}/${target}_export.h")
set(template_export_file "include/${target}/${target}_api.h")
set(export_macro         "${target_id}_API")

# Exit here if required dependencies are not met
if (NOT OPTION_ENABLE_RANDOM_PLUGIN)
    message(STATUS "Plugin ${target} disabled")
    return()
else()
    message(STATUS "Plugin ${target}")
endif()


#
# Sources
#

set(include_path "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(source_path  "${CMAKE_CURRENT_SOURCE_DIR}/source")

set(headers
    random.hpp
)

set(sources
    random.cpp
)

# Group source files
set(header_group "Header Files (API)")
set(source_group "Source Files")
source_group_by_path(${include_path} "\\\\.h$|\\\\.hpp$"
    ${header_group} ${headers})
source_group_by_path(${source_path}  "\\\\.cpp$|\\\\.c$|\\\\.h$|\\\\.hpp$"
    ${source_group} ${sources})


#
# Create library
#

# Build library
add_library(${target}
    SHARED
    ${sources}
    ${headers}
)

# Create namespaced alias
add_library(${META_PROJECT_NAME}::${target} ALIAS ${target})


# Create API export header
generate_export_header(${target}
    EXPORT_FILE_NAME  ${export_file}
    EXPORT_MACRO_NAME ${export_macro}
)
generate_template_export_header(${target}
    ${target_id}
    ${template_export_file}
)

#
# Project options
#

set_target_properties(${target}
    PROPERTIES
    ${DEFAULT_PROJECT_OPTIONS}
    FOLDER "${IDE_FOLDER}"
    VERSION ${META_VERSION}
    SOVERSION ${META_VERSION_MAJOR}
)


#
# Include directories
#

target_include_directories(${target}
    PRIVATE
    ${PROJECT_BINARY_DIR}/source/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_BINARY_DIR}/include

    PUBLIC
    ${DEFAULT_INCLUDE_DIRECTORIES}

    INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
    $<INSTALL_INTERFACE:include>
)


#
# Libraries
#

target_link_libraries(${target}
    PRIVATE

    PUBLIC
    ${DEFAULT_LIBRARIES}
    ${META_PROJECT_NAME}::lc3

    INTERFACE
)


#
# Compile definitions
#
target_compile_definitions(${target}
    PRIVATE

    PUBLIC
    ${DEFAULT_COMPILE_DEFINITIONS}

    INTERFACE
)


#
# Compile options
#

target_compile_options(${target}
    PRIVATE

    PUBLIC
    ${DEFAULT_COMPILE_OPTIONS}

    INTERFACE
)


#
# Linker options
#

target_link_libraries(${target}
    PRIVATE

    PUBLIC
    ${DEFAULT_LINKER_OPTIONS}

    INTERFACE
)


#
# Target Health
#

perform_health_checks(
    ${target}
    ${sources}
    ${headers}
)

generate_coverage_report(${target})


#
# Deployment
#

# Library
install(TARGETS ${target}
    RUNTIME DESTINATION ${INSTALL_LIB}    COMPONENT runtime
)
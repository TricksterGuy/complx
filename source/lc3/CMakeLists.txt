#
# Library name and options
#

# Target name
set(target lc3)
set(target_objlib ${target}_objlib)

# Exit here if required dependencies are not met
message(STATUS "Lib ${target}")

# Set API export file and macro
string(MAKE_C_IDENTIFIER ${target} target_id)
string(TOUPPER ${target_id} target_id)
set(export_file          "include/${target}/${target}_export.h")
set(template_export_file "include/${target}/${target}_api.h")
set(export_macro         "${target_id}_API")


#
# Sources
#

set(include_path "${CMAKE_CURRENT_SOURCE_DIR}/include/${target}")
set(source_path  "${CMAKE_CURRENT_SOURCE_DIR}/source")

set(headers
    ${include_path}/ExpressionEvaluator.hpp
    ${include_path}/lc3_assemble.hpp
    ${include_path}/lc3.hpp
    ${include_path}/lc3_debug.hpp
    ${include_path}/lc3_execute.hpp
    ${include_path}/lc3_expressions.hpp
    ${include_path}/lc3_os.hpp
    ${include_path}/lc3_params.hpp
    ${include_path}/lc3_parser.hpp
    ${include_path}/lc3_plugin.hpp
    ${include_path}/lc3_runner.hpp
    ${include_path}/lc3_symbol.hpp
    ${include_path}/../lc3.hpp

)

set(sources
    ${source_path}/ExpressionEvaluator.cpp
    ${source_path}/lc3_assemble.cpp
    ${source_path}/lc3.cpp
    ${source_path}/lc3_debug.cpp
    ${source_path}/lc3_execute.cpp
    ${source_path}/lc3_expressions.cpp
    ${source_path}/lc3_os.cpp
    ${source_path}/lc3_params.cpp
    ${source_path}/lc3_parser.cpp
    ${source_path}/lc3_plugin.cpp
    ${source_path}/lc3_runner.cpp
    ${source_path}/lc3_symbol.cpp
)

# Group source files
set(header_group "Header Files (API)")
set(source_group "Source Files")
source_group_by_path(${include_path} "\\\\.h$|\\\\.hpp$"
    ${header_group} ${headers})
source_group_by_path(${source_path}  "\\\\.cpp$|\\\\.c$|\\\\.h$|\\\\.hpp$"
    ${source_group} ${sources})


#
# Create object library
#

add_library(${target_objlib}
    OBJECT
    ${sources}
    ${headers}
)


#
# Project options
#

set_target_properties(${target_objlib}
    PROPERTIES
    ${DEFAULT_PROJECT_OPTIONS}
    POSITION_INDEPENDENT_CODE 1
    FOLDER "${IDE_FOLDER}"
)


#
# Include directories
#

target_include_directories(${target_objlib}
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
# Compile definitions
#

target_compile_definitions(${target_objlib}
    PRIVATE

    PUBLIC
    SHARED_LIBRARY_SUFFIX="${CMAKE_SHARED_LIBRARY_SUFFIX}"
    ${DEFAULT_COMPILE_DEFINITIONS}

    INTERFACE
)


#
# Compile options
#

target_compile_options(${target_objlib}
    PRIVATE

    PUBLIC
    ${DEFAULT_COMPILE_OPTIONS}

    INTERFACE
)


#
# Create shared library
#

# Build library
add_library(${target}
    SHARED
    $<TARGET_OBJECTS:${target}_objlib>
)

# Create namespaced alias
add_library(${META_PROJECT_NAME}::${target} ALIAS ${target})

# Export library for downstream projects
export(TARGETS ${target} NAMESPACE ${META_PROJECT_NAME}:: FILE ${PROJECT_BINARY_DIR}/cmake/${target}/${target}-export.cmake)

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
    dl

    INTERFACE
)


if(WIN32)
    # Needed for htons
    target_link_libraries(${target} PUBLIC ws2_32)
endif(WIN32)


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
    EXPORT  "${target}-export"            COMPONENT dev
    RUNTIME DESTINATION ${INSTALL_BIN}    COMPONENT runtime
    LIBRARY DESTINATION ${INSTALL_SHARED} COMPONENT runtime
    ARCHIVE DESTINATION ${INSTALL_LIB}    COMPONENT dev
)

# Header files
install(DIRECTORY
    ${CMAKE_CURRENT_SOURCE_DIR}/include/${target} DESTINATION ${INSTALL_INCLUDE}
    COMPONENT dev
)

# Generated header files
install(DIRECTORY
    ${CMAKE_CURRENT_BINARY_DIR}/include/${target} DESTINATION ${INSTALL_INCLUDE}
    COMPONENT dev
)

# CMake config
install(EXPORT ${target}-export
    NAMESPACE   ${META_PROJECT_NAME}::
    DESTINATION ${INSTALL_CMAKE}/${target}
    COMPONENT   dev
)


#
# Begin Test Only Static Library
#

set(target_testlib ${target}_testlib)

if(NOT OPTION_BUILD_TESTS)
    message(STATUS "TestLib ${target_testlib} skipped: Not building tests")
    return()
else()
    message(STATUS "Testlib ${target_testlib}")
endif()

# Build library
add_library(${target_testlib}
    STATIC
    $<TARGET_OBJECTS:${target}_objlib>
)

# Create namespaced alias
add_library(${META_PROJECT_NAME}::${target_testlib} ALIAS ${target_testlib})

# Export library for downstream projects
export(TARGETS ${target_testlib} NAMESPACE ${META_PROJECT_NAME}:: FILE ${PROJECT_BINARY_DIR}/cmake/${target}/${target_testlib}-export.cmake)

#
# Project options
#

set_target_properties(${target_testlib}
    PROPERTIES
    ${DEFAULT_PROJECT_OPTIONS}
    FOLDER "${IDE_FOLDER}"
)


#
# Include directories
#

target_include_directories(${target_testlib}
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

target_link_libraries(${target_testlib}
    PRIVATE

    PUBLIC
    ${DEFAULT_LIBRARIES}
    dl

    INTERFACE
)


if(WIN32)
    # Needed for htons
    target_link_libraries(${target_testlib} PUBLIC ws2_32)
endif(WIN32)


#
# Linker options
#

target_link_libraries(${target_testlib}
    PRIVATE

    PUBLIC
    ${DEFAULT_LINKER_OPTIONS}

    INTERFACE
)

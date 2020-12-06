option(OPTION_ENABLE_PYLC3 OFF)

if (NOT OPTION_ENABLE_PYL3)
  message(STATUS "Lib pylc3 disabled")
  return()
endif()

include_directories(${complx-tools_SOURCE_DIR}/lc3test)
include_directories(${complx-tools_SOURCE_DIR}/liblc3)
include_directories(${complx-tools_SOURCE_DIR}/logging)

# SET(Boost_USE_STATIC_LIBS OFF)
set(CMAKE_SHARED_LINKER_FLAGS "-Wl,--export-all-symbols")

find_package(PythonInterp REQUIRED)
find_package(PythonLibs REQUIRED)
find_package(Boost COMPONENTS python REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})
include_directories(${PYTHON_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIR})

set(CASTXML_PATH "/usr/bin/castxml" CACHE STRING "where to find the castXML executable")

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/PyLC3Gen.cpp ${CMAKE_CURRENT_SOURCE_DIR}/exposed_decl.pypp.txt ${CMAKE_CURRENT_SOURCE_DIR}/named_tuple.py
    COMMAND python pylc3_generator.py ${CASTXML_PATH}
    DEPENDS PyLC3.hpp
    MAIN_DEPENDENCY pylc3_generator.py
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Generating PyLC3Gen.cpp"
)

set(SRC_PYLC3
    PyLC3.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/PyLC3Gen.cpp
)

add_library(
    pylc3
    MODULE
    ${SRC_PYLC3}
)

target_link_libraries(
    pylc3
    lc3
    ${Boost_LIBRARIES}
    ${PYTHON_LIBRARIES}
)

if (ENABLE_REPLAY)
add_definitions(-DENABLE_LC3_REPLAY)
include_directories("../replay")
target_link_libraries(
    pylc3
    lc3_replay
)
endif(ENABLE_REPLAY)

set_target_properties(pylc3 PROPERTIES SUFFIX .so)
set_target_properties(pylc3 PROPERTIES PREFIX "")

if(SKBUILD)
    find_package(PythonExtensions REQUIRED)
    python_extension_module(pylc3)

    install(TARGETS pylc3 LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
    install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/unittests DESTINATION pyLC3/)
    install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/cli DESTINATION pyLC3/)

    install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/unittests/lc3_unit_test_case_test.py DESTINATION tests/)
    install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/cli/comp_test.py DESTINATION tests/)
else(SKBUILD)
    install(TARGETS pylc3 LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

    add_custom_command(
        TARGET pylc3 POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_BINARY_DIR}/pylc3/pylc3.so ${CMAKE_CURRENT_SOURCE_DIR}/pylc3.so
        COMMENT "Copying needed plugins for testing")

    get_filename_component(PYTHON_WORKING_DIR ${CMAKE_CURRENT_SOURCE_DIR} PATH)
    add_test(NAME lc3_unit_test_case_test COMMAND ${PYTHON_EXECUTABLE} -m pylc3.unittests.lc3_unit_test_case_test WORKING_DIRECTORY ${PYTHON_WORKING_DIR})
    add_test(NAME comp_test COMMAND ${PYTHON_EXECUTABLE} -m pylc3.cli.comp_test WORKING_DIRECTORY ${PYTHON_WORKING_DIR})
endif(SKBUILD)

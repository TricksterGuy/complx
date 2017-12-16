set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../cmake)
find_package(Boost COMPONENTS unit_test_framework REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})
include_directories(../liblc3)

include(CodeCoverage)

if(CMAKE_HOST_UNIX)
    add_definitions(-DBOOST_TEST_DYN_LINK=1)
endif(CMAKE_HOST_UNIX)

APPEND_COVERAGE_COMPILER_FLAGS()

file(GLOB SRC_TESTS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp)

add_executable(
    lc3_test_suite
    ${SRC_TESTS}
)

target_link_libraries(
    lc3_test_suite
    lc3
    ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
)

set_target_properties(lc3_test_suite PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_test(NAME lc3_test_suite WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/lc3_test_suite)

setup_target_for_coverage(
    NAME "lc3_test_coverage"
    EXECUTABLE lc3_test_suite -j ${PROCESSOR_COUNT}
    DEPENDENCIES lc3_test_suite
)

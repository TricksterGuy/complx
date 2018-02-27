include_directories(${complx-tools_SOURCE_DIR}/lc3test)
include_directories(${complx-tools_SOURCE_DIR}/liblc3)
include_directories(${complx-tools_SOURCE_DIR}/logging)

find_package(PythonInterp REQUIRED)
find_package(PythonLibs REQUIRED)
find_package(Boost COMPONENTS python REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})
include_directories(${PYTHON_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIR})

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/PyLC3Gen.cpp ${CMAKE_CURRENT_SOURCE_DIR}/exposed_decl.pypp.txt ${CMAKE_CURRENT_SOURCE_DIR}/named_tuple.py
    COMMAND python pylc3_generator.py
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
    SHARED
    ${SRC_PYLC3}
)

target_link_libraries(
    pylc3
    lc3
    ${wxWidgets_LIBRARIES}
    ${Boost_LIBRARIES}
    ${PYTHON_LIBRARIES}
)

set_target_properties(pylc3 PROPERTIES SUFFIX .so)
set_target_properties(pylc3 PROPERTIES PREFIX "")

option(ENABLE_RANDOM "Enable the Random Value Plugin" ON)
option(ENABLE_SECOND_TIMER "Enable the Second Timer Plugin" ON)
option(ENABLE_TIMER "Enable the Timer Plugin" ON)
include_directories(${complx-tools_SOURCE_DIR}/liblc3)
set(CMAKE_SKIP_RPATH TRUE)

if(ENABLE_RANDOM)

set(SRC_RANDOM
    random/random.cpp
)

add_library(
    lc3_random
    SHARED
    ${SRC_RANDOM}
)

target_link_libraries(
    lc3_random
    lc3
)

install(TARGETS lc3_random LIBRARY DESTINATION lib)

endif(ENABLE_RANDOM)


if(ENABLE_SECOND_TIMER)

set(SRC_SECOND_TIMER
    second_timer/second_timer.cpp
)

add_library(
    lc3_second_timer
    SHARED
    ${SRC_SECOND_TIMER}
)

target_link_libraries(
    lc3_second_timer
    lc3
)

install(TARGETS lc3_second_timer LIBRARY DESTINATION lib)

endif(ENABLE_SECOND_TIMER)

if(ENABLE_TIMER)

set(SRC_TIMER
    timer/timer.cpp
)

add_library(
    lc3_timer
    SHARED
    ${SRC_TIMER}
)

target_link_libraries(
    lc3_timer
    lc3
)

install(TARGETS lc3_timer LIBRARY DESTINATION lib)

endif(ENABLE_TIMER)


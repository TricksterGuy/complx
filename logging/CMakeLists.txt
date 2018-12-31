set(SRC_LOGGING
    logger.cpp
)

add_library(
    logger
    SHARED
    ${SRC_LOGGING}
)

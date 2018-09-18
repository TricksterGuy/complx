add_subdirectory(devices)
add_subdirectory(instructions)
add_subdirectory(traps)

if(ENABLE_COMPLX)
add_subdirectory(other)
endif(ENABLE_COMPLX)

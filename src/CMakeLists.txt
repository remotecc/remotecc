
# Add executable: remotecc
set(SRC_COMMON_FILES)

add_executable(remotecc
    ${CMAKE_CURRENT_LIST_DIR}/remotecc.cpp
    ${SRC_COMMON_FILES})
rcc_set_common_properties(remotecc)

# Add executable: remoteccd
add_executable(remoteccd
    ${CMAKE_CURRENT_LIST_DIR}/remoteccd.cpp
    ${SRC_COMMON_FILES})
rcc_set_common_properties(remoteccd)


# remoteccd supports using MPI for daemon to daemon communication
option(REMOTECC_DAEMON_MPI_SUPPORT
    "Support using MPI for daemon to daemon communication"
    ON)
if (REMOTECC_DAEMON_MPI_SUPPORT)
    find_package(MPI)
    if (MPI_CXX_FOUND)
        message(STATUS "MPI found for remoteccd")
        message(STATUS "MPIEXEC: ${MPIEXEC}")
        message(STATUS "MPIEXEC_NUMPROC_FLAG: ${MPIEXEC_NUMPROC_FLAG}")
        message(STATUS "MPIEXEC_PREFLAGS: ${MPIEXEC_PREFLAGS}")
        message(STATUS "MPIEXEC_POSTFLAGS: ${MPIEXEC_POSTFLAGS}")

        message(STATUS "MPI_CXX_INCLUDE_PATH: ${MPI_CXX_INCLUDE_PATH}")
        message(STATUS "MPI_CXX_COMPILE_FLAGS: ${MPI_CXX_COMPILE_FLAGS}")
        message(STATUS "MPI_CXX_LIBRARIES: ${MPI_CXX_LIBRARIES}")
        message(STATUS "MPI_CXX_LINK_FLAGS: ${MPI_CXX_LINK_FLAGS}")

        # If MPI is detected, add MPI related options to remoteccd
        # CMake adds imported target MPI::MPI_<lang> from 3.9.0
        # See https://cmake.org/cmake/help/v3.9/release/3.9.html#modules
        if (TARGET MPI::MPI_CXX)
            target_link_libraries(remoteccd PRIVATE MPI::MPI_CXX)
        else()
            target_include_directories(remoteccd PRIVATE ${MPI_CXX_INCLUDE_PATH})
            target_compile_options(remoteccd PRIVATE ${MPI_CXX_COMPILE_FLAGS})
            target_link_libraries(remoteccd PRIVATE ${MPI_CXX_LIBRARIES})
            target_link_options_private(remoteccd ${MPI_CXX_LINK_FLAGS})
        endif()
        target_compile_definitions(remoteccd PRIVATE REMOTECC_DAEMON_MPI_SUPPORT=1)
    else()
        message(WARNING "MPI for CXX is not found: remoteccd won't be compiled with MPI support")
    endif()
else()
    message(STATUS "MPI support for remoteccd is disabled")
endif()


# Installation
install(
    TARGETS remotecc remoteccd
    RUNTIME
    DESTINATION bin)

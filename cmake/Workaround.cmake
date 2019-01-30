
# Detect CMake version
macro(_rcc_set_cmake_version_at_least Major Minor Patch)
    if (${CMAKE_VERSION} VERSION_GREATER "${Major}.${Minor}.${Patch}" OR
        ${CMAKE_VERSION} VERSION_EQUAL   "${Major}.${Minor}.${Patch}")
        message(STATUS "CMake version detection: set RCC_CMAKE_VERSION_AT_LEAST_${Major}_${Minor}_${Patch} to ON")
        set(RCC_CMAKE_VERSION_AT_LEAST_${Major}_${Minor}_${Patch} ON)
    else()
        message(STATUS "CMake version detection: set RCC_CMAKE_VERSION_AT_LEAST_${Major}_${Minor}_${Patch} to nothing")
        set(RCC_CMAKE_VERSION_AT_LEAST_${Major}_${Minor}_${Patch})
    endif()
endmacro()

message(STATUS "CMake version: ${CMAKE_VERSION}")
_rcc_set_cmake_version_at_least(3 6 0)
_rcc_set_cmake_version_at_least(3 7 0)
_rcc_set_cmake_version_at_least(3 8 0)
_rcc_set_cmake_version_at_least(3 9 0)
_rcc_set_cmake_version_at_least(3 10 0)
_rcc_set_cmake_version_at_least(3 11 0)
_rcc_set_cmake_version_at_least(3 12 0)
_rcc_set_cmake_version_at_least(3 13 0)



#
# target_link_options_private(Target, options...)
#   Add linker options to Target
# target_link_options() is introduced by CMake 3.13.0, after that we may use target_link_options for clear intent.
# Before that, target_link_libraries is used.
# NOTE:
#   As the name suggests, the options is added as "PRIVATE"
#
function(target_link_options_private Target #[[options...]])
    foreach(opt ${ARGN})
        if (RCC_CMAKE_VERSION_AT_LEAST_3_13_0)
            target_link_options(${Target} PRIVATE ${opt})
        else()  # CMake < 3.13.0
            target_link_libraries(${Target} PRIVATE ${opt})
        endif()
    endforeach()
endfunction()



function(target_compile_options_if_available Target Scope Option)
    include(CheckCXXCompilerFlag)
    CHECK_CXX_COMPILER_FLAG("${Option}" "COMPILER_FLAG")
    if ("${COMPILER_FLAG}")
        message(STATUS "Add compile option to target ${Target}: ${Option}")
        target_compile_options(${Target} ${Scope} "${Option}")
    else()
        message(STATUS "Compile option is not supported: ${Option}")
    endif()
endfunction()

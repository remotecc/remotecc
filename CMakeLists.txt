cmake_minimum_required(VERSION 3.5.0 FATAL_ERROR)

project(remotecc
    VERSION 0.0.0
    LANGUAGES C CXX)

# Choose _WIN32_WINNT definition on platform WINDOWS (OS:　Windows or Cygwin)
if (WIN32 OR CYGWIN)
    set(REMOTECC_WIN32_WINNT "_WIN32_WINNT_WIN7" CACHE STRING
        "Define _WIN32_WINNT on Windows or Cygwin (default: Windows 7)")
endif()


# Add necessary CMake modules and scripts
list(INSERT CMAKE_MODULE_PATH 0 "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(cmake/Workaround.cmake)
include(cmake/ImportedTarget.cmake)


# Treat warning as error (default: OFF)
# Just enable this when doing continuous integration
option(REMOTECC_TREAT_WARNINGS_AS_ERRORS
    "Treat compiler warnings as errors, most for CI only (default: OFF)"
    OFF)
if (REMOTECC_TREAT_WARNINGS_AS_ERRORS)
    message(STATUS "NOTE: Treat all warnings as errors!")
endif()


# Add thirdparty libraries
include(thirdparty/CMakeLists.txt)


# Create infra INTERFACE library
# This is needed by all executables and tests
include(infra/CMakeLists.txt)


# function:
#   rcc_set_common_properties(Target)
# Add common properties to `Target`
function(rcc_set_common_properties Target)
    # "Link" to infra INTERFACE library
    # This includes necessary headers and definitions
    target_link_libraries(${Target} PRIVATE infra)

    # Also, "link" with boost headers
    target_link_libraries(${Target} PRIVATE Boost::boost)

    # Add definition __THIS_IS_${Target:Upper}__=1 to Target:
    string(TOUPPER ${Target} TARGET_UPPERCASE)
    target_compile_definitions(${Target} PRIVATE __THIS_IS_${TARGET_UPPERCASE}__=1)

    # Add some definitions on platform WINDOWS (OS:　Windows or Cygwin)
    if (WIN32 OR CYGWIN)
        target_compile_definitions(${Target} PRIVATE _WIN32_WINNT=${REMOTECC_WIN32_WINNT})
        target_compile_definitions(${Target} PRIVATE NOMINMAX)
    endif()

    # Enable C11, C++14
    set_property(TARGET ${Target} PROPERTY C_STANDARD 11)
    set_property(TARGET ${Target} PROPERTY CXX_STANDARD 14)

    # We don't set C_STANDARD_REQUIRED or CXX_STANDARD_REQUIRED to ON here, to avoid potential (unnecessarily) failure.
    # If a required language feature is not supported by compiler, compilation just fails.
    #set_property(TARGET ${Target} PROPERTY C_STANDARD_REQUIRED ON)
    #set_property(TARGET ${Target} PROPERTY CXX_STANDARD_REQUIRED ON)

    # Don't disable C/C++ compiler specific extensions
    # We use these features to workaround flaws in C++
    # e.g., how to deal with comma before __VA_ARGS__ in variadic macros
    set_property(TARGET ${Target} PROPERTY C_EXTENSIONS ON)
    set_property(TARGET ${Target} PROPERTY CXX_EXTENSIONS ON)

    # Enable all compiler warnings by default
    if (MSVC)
        target_compile_options(${Target} PRIVATE "/W4")
    else()
        target_compile_options_if_available(${Target} PRIVATE "-Wall")
        target_compile_options_if_available(${Target} PRIVATE "-Wextra")
    endif()

    # Treat warnings as errors?
    if (REMOTECC_TREAT_WARNINGS_AS_ERRORS)
        if (MSVC)
            # MSVC has "/WX" for both the compiler and the linker
            # (Compiler) /WX: Treat all compiler warnings as errors
            # (Linker) /WX: Treat linker warnings as errors
            # See:
            #   https://docs.microsoft.com/en-us/cpp/build/reference/compiler-option-warning-level
            #   https://docs.microsoft.com/en-us/cpp/build/reference/wx-treat-linker-warnings-as-errors
            target_compile_options(${Target} PRIVATE "/WX")
            target_link_options(${Target} PRIVATE "/WX")
        else()
            target_compile_options_if_available(${Target} PRIVATE "-Werror")
        endif()
    endif()

endfunction()


# Add src/
include(src/CMakeLists.txt)


# Do we build unit tests?
if ("${CMAKE_CURRENT_SOURCE_DIR}" STREQUAL "${CMAKE_SOURCE_DIR}")
    # remotecc is built as top project: enable unit tests by default
    set(_RCC_ENABLE_UNITTEST_DEFAULT ON)
else()
    # remotecc is built as "embedded" project: disable unit-test by default
    set(_RCC_ENABLE_UNITTEST_DEFAULT OFF)
endif()
option(REMOTECC_ENABLE_UNITTEST
    "Enable remotecc unit tests"
    ${_RCC_ENABLE_UNITTEST_DEFAULT})

if (REMOTECC_ENABLE_UNITTEST)
    message(STATUS "Unit tests are enabled")
    enable_testing()
    include(test/CMakeLists.txt)
else()
    message(STATUS "Unit tests are disabled")
endif ()

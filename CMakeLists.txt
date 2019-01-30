cmake_minimum_required(VERSION 3.5.0 FATAL_ERROR)

project(remotecc
    VERSION 0.0.0
    LANGUAGES C CXX)


# Choose _WIN32_WINNT definition on platform WINDOWS (OS: Windows or Cygwin)
if (WIN32 OR CYGWIN)
    set(REMOTECC_WIN32_WINNT "0x0601" CACHE STRING
        "Define _WIN32_WINNT on Windows or Cygwin (default: Windows 7)")
endif()


# Add necessary CMake modules and scripts
list(INSERT CMAKE_MODULE_PATH 0 "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(cmake/Workaround.cmake)
include(cmake/ImportedTarget.cmake)


# Policy CMP0079: (CMake >= 3.13.0)
# ``target_link_libraries()`` allows use with targets in other directories.
# No matter OLD or NEW. Just set one to make CMake happy.
if (RCC_CMAKE_VERSION_AT_LEAST_3_13_0)
    cmake_policy(SET CMP0079 NEW)
endif()


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

    # Link with thread library
    set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
    find_package(Threads REQUIRED)
    target_link_libraries(${Target} PRIVATE Threads::Threads)

    # Add some definitions on platform WINDOWS (OS: Windows or Cygwin)
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
        # Remove the default warning level (if any)
        # See: https://stackoverflow.com/questions/45995784/how-to-set-compiler-options-with-cmake-in-visual-studio-2017
        # TODO: need confirmation
        string(REGEX REPLACE "/W[0-4]" "" CMAKE_CXX_FLAGS_INIT "${CMAKE_CXX_FLAGS_INIT}")
        # Then set /W4 for MSVC
        target_compile_options(${Target} PRIVATE "/W4")

        # Disable C4127: conditional expression is constant
        # https://docs.microsoft.com/en-us/cpp/error-messages/compiler-warnings/compiler-warning-level-4-c4127
        target_compile_options(${Target} PRIVATE "/wd4127")
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

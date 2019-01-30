# Create an INTERFACE library "infra", other executables "links" to it
add_library(infra INTERFACE)

# Add current directory (which contains "infra.hpp") to #include directory
target_include_directories(infra BEFORE INTERFACE ${CMAKE_CURRENT_LIST_DIR})

# infra takes advantage of Boost
target_link_libraries(infra INTERFACE Boost::boost)

target_sources(infra INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/infra/macro.hpp
    ${CMAKE_CURRENT_LIST_DIR}/infra.cpp
    ${CMAKE_CURRENT_LIST_DIR}/infra.hpp)


# Adding compile_definitions / include_directories / link_libraries to imported targets
# is not supported before CMake 3.11.0
# In this case, we modify INTERFACE_XXX property of the imported target directly

function(imported_target_compile_definitions Target #[[defs]])
    foreach(def ${ARGN})
        if (RCC_CMAKE_VERSION_AT_LEAST_3_11_0)
            target_compile_definitions(${Target} INTERFACE ${def})
        else()  # CMake < 3.11.0
            set_property(
                TARGET ${Target}
                APPEND
                PROPERTY INTERFACE_COMPILE_DEFINITIONS ${def})
        endif()
    endforeach()
endfunction()


function(imported_target_include_directories Target #[[dirs]])
    foreach(dir ${ARGN})
        if (RCC_CMAKE_VERSION_AT_LEAST_3_11_0)
            target_include_directories(${Target} BEFORE INTERFACE ${dir})
        else()  # CMake < 3.11.0
            set_property(
                TARGET ${Target}
                APPEND
                PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${dir})
        endif()
    endforeach()
endfunction()


function(imported_target_link_libraries Target #[[libs]])
    foreach(lib ${ARGN})
        if (RCC_CMAKE_VERSION_AT_LEAST_3_11_0)
            target_link_libraries(${Target} INTERFACE ${lib})
        else()  # CMake < 3.11.0
            set_property(
                TARGET ${Target}
                APPEND
                PROPERTY INTERFACE_LINK_LIBRARIES ${lib})
        endif()
    endforeach()
endfunction()

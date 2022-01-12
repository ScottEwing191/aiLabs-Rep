
if(CMAKE_VERSION VERSION_GREATER_EQUAL 3.3)
    cmake_policy(PUSH)
    cmake_policy(SET CMP0057 NEW)
    if(NOT "CONFIG" IN_LIST ARGS AND NOT "NO_MODULE" IN_LIST ARGS AND "HDF5" IN_LIST ARGS)
        # The caller hasn't said "CONFIG", so they want the built-in FindHDF5.cmake behavior. Set configurations macros to ensure the built-in script finds us.
        if("dynamic" STREQUAL "static")
            set(HDF5_USE_STATIC_LIBRARIES ON)
            add_compile_definitions(H5_BUILT_AS_STATIC_LIB)
        else()
            set(HDF5_USE_STATIC_LIBRARIES OFF)
            add_compile_definitions(H5_BUILT_AS_DYNAMIC_LIB)
        endif()
    endif()
    cmake_policy(POP)
endif()
_find_package(${ARGS})

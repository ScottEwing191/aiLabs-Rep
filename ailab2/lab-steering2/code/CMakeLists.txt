cmake_minimum_required(VERSION 3.0)

#if (DEFINED ENV{VCPKG_ROOT})
#  set(VCPKG_CMAKE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")
#  list(APPEND CMAKE_TOOLCHAIN_FILE ${VCPKG_CMAKE})
#endif ()

set(CMAKE_TOOLCHAIN_FILE "C://Users//B00371006//Desktop//aiLabs-Rep//vcpkg-export-20220111-084639//scripts//buildsystems//vcpkg.cmake")

set(RLCPP "${CMAKE_CURRENT_SOURCE_DIR}/../../raylib-cpp-4.0.0")
set(RLEXTRAS "${CMAKE_CURRENT_SOURCE_DIR}/../../raylib-extras")

project(lab-week3-steering2)

find_package(raylib CONFIG REQUIRED)

foreach(n steering3)
  add_executable(${n} ${n}.cpp)
  target_include_directories(${n} PRIVATE ${RAYLIB_INCLUDE_DIRS})
  target_include_directories(${n} PRIVATE ${RLCPP}/include ${RLEXTRAS})
  target_link_libraries(${n} PRIVATE ${RAYLIB_LIBRARIES})

  if (UNIX)
    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
      target_link_libraries(${n} PRIVATE asan ubsan)
    endif ()

    target_link_libraries(${n} PRIVATE m dl pthread)
  endif()

endforeach(n)

set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT steering3)


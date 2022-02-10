cmake_minimum_required(VERSION 3.0)

if (DEFINED ENV{VCPKG_ROOT})
  set(VCPKG_CMAKE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")
  list(APPEND CMAKE_TOOLCHAIN_FILE ${VCPKG_CMAKE})
endif ()

set(CMAKE_CXX_STANDARD 17) # for std::variant

set(NAME astar-steering-assignment)
project(${NAME})

set(RLCPP "${CMAKE_CURRENT_SOURCE_DIR}/../../raylib-cpp-4.0.0")
set(RLEXTRAS "${CMAKE_CURRENT_SOURCE_DIR}/../../raylib-extras")

find_package(raylib CONFIG REQUIRED)

add_executable(${NAME} ${NAME}.cpp)
target_include_directories(${NAME} PRIVATE ${RAYLIB_INCLUDE_DIRS})
target_include_directories(${NAME} PRIVATE ${RLCPP}/include ${RLEXTRAS})

target_link_libraries(${NAME} PRIVATE ${RAYLIB_LIBRARIES})

if (UNIX)
  if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_link_libraries(${NAME} PRIVATE asan ubsan)
  endif ()

  target_link_libraries(${NAME} PRIVATE m dl pthread)
endif()

set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT ${NAME})


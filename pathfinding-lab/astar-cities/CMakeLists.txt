cmake_minimum_required(VERSION 3.7)   # VS_STARTUP_PROPERTY

if (DEFINED ENV{VCPKG_ROOT})
  set(VCPKG_CMAKE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")
  list(APPEND CMAKE_TOOLCHAIN_FILE ${VCPKG_CMAKE})
endif ()

set(name astar-cities)
project(${name})

add_executable(${name} ${name}.cpp)

set(Boost_USE_STATIC_LIBS ON)     # only find static libs
find_package(Boost REQUIRED) # COMPONENTS filesystem system)

target_include_directories(${name} PRIVATE ${Boost_INCLUDE_DIRS})
set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT ${name})




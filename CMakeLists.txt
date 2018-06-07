# See README.md for instruction on how to build SHAD with CMake
cmake_minimum_required(VERSION 3.8)

project(SHAD ${cmake_3_0_PROJ_VERSION} ${cmake_3_0_LANGUAGES} CXX)
set(CMAKE_CXX_STANDARD 14)

if (NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "No build type selected, default to Debug")
  set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Build type (default Debug)" FORCE)
endif()


# No in-tree build allowed.
if (CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
  message(FATAL_ERROR
    "In-source build are not allowed.
Please create a directory directory and run cmake from there, passing the path
to this source directory as the last argumente.
This process created the file `CMakeCache.txt' and the directory `CMakeFiles'.
Please delete them.")
endif()


string(TOUPPER "${CMAKE_BUILD_TYPE}" uppercase_CMAKE_BUILD_TYPE)

if (CMAKE_BUILD_TYPE AND
    NOT uppercase_CMAKE_BUILD_TYPE MATCHES "^(DEBUG|RELEASE|RELWITHDEBINFO|MINSIZEREL)$")
  message(FATAL_ERROR "Invalid value for CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")
endif()

set(
  DEMO_RUNTIME_SYSTEM "TBB" CACHE STRING
  "Runtime system to be used as backend of the Abstract Runtime API (Default=TBB, Supported=TBB | GMT)")


# Add path for custom CMake modules
set(CMAKE_MODULE_PATH
  ${CMAKE_MODULE_PATH}
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

if (DEMO_RUNTIME_SYSTEM STREQUAL "TBB")
  find_package(TBB REQUIRED)
  if (TBB_FOUND)
    list(APPEND RT ${TBB_LIBRARIES})
  endif()
endif()

if(DEMO_RUNTIME_SYSTEM STREQUAL "GMT")
  find_package(GMT REQUIRED)
  include_directories(${GMT_INCLUDE_DIRS})
  list(APPEND RT ${GMT_LIBRARIES})
endif()

find_package(SHAD REQUIRED)
if (SHAD_FOUND)
  list(APPEND RT ${SHAD_LIBRARIES})
endif()
  
include_directories(${SHAD_INCLUDE_DIRS})
include_directories(include)

add_executable(hello-shad src/hello-shad.cc)
target_link_libraries(hello-shad ${RT})

add_executable(tc src/reader.cc src/tc.cc)
target_link_libraries(tc ${RT})

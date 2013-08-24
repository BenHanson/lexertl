#
# Part of CMake configuration for lexertl library
#
# Copyright (c) 2013 Mateusz Loskot <mateusz@loskot.net>
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file licence_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#
cmake_minimum_required (VERSION 2.8)
project(lexertl)

if(MSVC)
  if(MSVC80 OR MSVC90 OR MSVC10 OR MSVC11)
    add_definitions(-D_CRT_SECURE_NO_DEPRECATE)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    add_definitions(-D_CRT_NONSTDC_NO_WARNING)
    add_definitions(-D_SCL_SECURE_NO_WARNINGS)
  endif()

  if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
    string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
  else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
  endif()
endif()

if(CMAKE_COMPILER_IS_GNUCXX)
  execute_process(COMMAND
    ${CMAKE_C_COMPILER} -dumpversion OUTPUT_VARIABLE GCC_VERSION)

  if(GCC_VERSION VERSION_GREATER 4.8 OR GCC_VERSION VERSION_EQUAL 4.8)
    set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wpedantic -Wno-long-long")
  else()
    set(CMAKE_CXX_FLAGS "-Wall -Wextra -pedantic -Wno-long-long")
  endif()
endif()

if(CMAKE_CXX_COMPILER MATCHES "clang")
  set(CMAKE_CXX_FLAGS "-Wall -Wextra -pedantic -Wno-long-long")
endif()

include_directories(${CMAKE_SOURCE_DIR}/lexertl)

add_subdirectory(examples)

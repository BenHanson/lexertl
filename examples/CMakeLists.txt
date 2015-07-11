#
# Part of CMake configuration for lexertl library
#
# Copyright (c) 2013 Mateusz Loskot <mateusz@loskot.net>
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file licence_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#
set (EXCLUDE_DIR "/CMakeFiles/")
file(GLOB dirs RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/*")
foreach(dir ${dirs})
  if(IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/${dir}")
    message(STATUS "Adding example: ${dir}")
    add_subdirectory("${dir}")
  endif()
endforeach()

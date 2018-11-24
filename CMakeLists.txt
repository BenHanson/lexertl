#
# Part of CMake configuration for lexertl library
#
# Copyright (c) 2013 Mateusz Loskot <mateusz@loskot.net>
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file licence_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#
cmake_minimum_required (VERSION 3.5)
project(lexertl VERSION 0.1.0)

# Define project
add_library(lexertl INTERFACE)
target_include_directories(lexertl INTERFACE 
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
	$<INSTALL_INTERFACE:include>
)

# Add custom requirements for the compiler
if(MSVC)
  if(MSVC80 OR MSVC90 OR MSVC10 OR MSVC11 OR MSVC12)
    add_definitions(-D_CRT_SECURE_NO_DEPRECATE)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    add_definitions(-D_CRT_NONSTDC_NO_WARNING)
    add_definitions(-D_SCL_SECURE_NO_WARNINGS)
  endif()

  if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
    string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
  else()
    target_compile_options(lexertl PRIVATE "/W4")
  endif()
endif()

if(CMAKE_COMPILER_IS_GNUCXX)
  execute_process(COMMAND
    "${CMAKE_C_COMPILER}" -dumpversion OUTPUT_VARIABLE GCC_VERSION)

  if(GCC_VERSION VERSION_LESS 4.3)
    message(WARNING "GCC 4.3+ is recommended, otherwise compilation may fail")
  endif()

  target_compile_options(lexertl PRIVATE "-Wall -Wextra -Wno-long-long -std=c++0x")

  if(GCC_VERSION VERSION_GREATER 4.8 OR GCC_VERSION VERSION_EQUAL 4.8)
	target_compile_options(lexertl PRIVATE "-Wpedantic")
  else()
	target_compile_options(lexertl PRIVATE "-pedantic")
  endif()
endif()

if(CMAKE_CXX_COMPILER MATCHES "clang")
  target_compile_options(lexertl PRIVATE "-Wall -Wextra -pedantic -Wno-long-long")
endif()

# Add the examples
add_subdirectory(examples)

# Generate install
install(TARGETS lexertl EXPORT lexertlTargets
	ARCHIVE DESTINATION lib
	LIBRARY DESTINATION lib
	RUNTIME DESTINATION bin
	PUBLIC_HEADER DESTINATION include
)
install(DIRECTORY include/lexertl DESTINATION include)
install(EXPORT lexertlTargets
	FILE lexertlTargets.cmake
	NAMESPACE lexertl::
	DESTINATION lib/cmake/lexertl
)
include(CMakePackageConfigHelpers)
write_basic_package_version_file("lexertlConfigVersion.cmake"
	VERSION ${lexertl_VERSION}
	COMPATIBILITY SameMajorVersion
)
install(FILES "lexertlConfig.cmake" "${CMAKE_CURRENT_BINARY_DIR}/lexertlConfigVersion.cmake"
	DESTINATION lib/cmake/lexertl
)


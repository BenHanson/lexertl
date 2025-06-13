#
# Part of CMake configuration for lexertl library
#
# Copyright (c) 2013 Mateusz Loskot <mateusz@loskot.net>
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file licence_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#
cmake_minimum_required (VERSION 3.5)
project(lexertl VERSION 1.0.0)

# Define project
add_library(lexertl INTERFACE)
target_include_directories(lexertl INTERFACE 
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
	$<INSTALL_INTERFACE:include>
)

# Generate install
include(GNUInstallDirs)

install(TARGETS lexertl EXPORT lexertlTargets
	ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
	PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

export(EXPORT lexertlTargets
 FILE ${CMAKE_BINARY_DIR}/cmake/lexertlTargets.cmake
 NAMESPACE lexertl::
)

install(DIRECTORY include/lexertl DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(EXPORT lexertlTargets
	FILE lexertlTargets.cmake
	NAMESPACE lexertl::
	DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/lexertl
)
include(CMakePackageConfigHelpers)

configure_package_config_file(
  ${CMAKE_SOURCE_DIR}/cmake/lexertlConfig.cmake.in
  ${CMAKE_BINARY_DIR}/cmake/lexertlConfig.cmake
  INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/lexertl
)

write_basic_package_version_file(
    ${CMAKE_BINARY_DIR}/cmake/lexertlConfigVersion.cmake
	COMPATIBILITY AnyNewerVersion
)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/cmake/lexertlConfig.cmake" "${CMAKE_CURRENT_BINARY_DIR}/cmake/lexertlConfigVersion.cmake"
	DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/lexertl
)


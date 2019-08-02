# Copyright (c) 2014-2015 The Regents of the University of California.
# All Rights Reserved.
#
# Permission is hereby granted, free of charge, to any person obtaining
# a copy of this software and associated documentation files (the
# "Software"), to deal in the Software without restriction, including
# without limitation the rights to use, copy, modify, merge, publish,
# distribute, sublicense, and/or sell copies of the Software, and to
# permit persons to whom the Software is furnished to do so, subject
# to the following conditions:
#
# The above copyright notice and this permission notice shall be included
# in all copies or substantial portions of the Software.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
# IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
# TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
# PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER
# OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
# EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
# PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
# PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
# LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
# NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
# Set cpack variables to generate a plugin installer.
#
# The plugin installers for the different platforms are
#
#   Linux - The plugin is packaged in a DEB file using the 'deb' cpack generator.
#   MacOS - The plugin is packaged in a PKG file using the 'productbuild' cpack generator.
#
# The plugin install directory structure is defined by the variables
#
#    SV_PLUGIN_INSTALL_DIR/SimVascular_VERSION/PLUGIN_NAME/PLUGIN_INSTALL_VERSION_DIR
#
# These variables are derived from variables set in SimVascular CMake files
#
#     SimVascular_VERSION - The date sv was built on a local machine, set in SimVascular/build/SimVascular-build/SimVascularConfig.cmake
#
#     SV_FULL_VERSION - The current plugin build date, set in SimVascular/Code/CMake/SimVascularSystemSetup.cmake
#
#     SV_PLUGIN_INSTALL_DIR - The plugins install directory, set in SimVascular/Code/CMake/SimVascularSystemSetup.cmake.
#                             This includes the main install directory and SimVascular version.
#
include(CPackComponent)

## Set general package information.
#
set(PLUGIN_NAME "Purkinje-Plugin")
set(CPACK_PACKAGE_NAME "simvascular-purkinje-plugin")
set(CPACK_PACKAGE_VENDOR "SimVascular.org")
set(CPACK_PACKAGE_CONTACT "SimVascular<sv@simvascular.org>")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Purkinje plugin")
set(PLUGIN_INSTALL_SCRIPT_DIR ".")
#set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/ReadMe.txt")
#set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/Copyright.txt")

## Set the plugin installation directory.
#
# We wanted to set the install directory with the simvascular build version 
# but SimVascular_VERSION is not defined when the directory is set.
# 
string(REPLACE "." "-" SV_INSTALL_VERSION_DIR ${SimVascular_VERSION})
string(REPLACE "." "-" SV_PLUGIN_INSTALL_VERSION_DIR ${SV_FULL_VERSION})

if (UNIX)
  set(CPACK_PACKAGING_INSTALL_PREFIX "${SV_PLUGIN_INSTALL_DIR}/${SV_PLUGIN_INSTALL_VERSION_DIR}/${PLUGIN_NAME}/${SV_PLUGIN_INSTALL_VERSION_DIR}")
endif()

message(STATUS "SimVascular_VERSION: ${SimVascular_VERSION}")
message(STATUS "SV_FULL_VERSION: ${SV_FULL_VERSION}")
message(STATUS "SV_PLUGIN_INSTALL_DIR: ${SV_PLUGIN_INSTALL_DIR}")
message(STATUS "SV_PLUGIN_INSTALL_VERSION_DIR: ${SV_PLUGIN_INSTALL_VERSION_DIR}")
message(STATUS "CMAKE_CURRENT_BINARY_DIR: ${CMAKE_CURRENT_BINARY_DIR}")
message(STATUS "CPACK_PACKAGING_INSTALL_PREFIX: ${CPACK_PACKAGING_INSTALL_PREFIX}")
message(STATUS "CPACK_PACKAGE_INSTALL_DIRECTORY: ${CPACK_PACKAGE_INSTALL_DIRECTORY}")
message(STATUS "CPACK_OUTPUT_FILE_PREFIX: ${CPACK_OUTPUT_FILE_PREFIX}")

## Set the generators used to create the install file and create a setup.sh script
# that sets environment variables to locate the plugin and Parasolid kernel libraries.
#
if(APPLE)
  set(CPACK_GENERATOR "productbuild")
  #set(CPACK_COMPONENTS_ALL libraries)
  set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-darwin-x64-${SV_FULL_VERSION}${INSTALL_TYPE_STR}")
  set(PLUGIN_SETUP_SCRIPT "setup.sh") 
  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/setup-macos.in ${PLUGIN_SETUP_SCRIPT})

elseif(LINUX)
  set(CPACK_GENERATOR DEB)
  set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-linux-x64-${SV_FULL_VERSION}${INSTALL_TYPE_STR}")
  set(PLUGIN_SETUP_SCRIPT "setup.sh") 
  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/setup-linux.in ${PLUGIN_SETUP_SCRIPT})
endif()

## Install setup script into plugin directory.
install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/${PLUGIN_SETUP_SCRIPT} DESTINATION ${PLUGIN_INSTALL_SCRIPT_DIR})

## Include CPack to introduce the appropriate targets
include(CPack)

#  execute_process(COMMAND install_name_tool -change lib_simvascular_module_purkinjenetwork.dylib bob/lib_simvascular_module_purkinjenetwork.dylib ${CPACK_PACKAGING_INSTALL_PREFIX}/lib/plugins/liborg_sv_gui_qt_purkinjenetwork.dylib)

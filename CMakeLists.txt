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

cmake_minimum_required(VERSION 2.8.12)

set(SV_EXTERNALS_VERSION_NUMBER "2018.05" CACHE STRING "SimVascular Externals Release Version")
message(STATUS "1) [CMakeList.txt] SV_EXTERNALS_VERSION_NUMBER ${SV_EXTERNALS_VERSION_NUMBER}")

project(SV_PLUGIN)
set(PLUGIN_NAME org.sv.gui.qt.purkinjenetwork)
set(SV_LIB_MODULE_PURKINJENETWORK_NAME _simvascular_module_purkinjenetwork)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}")

find_package(SimVascular REQUIRED)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${SimVascular_C_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${SimVascular_CXX_FLAGS}")

include_directories(${SimVascular_INCLUDE_DIRS})

message(STATUS "[CMakeLists] SimVascular_USE_EXTERNALS_FILE '${SimVascular_USE_EXTERNALS_FILE}'")
#include(SimVascularExternalsVersions)
include(${SimVascular_USE_EXTERNALS_FILE})

include_directories(
  "${CMAKE_SOURCE_DIR}/Modules/PurkinjeNetwork"
  "${CMAKE_BINARY_DIR}/Modules/PurkinjeNetwork"
  "${CMAKE_SOURCE_DIR}/Plugins/${PLUGIN_NAME}"
  "${CMAKE_BINARY_DIR}/Plugins/${PLUGIN_NAME}"
  )

add_subdirectory("${CMAKE_SOURCE_DIR}/Modules/PurkinjeNetwork")
add_subdirectory("${CMAKE_SOURCE_DIR}/Plugins/${PLUGIN_NAME}")


# Hey emacs, this is a -*- makefile -*-

# Copyright (c) Stanford University, The Regents of the University of
#               California, and others.
#
# All Rights Reserved.
#
# See Copyright-SimVascular.txt for additional details.
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

TOP=$(SV_TOP)
PURKINJENETWORK_PLUGIN_TOP=..
TARGETLIB_TOP=$(PURKINJENETWORK_PLUGIN_TOP)
include $(TOP)/include.mk
include $(PURKINJENETWORK_PLUGIN_TOP)/include.purkinjenetwork.mk

#
#  The following are helper scripts to create PC
#  distributions.  Note that some of the paths are hardcoded.
#

SV_CLUSTER = $(CLUSTER)

ifeq ($(CLUSTER),x64_cygwin)
  include $(TOP)/MakeHelpers/$(SV_EXTERNALS_VERSION_NUMBER)/wix-3.14.x64_cygwin.mk
endif

SV_TIMESTAMP ?= $(shell date +"%Y-%m-%d")
SV_PLUGIN_TIMESTAMP := $(shell date +"%Y-%m-%d")
SV_PLUGIN_MAJOR_VER_NO := $(shell date +"%y")
SV_PLUGIN_MINOR_VER_NO := $(shell date +"%m")
SV_PLUGIN_PATCH_VER_NO := $(shell date +"%d")

DIST_DIR_PLUGIN_TOP=package-release-plugin-purkinjenetwork

ifneq ($(CLUSTER),x64_linux)
  all: msiclean-plugin-purkinjenetwork msi-plugin-purkinjenetwork
else
  all: linux-distribution taritup
endif

#
#  Create msi installer for simvascular plugin-purkinjenetwork
#

msiclean-plugin-purkinjenetwork:
	-rm -f *~
	-rm -fR $(DIST_DIR_PLUGIN_TOP)
	-rm -f simvascular-plugin-purkinjenetwork.msi
	-rm -f tmp/simvascular-plugin-purkinjenetwork.wixobj
	-rm -f tmp/simvascular-plugin-purkinjenetwork.wxs
	-rm -f simvascular-plugin-purkinjenetwork.wixpdb
	-rm -f tmp/uuidgen.exe*
	-rm -f tmp/uuidgen.obj*

msi-plugin-purkinjenetwork:	msiclean-plugin-purkinjenetwork
	-mkdir -p tmp
ifeq ($(CXX_COMPILER_VERSION),mingw-gcc)
	x86_64-w64-mingw32-gcc -o tmp/uuidgen.exe windows_msi_helpers/uuidgen.c -lole32
else
	CL /Fetmp/uuidgen.exe /Fotmp/uuidgen.obj windows_msi_helpers/uuidgen.c ole32.lib
endif
	-rm -Rf $(DIST_DIR_PLUGIN_TOP)
	-mkdir -p $(DIST_DIR_PLUGIN_TOP)
	-date > $(DIST_DIR_PLUGIN_TOP)/release-date
	-chmod -R a+rwx $(DIST_DIR_PLUGIN_TOP)
	-echo "puts \"[file tail $(DIST_DIR)]\"" | tclsh > tmp/timestamp
	-cp -f tmp/timestamp $(DIST_DIR_PLUGIN_TOP)/
	-cp -f $(PURKINJENETWORK_PLUGIN_TOP)/Lib/$(LIB_BUILD_DIR)/*purkinjenetwork*.$(SOEXT)* $(DIST_DIR_PLUGIN_TOP)
	-echo "Create WXS file..."
	-tclsh windows_msi_helpers/makewxs-plugin-purkinjenetwork.tcl $(SV_VERSION) $(SV_PLATFORM) $(SV_TIMESTAMP) $(SV_PLUGIN_TIMESTAMP) $(DIST_DIR_PLUGIN_TOP) $(SV_MAJOR_VER_NO) $(SV_PLUGIN_MAJOR_VER_NO) $(SV_PLUGIN_MINOR_VER_NO) $(SV_PLUGIN_PATCH_VER_NO)
	-echo "Run candle..."
	-$(WIXUI_PATH)/candle.exe -ext WixBalExtension -ext WixUIExtension -ext WixUtilExtension -out tmp/simvascular-plugin-purkinjenetwork.wixobj tmp/simvascular-plugin-purkinjenetwork.wxs
	-echo "Run light..."
	-$(WIXUI_PATH)/light.exe -ext WixBalExtension -ext WixUIExtension -ext WixUtilExtension -out simvascular-plugin-purkinjenetwork.msi tmp/simvascular-plugin-purkinjenetwork.wixobj

veryclean: msiclean-plugin-purkinjenetwork
	-rm -f tar_files/*.tar
	-rm -f tar_files/*.tar.gz
	-rm -Rf $(DIST_DIR_TOP)
	-rm -Rf tmp/*

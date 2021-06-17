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

# need to set to build!
# SV_TOP: set environment variable "SV_TOP" to point to core SV build "BuildWithMake" directory!

# need to set before running executable at command line
# SV_PLUGIN_PATH: location for modules and shared objects (: separated linux, ; separated windows)
# SV_CUSTOM_PLUGINS: location for qt plugins (: separated linux, ; separated windows)

TOP=$(SV_TOP)
PURKINJENETWORK_PLUGIN_TOP=../../../../../BuildWithMake
TARGETLIB_TOP=$(PURKINJENETWORK_PLUGIN_TOP)
include $(TOP)/include.mk
include $(PURKINJENETWORK_PLUGIN_TOP)/include.purkinjenetwork.mk

CXXFLAGS = $(GLOBAL_CXXFLAGS) \
	   $(LOCAL_INCDIR) \
	   $(VTK_INCDIRS) \
	   $(TCLTK_INCDIR) \
           $(PYTHON_INCDIR) \
           $(QT_INCDIRS) $(QT_DEFS) \
	   $(ITK_INCDIRS) \
           $(MITK_INCDIRS) $(MITK_DEFS) \
           -Dorg_sv_gui_qt_purkinjenetwork_EXPORTS \
          -DUS_MODULE_NAME=sv4guiModulePurkinjenetwork

CXXFLAGS += -I../../Modules/PurkinjeNetwork

HDRS	= \
    sv4gui_PurkinjeNetworkCreate.h \
    sv4gui_PurkinjeNetworkCreateAction.h \
    sv4gui_PurkinjeNetworkEdit.h \
    sv4gui_LocalTableDelegate.h \
    sv4gui_PurkinjeNetworkLegacySaveAction.h \
    sv4gui_PurkinjeNetworkLoadSurfaceAction.h \
    sv4gui_PurkinjeNetworkLoadVolumeAction.h \
    sv4gui_PurkinjeNetworkPluginActivator.h \
    sv4gui_PurkinjeNetworkMeshContainer.h \
    sv4gui_PurkinjeNetworkMeshMapper.h \
    sv4gui_PurkinjeNetworkInteractor.h \
    sv4gui_PurkinjeNetwork1DMapper.h \
    sv4gui_PurkinjeNetwork1DContainer.h \
    sv4gui_PurkinjeNetworkModel.h

CXXSRCS	=  \
    sv4gui_PurkinjeNetworkCreate.cxx \
    sv4gui_PurkinjeNetworkCreateAction.cxx \
    sv4gui_PurkinjeNetworkEdit.cxx \
    sv4gui_VtkPurkinjeNetworkSphereWidget.cxx \
    sv4gui_LocalTableDelegate.cxx \
    sv4gui_PurkinjeNetworkLegacySaveAction.cxx \
    sv4gui_PurkinjeNetworkLoadSurfaceAction.cxx \
    sv4gui_PurkinjeNetworkLoadVolumeAction.cxx \
    sv4gui_PurkinjeNetworkPluginActivator.cxx \
    sv4gui_PurkinjeNetworkMeshContainer.cxx \
    sv4gui_PurkinjeNetworkMeshMapper.cxx \
    sv4gui_PurkinjeNetworkInteractor.cxx \
    sv4gui_PurkinjeNetwork1DMapper.cxx \
    sv4gui_PurkinjeNetwork1DContainer.cxx \
    sv4gui_PurkinjeNetworkModel.cxx

UIFILES = \
    sv4gui_PurkinjeNetworkCreate.ui \
    sv4gui_PurkinjeNetworkEdit.ui

QRCFILES =

QRCFILES += org_sv_gui_qt_purkinjenetwork_manifest.qrc \
            org_sv_gui_qt_purkinjenetwork_cached.qrc

RCFILES = \
    plugin.xml \
    resources/purkinje_network.png

CXXSRCS += $(addprefix moc_,$(HDRS:.h=.cxx))

CXXSRCS += $(addprefix qrc_,$(notdir $(QRCFILES:.qrc=.cxx)))

DLLHDRS =

DLLSRCS =

DLLLIBS += $(PYTHON_LIB) $(VTK_LIBS) $(ITK_LIBS) $(QT_LIBS) $(MITK_LIBS)

DLLLIBS +=  \
          $(SVLIBFLAG)$(SV_PLUGIN_DATAMANAGER_NAME)$(LIBLINKEXT) \
          $(SVLIBFLAG)$(SV_LIB_MODULE_COMMON_NAME)$(LIBLINKEXT) \
          $(SVLIBFLAG)$(SV_LIB_MODULE_SEGMENTATION_NAME)$(LIBLINKEXT) \
          $(SVLIBFLAG)$(SV_LIB_MODULE_PATH_NAME)$(LIBLINKEXT) \
          $(SVLIBFLAG)$(SV_LIB_MODULE_PROJECTMANAGEMENT_NAME)$(LIBLINKEXT) \
          $(SVLIBFLAG)$(SV_LIB_MODULE_MESH_NAME)$(LIBLINKEXT) \
          $(SVLIBFLAG)$(SV_LIB_MODULE_MODEL_NAME)$(LIBLINKEXT) \
          $(SVLIBFLAG)$(SV_LIB_MODULE_QTWIDGETS_NAME)$(LIBLINKEXT) \
          $(SVLIBFLAG)$(SV_LIB_ITK_LSET_NAME)$(LIBLINKEXT) \
          $(SVLIBFLAG)$(SV_LIB_REPOSITORY_NAME)$(LIBLINKEXT) \
          $(SVLIBFLAG)$(SV_LIB_GEOM_NAME)$(LIBLINKEXT) \
          $(SVLIBFLAG)$(SV_LIB_UTILS_NAME)$(LIBLINKEXT) \
          $(SVLIBFLAG)$(SV_LIB_GLOBALS_NAME)$(LIBLINKEXT) \
          $(SVLIBFLAG)$(SV_LIB_MODULE_PURKINJENETWORK_NAME)$(LIBLINKEXT) \
          $(SVLIBFLAG)$(SV_LIB_MODULE_PROJECTMANAGEMENT_NAME)$(LIBLINKEXT) \
          $(SVLIBFLAG)$(SV_LIB_POLYDATA_SOLID_NAME)$(LIBLINKEXT)
# $(SVLIBFLAG)$(SV_LIB_MODULE_PATH_NAME)$(LIBLINKEXT) \
# $(SVLIBFLAG)$(SV_LIB_MODULE_COMMON_NAME)$(LIBLINKEXT) \
# $(SVLIBFLAG)$(SV_LIB_MODULE_SEGMENTATION_NAME)$(LIBLINKEXT) \
# $(SVLIBFLAG)$(SV_LIB_MODULE_PURKINJENETWORK_NAME)$(LIBLINKEXT) \
# $(SVLIBFLAG)$(SV_LIB_MODULE_MODEL_NAME)$(LIBLINKEXT) \
# $(SVLIBFLAG)$(SV_LIB_MODULE_QTWIDGETS_NAME)$(LIBLINKEXT) \
# $(SVLIBFLAG)$(SV_LIB_MODULE_PATH_NAME)$(LIBLINKEXT) \
# $(SVLIBFLAG)$(SV_LIB_MODULE_COMMON_NAME)$(LIBLINKEXT) \
# $(SVLIBFLAG)$(SV_LIB_MODULE_MODEL_NAME)$(LIBLINKEXT) \
# $(SVLIBFLAG)$(SV_LIB_MODULE_QTWIDGETS_NAME)$(LIBLINKEXT) \
#


EXTRA_MOC_INCDIRS = $(MITK_PLUGIN_INCDIRS) -I$(SV_TOP)/../Code/Source/Include  -I../../../org.sv.gui.qt.datamanager

TARGET_LIB_NAME = $(SV_PLUGIN_PURKINJENETWORK_NAME)

PLUGIN_SYMBOLIC_NAME = org_sv_gui_qt_purkinjenetwork
PLUGIN_EXPORTS_NAME = org_sv_gui_qt_purkinjenetwork
PLUGIN_EXPORTS_PREFIX = SV_QT_
PLUGIN_NAME = PURKINJENETWORK

SV_COPY_DLL_TO_BIN_PLUGINS = 1

include $(TOP)/targetlib.mk

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

set(CPP_FILES
    sv4gui_PurkinjeNetworkCreate.cxx
    sv4gui_PurkinjeNetworkCreateAction.cxx
    sv4gui_PurkinjeNetworkEdit.cxx
    sv4gui_VtkPurkinjeNetworkSphereWidget.cxx
    sv4gui_LocalTableDelegate.cxx
    sv4gui_PurkinjeNetworkLegacySaveAction.cxx
    sv4gui_PurkinjeNetworkLoadSurfaceAction.cxx
    sv4gui_PurkinjeNetworkLoadVolumeAction.cxx
    sv4gui_PurkinjeNetworkPluginActivator.cxx
    sv4gui_PurkinjeNetworkMeshContainer.cxx
    sv4gui_PurkinjeNetworkMeshMapper.cxx
    sv4gui_PurkinjeNetworkInteractor.cxx
)

set(MOC_H_FILES
    sv4gui_PurkinjeNetworkCreate.h
    sv4gui_PurkinjeNetworkCreateAction.h
    sv4gui_PurkinjeNetworkEdit.h
    sv4gui_LocalTableDelegate.h
    sv4gui_PurkinjeNetworkLegacySaveAction.h
    sv4gui_PurkinjeNetworkLoadSurfaceAction.h
    sv4gui_PurkinjeNetworkLoadVolumeAction.h
    sv4gui_PurkinjeNetworkPluginActivator.h
    sv4gui_PurkinjeNetworkMeshContainer.h
    sv4gui_PurkinjeNetworkMeshMapper.h
    sv4gui_PurkinjeNetworkInteractor.h
)

set(UI_FILES
    sv4gui_PurkinjeNetworkCreate.ui
    sv4gui_PurkinjeNetworkEdit.ui
)

set(CACHED_RESOURCE_FILES
  plugin.xml
  resources/purkinje_network.png
)

set(QRC_FILES
)

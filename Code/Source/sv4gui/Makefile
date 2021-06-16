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

.PHONY: setup

shared-complete: setup pre-shared-build shared

setup:
  # we require a top level SV build directory
  # be set in an env variable and exist
  ifndef SV_TOP
    $(error SV_TOP not defined)
  endif
  TOP=$(SV_TOP)
  PURKINJENETWORK_PLUGIN_TOP=../../../BuildWithMake
  TARGETLIB_TOP=$(PURKINJENETWORK_PLUGIN_TOP)
  include $(TOP)/include.mk
  include $(PURKINJENETWORK_PLUGIN_TOP)/include.purkinjenetwork.mk

  SV4GUI_MODULE_DIRS = Modules/PurkinjeNetwork

pre-shared-build: create_exports_h us-init-module moc ui qrc create_cppmicroservices_file

shared:
	@for i in ${SV4GUI_MODULE_DIRS}; do ( \
	  cd $$i; \
	  $(MAKE) shared) ; done

create_exports_h:
	@for i in ${SV4GUI_MODULE_DIRS}; do ( \
	  cd $$i; \
	  $(MAKE) create_exports_h) ; done

moc:
	@for i in ${SV4GUI_MODULE_DIRS}; do ( \
	  cd $$i; \
	  $(MAKE) moc) ; done

ui:
	@for i in ${SV4GUI_MODULE_DIRS}; do ( \
	  cd $$i; \
	  $(MAKE) ui) ; done

qrc:
	@for i in ${SV4GUI_MODULE_DIRS}; do ( \
	  cd $$i; \
	  $(MAKE) qrc) ; done

us-init-module:
	@for i in ${SV4GUI_MODULE_DIRS}; do ( \
	  cd $$i; \
	  $(MAKE) us-init-module) ; done

create_cppmicroservices_file:
	@for i in ${SV4GUI_MODULE_DIRS}; do ( \
	  cd $$i; \
	  $(MAKE) create_cppmicroservices_file) ; done

create_exports_cv_h:

clean:
	for i in ${SV4GUI_MODULE_DIRS}; do ( \
	  cd $$i; \
	  $(MAKE) clean ) ; done

veryclean: clean
	@for i in ${SV4GUI_MODULE_DIRS}; do ( \
	  rm -f ../Include/Make/sv$$i\Exports.h; \
	  rm -f ../Include/Make/mitk$$i\Exports.h) ; done
	for i in ${SV4GUI_MODULE_DIRS}; do ( \
	  cd $$i; \
	  $(MAKE) veryclean ) ; done




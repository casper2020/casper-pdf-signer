#
# Copyright (c) 2011-2020 Cloudware S.A. All rights reserved.
#
# This file is part of casper-pdf-signer.
#
# casper-pdf-signer is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# casper-pdf-signer  is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU Affero General Public License
# along with casper-pdf-signer. If not, see <http://www.gnu.org/licenses/>.
#
THIS_DIR := $(abspath $(dir $(abspath $(lastword $(MAKEFILE_LIST)))))
ifeq (casper-pdf-signer, $(shell basename $(THIS_DIR)))
  PACKAGER_DIR := $(abspath $(THIS_DIR)/../casper-packager)
else
  PACKAGER_DIR := $(abspath $(THIS_DIR)/..)
endif

include $(PACKAGER_DIR)/common/c++/settings.mk

REL_NAME            := libcps
REL_VARIANT         ?= 0
REL_NAME            ?= casper-pdf-signer$(EXECUTABLE_SUFFIX)
REL_DATE            := $(shell date -u)
REL_HASH            := $(shell git rev-parse HEAD)
REL_BRANCH          := $(shell git rev-parse --abbrev-ref HEAD)
REL_BINARY_ABBR     := cps
REL_BINARY_NAME     := $(REL_NAME)

PROJECT_SRC_DIR     := $(ROOT_DIR)/casper-pdf-signer
EXECUTABLE_NAME     :=
EXECUTABLE_MAIN_SRC :=
LIBRARY_TYPE        := static
ICU_DEP_ON          ?= true
CURL_DEP_ON         ?= true
OPENSSL_DEP_ON      ?= true
LIBRARY_NAME        := libcps.a
VERSION             := $(shell cat $(PACKAGER_DIR)/casper-pdf-signer/version)
CHILD_CWD           := $(THIS_DIR)
CHILD_MAKEFILE      := $(firstword $(MAKEFILE_LIST))

VERSION_FILE 	    ?= $(PROJECT_SRC_DIR)/src/version.h
VERSION             ?= $(shell cat $(PACKAGER_DIR)/casper-pdf-signer$(EXECUTABLE_SUFFIX)/version)

############################
# COMMON VARIABLES
############################

include $(PROJECT_SRC_DIR)/common.mk

############################
# COMMON REQUIRED VARIABLES
############################

include $(PACKAGER_DIR)/common/c++/common.mk

all: lib

# version
version:
	@echo " $(LOG_COMPILING_PREFIX) - patching $(VERSION_FILE)"
	@cp -f $(PROJECT_SRC_DIR)/src/version.tpl.h $(VERSION_FILE)
	@sed -i.bak s#"@b.n.s@"#${EXECUTABLE_SUFFIX}#g $(VERSION_FILE)
	@sed -i.bak s#"x.x.x"#$(VERSION)#g $(VERSION_FILE)
	@sed -i.bak s#"n.n.n"#$(REL_NAME)#g $(VERSION_FILE)
	@sed -i.bak s#"v.v.v"#"$(REL_VARIANT)"#g $(VERSION_FILE)
	@sed -i.bak s#"a.b.b"#"$(REL_BINARY_ABBR)"#g $(VERSION_FILE)
	@sed -i.bak s#"c.t.n"#$(REL_BINARY_NAME)#g $(VERSION_FILE)
	@sed -i.bak s#"r.r.d"#"$(REL_DATE)"#g $(VERSION_FILE)
	@sed -i.bak s#"r.r.b"#"$(REL_BRANCH)"#g $(VERSION_FILE)
	@sed -i.bak s#"r.r.h"#"$(REL_HASH)"#g $(VERSION_FILE)
	@sed -i.bak s#"t.t.t"#$(TARGET)#g $(VERSION_FILE)
	@rm -f $(VERSION_FILE).bak

.SECONDARY:

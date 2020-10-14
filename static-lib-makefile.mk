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

############################
# COMMON VARIABLES
############################

include $(PROJECT_SRC_DIR)/common.mk

############################
# COMMON REQUIRED VARIABLES
############################

include $(PACKAGER_DIR)/common/c++/common.mk

all: lib

.SECONDARY:

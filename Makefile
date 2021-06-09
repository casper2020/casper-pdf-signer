#
# Copyright (c) 2011-2020 Cloudware S.A. All rights reserved.
#

THIS_DIR := $(abspath $(dir $(abspath $(lastword $(MAKEFILE_LIST)))))
ifeq (casper-pdf-signer, $(shell basename $(THIS_DIR)))
  PACKAGER_DIR := $(abspath $(THIS_DIR)/../casper-packager)
else
  PACKAGER_DIR := $(abspath $(THIS_DIR)/..)
endif

include $(PACKAGER_DIR)/common/c++/settings.mk

REL_VARIANT          ?= 0
REL_NAME             ?= casper-pdf-signer$(EXECUTABLE_SUFFIX)
REL_DATE             := $(shell date -u)
REL_HASH             := $(shell git rev-parse HEAD)
REL_BRANCH           := $(shell git rev-parse --abbrev-ref HEAD)

PROJECT_SRC_DIR     := $(ROOT_DIR)/casper-pdf-signer
EXECUTABLE_SUFFIX   ?=
EXECUTABLE_NAME     ?= casper-pdf-signer$(EXECUTABLE_SUFFIX)
EXECUTABLE_MAIN_SRC := src/main.cc
LIBRARY_NAME        :=
VERSION             := $(shell cat $(PACKAGER_DIR)/casper-pdf-signer/version)
CHILD_CWD           := $(THIS_DIR)
CHILD_MAKEFILE      := $(MAKEFILE_LIST)

###################
# THIS TOOL SOURCE
###################

# SOURCE

BISON_SRC :=
RAGEL_SRC :=

C_SRC  :=
CC_SRC := \
	$(CASPER_PDF_SIGNER_CC_SRC)

OBJECTS :=           \
	$(C_SRC:.c=.o)      \
	$(CC_SRC:.cc=.o)    \
	$(RAGEL_SRC:.rl=.o) \
	$(BISON_SRC:.yy=.o)

INCLUDE_DIRS := \
	-I $(PROJECT_SRC_DIR)/src

# common makefile
include $(PACKAGER_DIR)/common/c++/common.mk

# dependencies
CASPER_PDF_SIGNER_DEPENDENCIES := \
	casper-connectors-icu-on \
	casper-osal-dep-on \
	postgresql-dep-on \
	curl-dep-on \
	openssl-dep-on \
	libevent2-dep-on \
	jsoncpp-dep-on \
	zlib-dep-on \
	hiredis-dep-on \
	beanstalk-client-dep-on

ifeq (Darwin, $(PLATFORM))
  CASPER_PDF_SIGNER_DEPENDENCIES += macos-security-framework-dep-on
endif
set-dependencies: $(CASPER_PDF_SIGNER_DEPENDENCIES)

# this is a command line tool
all: exec

# version
version:
	@echo " $(LOG_COMPILING_PREFIX) - patching $(PROJECT_SRC_DIR)/src/version.h"
	@cp -f $(PROJECT_SRC_DIR)/src/version.tpl.h $(PROJECT_SRC_DIR)/src/version.h
	@sed -i.bak s#"@b.n.s@"#${EXECUTABLE_SUFFIX}#g $(PROJECT_SRC_DIR)/src/version.h
	@sed -i.bak s#"x.x.x"#$(VERSION)#g $(PROJECT_SRC_DIR)/src/version.h
	@sed -i.bak s#"n.n.n"#$(REL_NAME)#g $(PROJECT_SRC_DIR)/src/version.h
	@sed -i.bak s#"r.r.d"#"$(REL_DATE)"#g $(PROJECT_SRC_DIR)/src/version.h
	@sed -i.bak s#"r.r.b"#"$(REL_BRANCH)"#g $(PROJECT_SRC_DIR)/src/version.h
	@sed -i.bak s#"r.r.h"#"$(REL_HASH)"#g $(PROJECT_SRC_DIR)/src/version.h
	@sed -i.bak s#"v.v.v"#"$(REL_VARIANT)"#g $(PROJECT_SRC_DIR)/src/version.h
	@rm -f $(PROJECT_SRC_DIR)/src/version.h.bak

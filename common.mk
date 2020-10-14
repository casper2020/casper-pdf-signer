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
# casper-pdf-signer is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU Affero General Public License
# along with casper-pdf-signer.  If not, see <http://www.gnu.org/licenses/>.
#

############################
# COMMON VARIABLES
############################

CASPER_PDF_SIGNER_CC_SRC := \
	$(PROJECT_SRC_DIR)/src/casper/pdf/reader.cc            \
	$(PROJECT_SRC_DIR)/src/casper/pdf/writer.cc            \
	$(PROJECT_SRC_DIR)/src/casper/pdf/annotation.cc        \
	$(PROJECT_SRC_DIR)/src/casper/pdf/object.cc            \
	$(PROJECT_SRC_DIR)/src/casper/pdf/qpdf/reader.cc       \
	$(PROJECT_SRC_DIR)/src/casper/pdf/signer.cc            \
	$(PROJECT_SRC_DIR)/src/casper/pdf/podofo/writer.cc     \
	$(PROJECT_SRC_DIR)/src/casper/pdf/podofo/annotation.cc \
	$(PROJECT_SRC_DIR)/src/casper/openssl/certificate.cc   \
	$(PROJECT_SRC_DIR)/src/casper/openssl/p7.cc            \
	$(PROJECT_SRC_DIR)/src/casper/openssl/private_key.cc

INCLUDE_DIRS := \
    -I $(PROJECT_SRC_DIR)/src

CC_SRC := \
    $(CASPER_PDF_SIGNER_CC_SRC)

OBJECTS := \
  $(CC_SRC:.cc=.o)

set-dependencies: casper-connectors-dep-on qpdf-dep-on podofo-dep-on cppcodec-dep-on

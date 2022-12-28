/**
 * @file includes.h
 *
 * Copyright (c) 2011-2020 Cloudware S.A. All rights reserved.
 *
 * This file is part of casper-pdf-signer.
 *
 * casper-pdf-signer is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * casper-pdf-signer is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with casper. If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef CASPER_PDF_PODOFO_INCLUDES_H_
#define CASPER_PDF_PODOFO_INCLUDES_H_

#include "cc/pragmas.h"

CC_DIAGNOSTIC_PUSH()

CC_DIAGNOSTIC_IGNORED("-Wmissing-exception-spec")
CC_DIAGNOSTIC_IGNORED("-Wdocumentation")
CC_DIAGNOSTIC_IGNORED("-Wdeprecated-declarations")
CC_DIAGNOSTIC_IGNORED("-Wc++98-compat-extra-semi")
CC_DIAGNOSTIC_IGNORED("-Wextra-semi")
CC_DIAGNOSTIC_IGNORED("-Wsign-conversion")
CC_DIAGNOSTIC_IGNORED("-Wshadow-field")

#include <podofo/podofo.h>

CC_DIAGNOSTIC_POP()

#endif // CASPER_PDF_PODOFO_INCLUDES_H_

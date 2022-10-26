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
#ifndef CASPER_PDF_QPDF_INCLUDES_H_
#define CASPER_PDF_QPDF_INCLUDES_H_

#pragma GCC diagnostic push
#ifdef __APPLE__
 #pragma GCC diagnostic ignored "-Wmissing-exception-spec"
 #pragma GCC diagnostic ignored "-Wdocumentation"
 #pragma GCC diagnostic ignored "-Wdeprecated-declarations"
 #pragma GCC diagnostic ignored "-Wc++98-compat-extra-semi"
 #pragma GCC diagnostic ignored "-Wextra-semi"
 #pragma GCC diagnostic ignored "-Wcomma"
 #pragma GCC diagnostic ignored "-Wshadow-all"
#endif
#include <qpdf/QPDF.hh>
#include <qpdf/QPDFPageDocumentHelper.hh>
#include <qpdf/QPDFFormFieldObjectHelper.hh>
#include <qpdf/QPDFAcroFormDocumentHelper.hh>
#pragma GCC diagnostic pop

#endif // CASPER_PDF_QPDF_INCLUDES_H_

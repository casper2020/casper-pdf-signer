/**
 * @file annotation.cc
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

#include "casper/pdf/podofo/annotation.h"

#include "cc/exception.h"

// MARK: - SignatureAnnotation

/**
 * @brief Partial copy constructor.
 *
 * @param a_annotation Original annotation
 */
casper::pdf::podofo::SignatureAnnotation::SignatureAnnotation (const ::casper::pdf::SignatureAnnotation& a_annotation)
    : casper::pdf::SignatureAnnotation(a_annotation)
{
    /* empty */
}

/**
 * @brief Destructor.
 */
casper::pdf::podofo::SignatureAnnotation::~SignatureAnnotation ()
{
    /* empty */
}

// MARK: -

/**
 * @brief Draw a signature annotation.
 *
 * @param a_annotation Annotation to draw.
 * @param a_rect       Annotation bbox.
 * @param a_document   Document where annotation is placed at.
 * @param a_field      Signature field.
 */
void casper::pdf::podofo::SignatureAnnotation::Draw (const ::PoDoFo::PdfAnnotation& a_annotation, const ::PoDoFo::PdfRect& a_rect,
                                                     ::PoDoFo::PdfDocument& a_document, ::PoDoFo::PdfSignatureField& a_field) const
{
    // TODO: 
}


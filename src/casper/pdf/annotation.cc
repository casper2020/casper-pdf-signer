/**
 * @file annotation.h
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

#include "casper/pdf/annotation.h"

#include "cc/exception.h"

// MARK: - Annotation

/**
 * @brief Default constructor.
 *
 * @param a_name Annotation name.
 */
casper::pdf::Annotation::Annotation (const std::string& a_name)
 : casper::pdf::Object(a_name)
{    
    rect_ = { 0, 0, 0, 0 };
    page_ = 1;
}

/**
 * @brief Copy constructor.
 *
 * @param a_annotation Object to copy from.
 */
casper::pdf::Annotation::Annotation (const casper::pdf::Annotation& a_annotation)
: casper::pdf::Object(a_annotation)
{
    rect_ = a_annotation.rect_;
    page_ = a_annotation.page_;
}

/**
 * @brief Destructor.
 */
casper::pdf::Annotation::~Annotation ()
{
    /* empty */
}

// MARK: - SignatureAnnotation

/**
 * @brief Default constructor.
 *
 * @param a_name Annotation name.
 */
casper::pdf::SignatureAnnotation::SignatureAnnotation (const std::string& a_name)
 : casper::pdf::Annotation(a_name)
{
    byte_range_ = { 0, 0, 0, 0};
}

/**
 * @brief Copy constructor.
 *
 * @param a_annotation Object to copy from.
 */
casper::pdf::SignatureAnnotation::SignatureAnnotation (const casper::pdf::SignatureAnnotation& a_annotation)
: casper::pdf::Annotation(a_annotation)
{
    info_       = a_annotation.info_;
    byte_range_ = a_annotation.byte_range_;
    visible_    = true;
}

/**
 * @brief Destructor.
 */
casper::pdf::SignatureAnnotation::~SignatureAnnotation ()
{
    /* empty */
}

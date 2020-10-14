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

#include "casper/pdf/object.h"

#include "cc/exception.h"

#include <string.h> // strcmp

/**
 * @brief Default constructor.
 *
 * @param a_name Annotation name.
 */
casper::pdf::Object::Object (const std::string& a_name)
 : name_(a_name)
{
    /* empty */
}

/**
 * @brief Copy constructor.
 */
casper::pdf::Object::Object (const casper::pdf::Object& a_object)
: name_(a_object.name_)
{
    /* empty */
}

/**
 * @brief Destructor.
 */
casper::pdf::Object::~Object ()
{
    /* empty */
}

// MARK: -

/**
 * @brief Convert mm or inch value to points.
 *
 * @param a_value Value to convert.
 * @param a_units Currently 'mm' or 'inch' are supported.
 */
double casper::pdf::Object::ToPoints (double a_value, const char* const a_units) const
{
    if ( 0 == strcmp(a_units, "mm") ) {
        return ( 72.0 * a_value / 25.4 );
    } else if ( 0 == strcmp(a_units, "inch") ) {
        return ( 72.0 * a_value );
    } else {
        throw cc::Exception("Don't know how to convert %s units to points!", a_units);
    }
}

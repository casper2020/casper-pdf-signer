/**
 * @file writer.h
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

#include "casper/pdf/writer.h"

// MARK: -

/**
 * @brief Default constructor.
 *
 * @param a_name Writer name.
 */
casper::pdf::Writer::Writer (const std::string& a_name)
 : name_(a_name)
{
    /* empty */
}

/**
 * @brief Destructor.
 */
casper::pdf::Writer::~Writer ()
{
    /* empty */
}

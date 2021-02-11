/**
 * @file types.h
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
#ifndef CASPER_PDF_TYPES_H_
#define CASPER_PDF_TYPES_H_

#include "casper/openssl/certificate.h"

#include <string>

namespace casper
{

    namespace pdf
    {

        typedef struct {
            size_t before_start_;
            size_t before_size_;
            size_t after_start_;
            size_t after_size_;
        } ByteRange;

        typedef struct {
            std::string oid_;
            std::string author_;
            std::string reason_;
            std::string certified_by_;
            std::string date_time_;
            std::string utc_date_time_;
            size_t      size_in_bytes_;
        } SignatureInfo;

        typedef struct {
            std::string digest_;       //!< PDF document SHA256 hash.
            std::string signing_time_; //!< When signing was done.
            std::string auth_attr_;    //!< B64 encoded signing attributes.
            std::string enc_digest_;   //!< B64 encoded signed signing attributes ( auth_attr_ ).
        } SigningInfo;
            
        typedef ::casper::openssl::Certificate Certificate;

        typedef struct {
           Certificate        signing_; //!< Signing certificate.
           Certificate::Chain chain_;   //!< The other certificates in chain ( intermediate and root ) .
        } Certificates;

    } // end of namepace 'pdf'

} // end of namespace 'casper'

#endif // CASPER_PDF_TYPES_H_

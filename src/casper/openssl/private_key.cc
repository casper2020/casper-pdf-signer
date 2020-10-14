/**
 * @file private_key.cc
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

#include "casper/openssl/private_key.h"

#include <string.h> // strlen

/**
 * @brief Defaiult 
 */
casper::openssl::PrivateKey::PrivateKey (const std::string& a_uri, const std::string& a_password)
    : uri_(a_uri), password_(a_password)
{
    /* empty */
}

/**
 * @brief Copy constructor.
 *
 * @param a_private_key Object to copy from.
 */
casper::openssl::PrivateKey::PrivateKey (const casper::openssl::PrivateKey& a_private_key)
    : uri_(a_private_key.uri_), password_(a_private_key.password_)
{
    /* empty */
}

/**
 * @brief Destructor.
 */
casper::openssl::PrivateKey::~PrivateKey ()
{
    /* empty */
}

/**
 * @brief Password callback.
 *
 * @param a_buffer    Where to write password to.
 * @param a_size      Buffer size.
 * param a_rw_flag   Ignored.
 * @param a_user_data Password it self.
 *
 * @return Copied password length.
 */
int casper::openssl::PrivateKey::PEMPasswordCallback (char* a_buffer, int a_size, int /* a_rw_flag */, void* a_user_data)
{
    const char* const pw     = (const char* const)a_user_data;
    const size_t      pw_len = strlen(pw);
    const size_t      cp_len = pw_len <= (size_t)a_size ? pw_len : (size_t)a_size;
    
    memcpy(a_buffer, pw, cp_len);
    
    return static_cast<int>(cp_len);
}

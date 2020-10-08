/**
 * @file private_key.h
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
#ifndef CASPER_OPENSSL_PRIVATE_KEY_H_
#define CASPER_OPENSSL_PRIVATE_KEY_H_

#include "cc/non-movable.h"

#include <string>

namespace casper
{

    namespace openssl
    {
    
        class PrivateKey final : public ::cc::NonMovable
        {
            
        public: // Const Data
            
            const std::string uri_;
            const std::string password_;

        public: // Constructor(s) / Destructor
            
            PrivateKey (const std::string& a_uri, const std::string& a_password);
            PrivateKey (const PrivateKey& a_private_key);
            
            virtual ~PrivateKey();
            
        public: // Operator(s) Overload
            
            inline PrivateKey& operator = (const PrivateKey& a_certificate) = delete;
            
        public: // Static Method(s) / Function(s)
            
            static int PEMPasswordCallback (char* a_buffer, int a_size, int /* a_rw_flag */, void* a_user_data);

        }; // end of class 'PrivateKey'
    
    } // end of namespace 'openssl'
    
} // end of namespace 'casper'

#endif // CASPER_OPENSSL_PRIVATE_KEY_H_

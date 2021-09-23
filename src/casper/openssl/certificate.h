/**
 * @file certificate.h
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
#ifndef CASPER_OPENSSL_CERTIFICATE_H_
#define CASPER_OPENSSL_CERTIFICATE_H_

#include "cc/non-movable.h"
#include "cc/exception.h"
#include "cc/types.h"

#include <inttypes.h> // uint8_t
#include <vector>
#include <string>

#include <openssl/x509.h>

namespace casper
{

    namespace openssl
    {
    
        class Certificate final : public ::cc::NonMovable
        {
            
        public: // Data Type(s)
            
            enum Type : uint8_t {
                NotSet = 0,
                Issuer,
                Intermediate,
                Entity
            };

            enum Origin : uint8_t {
                File,   //!< BASE64 encoded file.
                Memory  //!< BASE64 in memory file
            };
                            
            enum Format : uint8_t {
                DER
            };
            
            typedef std::vector<Certificate> Chain;
            
        private: // Const Data
            
            const Type   type_;   //!< One of \link Type \link.
            const Origin origin_; //!< One of \link Origin \link.
            const Format format_; //!< One of \link Format \link.
            
        private: // Data
                        
            std::string data_;   //!< File URI or Data, based on provided origin.

        public: // Constructor(s) / Destructor
            
            Certificate () = delete;
            Certificate (const Type a_type, const Origin a_origin, const Format a_format);
            Certificate (const Type a_type, const Origin a_origin, const Format a_format, const std::string& a_data);
            Certificate (const Certificate& a_certificate);

            virtual ~Certificate ();
            
        public: // Static Method(s) / Function(s)
            
            static size_t Load   (const Certificate& a_certificate, X509** o_x509);
            static void   Unload (X509** o_x509);
            
            static size_t Load   (const Certificate::Chain& a_chain, std::vector<X509*>& o_chain);
            static void   Unload (std::vector<X509*>& o_chain);
            
        public: // Operator(s) Overload
            
            inline Certificate& operator = (const Certificate& a_certificate) = delete;
            
        public: // Inline Method(s) / Function(s)
            
            void               Set    (const std::string& a_data);
            const std::string& data   () const;
            const Type&        type   () const;
            const Origin&      origin () const;
            const Format&      format () const;
            
        public: // Static Method(s) / Function(s)
            
            static const char* const Type2CString   (const Type& a_type);
            static const char* const Format2CString (const Format& a_format);
            
        }; // end of class 'Certificate'
    
        /**
         * @brief Set PEM data.
         */
        inline void Certificate::Set (const std::string& a_data)
        {
            data_ = a_data;
        }
    
        /**
         * @brief R/O access to PEM data.
         */
        inline const std::string& Certificate::data () const
        {
            return data_;
        }
    
        /**
         * @brief R/O access to type.
         */
        inline const Certificate::Type& Certificate::type () const
        {
            return type_;
        }
        
        /**
         * @brief R/O access to origin.
         */
        inline const Certificate::Origin& Certificate::origin () const
        {
            return origin_;
        }
        
        /**
         * @brief R/O access to format.
         */
        inline const Certificate::Format& Certificate::format () const
        {
            return format_;
        }
        
        /**
         * @brief Translate a \link Certificate::Type \link to a C string.
         *
         * @param a_type One of \link Certificate::Type \link.
         *
         * @return Certificate type as C string.
         */
        inline const char* const Certificate::Type2CString (const Certificate::Type& a_type)
        {
            switch(a_type) {
                case Certificate::Type::Issuer:
                    return "Issuer";
                case Certificate::Type::Intermediate:
                    return "Intermediate";
                case Certificate::Type::Entity:
                    return "Entity";
                default:
                    throw ::cc::Exception("Don't know how to translate certificate type " UINT8_FMT " to string!", static_cast<uint8_t>(a_type));
            }
        }
                
        /**
         * @brief Translate a \link Certificate::Format \link to a C string.
         *
         * @param a_format One of \link Certificate::Format \link.
         *
         * @return Certificate type as C string.
         */
        inline const char* const Certificate::Format2CString (const Certificate::Format& a_format)
        {
            switch(a_format) {
                case Certificate::Format::DER:
                    return "DER";
                default:
                    throw ::cc::Exception("Don't know how to translate certificate format " UINT8_FMT " to string!", static_cast<uint8_t>(a_format));
            }
        }
        
    } // end of namespace 'openssl'
    
} // end of namespace 'casper'

#endif // CASPER_OPENSSL_CERTIFICATE_H_

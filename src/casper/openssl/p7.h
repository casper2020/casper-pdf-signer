/**
 * @file p7.h
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
#ifndef CASPER_OPENSSL_P7_H_
#define CASPER_OPENSSL_P7_H_

#include "cc/non-copyable.h"
#include "cc/non-movable.h"

#include <string>
#include <vector>
#include <functional> // std::function

#include <openssl/pkcs7.h>
#include <openssl/x509.h>

#include "casper/openssl/certificate.h"
#include "casper/openssl/private_key.h"

namespace casper
{

    namespace openssl
    {
    
        class P7 final : public ::cc::NonCopyable, public ::cc::NonMovable
        {

        public: // Constructor(s) / Destructor
            
            P7 ();
            virtual ~P7 ();
        
        private: // Static Const Data
            
            static const char* const sk_p7_err_msg_unable_to_create_new_object_;
            static const char* const sk_p7_err_msg_unable_to_set_type_;
            static const char* const sk_p7_err_msg_unable_to_add_attribute_;
            static const char* const sk_p7_err_msg_unable_to_load_private_key_;
            static const char* const sk_p7_err_msg_unable_to_add_certificate_;
            static const char* const sk_p7_err_msg_unable_to_add_signer_;
            static const char* const sk_p7_err_msg_unable_to_set_content_;
            static const char* const sk_p7_err_msg_unable_to_set_digest_;
            static const char* const sk_p7_err_msg_unable_to_set_si_field_;
            static const char* const sk_p7_err_msg_unable_to_to_sign_si_;
            static const char* const sk_p7_err_msg_unable_to_encoded_to_der_;
            static const char* const sk_p7_err_msg_unable_to_export_to_pem_;
            static const char* const sk_p7_err_msg_unable_to_open_file_with_;
            static const char* const sk_p7_err_msg_unable_to_close_file_with_;
            static const char* const sk_p7_err_msg_unable_to_load_;
            static const char* const sk_p7_exp_msg_unable_to_load_;
            static const char* const sk_p7_err_msg_signature_validation_failed_;
            
        public: // Static Method(s) / Function(s)
            
            static void GetSigningTime             (std::string& o_value);
            static void CalculateSigningAttributes (const std::string& a_digest, const Certificate* a_certificate,
                                                    std::string& o_signing_time, std::string& o_auth_attr);
            
        public: // Static Method(s) / Function(s)
            
            static void Sign (const Certificate& a_certificate, const Certificate::Chain& a_chain, const PrivateKey& a_key,
                              const std::string& a_digest, const std::string& a_signing_time,
                              std::function<void(const unsigned char*, const size_t&)> a_callback,
                              std::string* o_enc_digest = nullptr);

            static void Sign (const Certificate& a_certificate, const Certificate::Chain& a_chain,
                              const std::string& a_digest, const std::string& a_enc_digest, const std::string& a_signing_time,
                              std::function<void(const unsigned char*, const size_t&)> a_callback);
            
            static void Export (const PKCS7* a_pkcs7, const std::string& a_uri);
            static void Export (const unsigned char* a_pkcs7, const size_t a_length, const std::string& a_uri);

        private: // Static Method(s) / Function(s)
            
            static size_t DecodeBase64 (const std::string& a_value, unsigned char** o_buffer);

        }; // end of class 'P7'
        
    } // end of namespace 'openssl'

} // end of namespace 'casper'

#endif // CASPER_OPENSSL_P7_H_

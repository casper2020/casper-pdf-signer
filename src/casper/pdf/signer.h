/**
 * @file signer.h
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
#ifndef CASPER_PDF_SIGNER_H_
#define CASPER_PDF_SIGNER_H_

#include "cc/non-copyable.h"
#include "cc/non-movable.h"

#include <string>

#include "casper/openssl/p7.h"

#include "casper/pdf/annotation.h"

namespace casper
{

    namespace pdf
    {
    
            class Signer final : public ::cc::NonCopyable, public ::cc::NonMovable
            {
                
            public: // Data Type(s)
                
                typedef openssl::Certificate        Certificate;
                typedef openssl::PrivateKey         PrivateKey;
                typedef ::casper::pdf::ByteRange    ByteRange;
                typedef ::casper::pdf::SigningInfo  SigningInfo;
                typedef ::casper::pdf::Certificates Certificates;
                
            public: // Static Data
                
                static const char* const sk_name_;

                static const char* const sk_field_err_msg_invalid_or_missing_;
                
                static const char* const sk_file_err_msg_fmt_unable_to_open_file_with_;
                static const char* const sk_file_err_msg_fmt_unable_to_close_file_with_;
                static const char* const sk_file_err_msg_fmt_unable_to_seek_to_end_of_file_;
                static const char* const sk_file_err_msg_fmt_unable_to_seek_to_position_of_file_;
                static const char* const sk_file_err_msg_fmt_read_error_;
                static const char* const sk_file_err_msg_fmt_read_mismatch_;
                static const char* const sk_file_err_msg_fmt_write_error_;
                static const char* const sk_file_err_msg_fmt_write_mismatch_;
                
                static const char* const sk_pdf_contents_not_enough_bytes_to_read_;
                static const char* const sk_pdf_byte_range_not_found_;
                
                static const char* const sk_pkcs7_err_msg_fmt_unable_to_write_data_not_enough_space_;
                static const char* const sk_pkcs7_err_msg_fmt_unable_to_write_data_failed_seek_to_start_of_contents_;
                static const char* const sk_pkcs7_err_msg_fmt_unable_to_write_data_failed_seek_to_end_of_contents_;
                static const char* const sk_pkcs7_err_msg_fmt_write_error_;
                static const char* const sk_pkcs7_err_msg_fmt_write_mismatch_;
                
            public: // Const Data
                
                const std::string signer_name_;
                const std::string signature_name_;
                
            private: // Data
                
                size_t          buffer_size_;
                unsigned char*  buffer_;

            public: // Constructor(s) / Destructor
                
                Signer () = delete;
                Signer (const char* const a_signer_name, const char* const a_signature_name = "casper-pdf-signature");
                virtual ~Signer ();
                
            public: // Placeholder - Method(s) / Function(s)
                
                void GetSigningTime (std::string& o_time);
                
                void SetPlaceholder (const std::string& a_in, pdf::SignatureAnnotation& a_annotation,
                                     std::string& o_out);

                void SetPlaceholder (const std::string& a_in, const std::string& a_out, pdf::SignatureAnnotation& a_annotation);

            public: // Signing Attributes - Method(s) / Function(s)
                
                void CalculateSigningAttributes (const std::string& a_uri, const Signer::ByteRange& a_byte_range,
                                                 Signer::SigningInfo& a_info);
                
                void CalculateSigningAttributes (const std::string& a_uri, const Signer::ByteRange& a_byte_range,
                                                 const Signer::Certificate& a_certificate,
                                                 Signer::SigningInfo& a_info);
                
                void CalculateSigningAttributes (const Signer::Certificate& a_certificate,
                                                 Signer::SigningInfo& a_info);
                
                void SignSigningAttributes (const Signer::PrivateKey a_key, Signer::SigningInfo& a_info);

            public: // Sign Method(s) / Function(s)
                
                void Sign (const std::string& a_uri,
                           const Signer::ByteRange& a_range, const std::string& a_digest,
                           const Signer::Certificates& a_certificates, const Signer::PrivateKey& a_key,
                           Signer::SigningInfo& o_info);

                void Sign (const std::string& a_uri,
                           const Signer::ByteRange& a_range, const Signer::SigningInfo& a_info,
                           const Signer::Certificates& a_certificates);

            public: // Method(s) / Function(s)
                
                void ZeroOut (const std::string& a_uri, const Signer::ByteRange& a_byte_range);
                                
            public: // Data Extraction - Method(s) / Function(s)
                
                void GetByteRange (const std::string& a_uri, const ssize_t a_page, Signer::ByteRange& o_range);
                void Export       (const std::string& a_uri, const Signer::ByteRange& a_range, const std::string& o_uri);

            private: // Method(s) / Function(s)
                
                void Write (const std::string& a_uri, const Signer::ByteRange& a_byte_range,
                            const unsigned char* a_bytes, const size_t a_size);
                                
                void ZeroOut (FILE* a_fp, const Signer::ByteRange& a_byte_range);

                void ZeroOut (FILE* a_fp, const size_t& a_size);
                
                void CalculateDigest (const std::string& a_uri, const Signer::ByteRange& a_byte_range, std::string& o_digest);
                
            public: // Static Method(s) / Function(s)
                
                static void Setup ();

            }; // end of class 'Signer'
    
    } // end of namespace 'pdf'

} // end of namespace 'casper'

#endif // CASPER_PDF_SIGNER_H_

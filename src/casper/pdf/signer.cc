/**
 * @file signer.cc
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

#include "casper/pdf/signer.h"

#include "cc/exception.h"

#include "cc/macros.h"

#include "cc/b64.h"
#include "cc/crypto/rsa.h"
#include "cc/hash/sha256.h"
#include "cc/types.h"
#include "cc/fs/file.h"

#include "casper/pdf/qpdf/reader.h"

#include "casper/pdf/podofo/writer.h"

// MARK: - STATIC CONST DATA

const char* const casper::pdf::Signer::sk_name_                                                                    = "casper-pdf-signature";
// FIELDS
const char* const casper::pdf::Signer::sk_field_err_msg_invalid_or_missing_                                        = "Invalid or missing '%s' value!";
// FILE
const char* const casper::pdf::Signer::sk_file_err_msg_fmt_unable_to_open_file_with_                               = "Unable to open file open '%s': %s !";
const char* const casper::pdf::Signer::sk_file_err_msg_fmt_unable_to_close_file_with_                              = "Unable to close file open '%s': %s !";
const char* const casper::pdf::Signer::sk_file_err_msg_fmt_unable_to_seek_to_end_of_file_                          = "Unable to seek to end of file: %s !";
const char* const casper::pdf::Signer::sk_file_err_msg_fmt_unable_to_seek_to_position_of_file_                     = "Unable to seek to file postion " SIZET_FMT ": %s !";
const char* const casper::pdf::Signer::sk_file_err_msg_fmt_read_error_                                             = "Unable to read data from file - %s!";
const char* const casper::pdf::Signer::sk_file_err_msg_fmt_read_mismatch_                                          = "Unable to read data from file - bytes read size mismatch - read " SIZET_FMT ", expecting " SIZET_FMT "!";
const char* const casper::pdf::Signer::sk_file_err_msg_fmt_write_error_                                            = "Unable to write data to file: %s!";
const char* const casper::pdf::Signer::sk_file_err_msg_fmt_write_mismatch_                                         = "Unable to write data to file: bytes written differs - wrote " SIZET_FMT ", expecting " SIZET_FMT "!";
// PDF
const char* const casper::pdf::Signer::sk_pdf_contents_not_enough_bytes_to_read_                                   = "Invalid PDF: not enough bytes to read '/Contents' !";
const char* const casper::pdf::Signer::sk_pdf_byte_range_not_found_                                                = "Invalid PDF: unable to find '/Sig/ByteRange' for annotation named '%s'!";
// PKCS7
const char* const casper::pdf::Signer::sk_pkcs7_err_msg_fmt_unable_to_write_data_not_enough_space_                 = "Unable to write PCKS7 data: not enough space!";
const char* const casper::pdf::Signer::sk_pkcs7_err_msg_fmt_unable_to_write_data_failed_seek_to_start_of_contents_ = "Unable to write PCKS7 data: seek to start of /Contents failed: %s!";
const char* const casper::pdf::Signer::sk_pkcs7_err_msg_fmt_unable_to_write_data_failed_seek_to_end_of_contents_   = "Unable to write PCKS7 data: seek to end of /Contents failed: %s!";
const char* const casper::pdf::Signer::sk_pkcs7_err_msg_fmt_write_error_                                           = "Unable to write PCKS7 data: %s!";
const char* const casper::pdf::Signer::sk_pkcs7_err_msg_fmt_write_mismatch_                                        = "Unable to write PCKS7 data: bytes written differs - wrote " SIZET_FMT ", expecting " SIZET_FMT "!";

// MARK: - Constructor(s) / Destructor

/**
 * @brief Default constructor.
 *
 * @param a_signer_name    Signer name.
 * @param a_signature_name Signature Name
 */
casper::pdf::Signer::Signer (const char* const a_signer_name, const char* const a_signature_name)
 : signer_name_(a_signer_name), signature_name_(a_signature_name)
{
    buffer_size_ = 1024;
    buffer_      = new unsigned char [buffer_size_];    
}

/**
 * @brief Destructor.
 */
casper::pdf::Signer::~Signer ()
{
    delete [] buffer_;
}

/**
 * @brief Get current time as the signing time.
 *
 * @param o_out Signing time YYMMDDHHMMSSZ - X509.
 */
void casper::pdf::Signer::GetSigningTime (std::string& o_out)
{
    casper::openssl::P7::GetSigningTime(o_out);
}

// MARK: - [PUBLIC] - Placeholder

/**
 * @brief Set a signature placeholder in a PDF document by creating a copy and keeping original intact
 *       ( because PDF will be invalid until is signed ).
 *
 * @param a_in  PDF local URI.
 * @param a_annotation Prefilled signature annotation, /link ByteRange /link will be set here.
 * @param o_out PDF local URI with placeholder.
 */
void casper::pdf::Signer::SetPlaceholder (const std::string& a_in, pdf::SignatureAnnotation& a_annotation,
                                          std::string& o_out)
{
    std::string name, path, ext;
    
    cc::fs::File::Path(a_in, path);
    cc::fs::File::Name(a_in, name);
    if ( 0 != name.length() ) {
        const char* const ptr = strrchr(name.c_str(), '.');
        if ( nullptr != ptr ) {
            ext  = ptr + sizeof(char);
            name = std::string(name.c_str(), 0, name.length() - strlen(ptr));
        }
    }
    cc::fs::File::Unique(path, name, ext, o_out);
    
    casper::pdf::podofo::Writer writer(signer_name_);

    // ... append placeholder ...
    writer.Open(a_in, o_out);
    writer.Append(a_annotation);
    writer.Close();

    // ... get byte range ..
    writer.GetByteRange(o_out, a_annotation);
        
    // ... zero-out /Contents ....
    ZeroOut(o_out, a_annotation.byte_range());
}

/**
 * @brief Set a signature placeholder in a PDF document by creating a copy and keeping original intact
 *       ( because PDF will be invalid until is signed ).
 *
 * @param a_in  PDF local URI.
 * @param a_out PDF local URI with placeholder.
 * @param a_annotation Prefilled signature annotation, /link ByteRange /link will be set here.
 */
void casper::pdf::Signer::SetPlaceholder (const std::string& a_in, const std::string& a_out, pdf::SignatureAnnotation& a_annotation)
{
    casper::pdf::podofo::Writer writer(signer_name_);

    // ... append placeholder ...
    writer.Open(a_in, a_out);
    writer.Append(a_annotation);
    writer.Close();

    // ... get byte range ..
    writer.GetByteRange(a_out, a_annotation);

    // ... zero-out /Contents ....
    ZeroOut(a_out, a_annotation.byte_range());
}

// MARK: - [PUBLIC] - Signing Attributes Calculation

/**
 * @brief Calculate 'signing attributes' in a PDF document.
 *
 * @param a_uri    PDF local URI.
 * @param a_range  See \link ByteRange \link.
 * @param a_info        Calculate and set some of the \link SigningInfo \link fields.
 *                      ( 'digest_', 'signing_time_' and 'auth_attr_' will be calculated and set here ).
 */
void casper::pdf::Signer::CalculateSigningAttributes (const std::string& a_uri, const Signer::ByteRange& a_range,
                                                      casper::pdf::Signer::SigningInfo& a_info)
{
    // ... document digest calculation ...
    // ( 'digest_' will be calculated here )
    CalculateDigest(a_uri, a_range, a_info.digest_);

    // ... calculate unsigned 'signing attributes' but do not sign them ...
    // ( 'signing_time_' and 'auth_attr_' will be calculated here )
    casper::openssl::P7::CalculateSigningAttributes(a_info.digest_, nullptr, a_info.signing_time_, a_info.auth_attr_);
}

/**
 * @brief Calculate 'signing attributes' in a PDF document.
 *
 * @param a_uri         PDF local URI.
 * @param a_range       See \link ByteRange \link.
 * @param a_certificate Signing certificate.
 * @param a_info        Calculate and set some of the \link SigningInfo \link fields.
 *                      ( 'digest_', 'signing_time_' and 'auth_attr_' will be calculated and set here ).
 */
void casper::pdf::Signer::CalculateSigningAttributes (const std::string& a_uri, const Signer::ByteRange& a_range,
                                                      const casper::pdf::Signer::Certificate& a_certificate,
                                                      casper::pdf::Signer::SigningInfo& a_info)
{
    // ... document digest calculation ...
    // ( 'digest_' will be calculated here )
    CalculateDigest(a_uri, a_range, a_info.digest_);

    // ... calculate unsigned 'signing attributes' but do not sign them ...
    // ( 'signing_time_' and 'auth_attr_' will be calculated here )
    casper::openssl::P7::CalculateSigningAttributes(a_info.digest_, &a_certificate, a_info.signing_time_, a_info.auth_attr_);
}

/**
 * @brief Calculate 'signing attributes' in a PDF document.
 *
 * @param a_certificate Signing certificate.
 * @param a_info        Calculate and set some of the \link SigningInfo \link fields.
 *                      ( 'signing_time_' and 'auth_attr_' will be calculated and set here ).
 */
void casper::pdf::Signer::CalculateSigningAttributes (const casper::pdf::Signer::Certificate& a_certificate,
                                                      casper::pdf::Signer::SigningInfo& a_info)
{
    // ... a_info.digest_ - must be already calculated ...
    if ( 0 == a_info.digest_.length() ) {
        throw cc::Exception(sk_field_err_msg_invalid_or_missing_, "SigningInfo.digest_");
    }

    // ... calculate unsigned 'signing attributes' but do not sign them ...
    // ( 'signing_time_' and 'auth_attr_' will be calculated here )
    casper::openssl::P7::CalculateSigningAttributes(a_info.digest_, &a_certificate, a_info.signing_time_, a_info.auth_attr_);
}

/**
 * @brief Signed a previous calculated PKCS7 signing attributes.
 *
 * @param a_key  Private key info.
 * @param a_info Calculate and set some of the \link SigningInfo \link fields.
 *               ( 'enc_digest_' will be calculated and set here ).
 */
void casper::pdf::Signer::SignSigningAttributes (const Signer::PrivateKey a_key, casper::pdf::Signer::SigningInfo& a_info)
{
    // ... a_info.auth_attr_ - must be already calculated ...
    if ( 0 == a_info.auth_attr_.length() ) {
        throw cc::Exception(sk_field_err_msg_invalid_or_missing_, "SigningInfo.auth_attr_");
    }
    
    unsigned char* ua = nullptr;
    
    try {
        
        const std::string& auth_attr = a_info.auth_attr_;

        size_t sz;

        try {
            const size_t mds = cppcodec::base64_rfc4648::decoded_max_size(auth_attr.length());
            ua = new unsigned char[mds];
            sz = cppcodec::base64_rfc4648::decode(ua, mds, auth_attr.c_str(), auth_attr.length());
        } catch (const cppcodec::parse_error& a_parse_error) {
            delete [] ua;
            const size_t mds = cppcodec::base64_url_unpadded::decoded_max_size(auth_attr.length());
            ua = new unsigned char[mds];
            sz = cppcodec::base64_url_unpadded::decode(ua, mds, auth_attr.c_str(), auth_attr.length());
        }
        
        a_info.enc_digest_ = cc::crypto::RSA::SignSHA256(ua, sz, a_key.uri_, a_key.password_);
        delete [] ua;
        
    } catch (...) {
        if ( nullptr != ua ) {
            delete [] ua;
        }
        cc::Exception::Rethrow(/* a_unhandled */ true, __FILE__, __LINE__, __FUNCTION__);
    }
}

// MARK: - [PUBLIC] PDF Document Signing


/**
 * @brief Sign a PDF document using a previously calculated 'SIGNER INFO' UNSIGNED attributes usign the private key.
 *
 * @param a_uri          PDF local URI.
 * @param a_range        See \link ByteRange \link.
 * @param a_digest       PDF document SHA256 digest base 64 encoded.
 * @param a_certificates Signing certificate and ( optionally ) all other certificates in chain.
 * @param a_key          Private key info.
 * @param o_info         See \link SigningInfo \link.
 */
void casper::pdf::Signer::Sign (const std::string& a_uri,
                                const casper::pdf::Signer::ByteRange& a_range,  const std::string& a_digest,
                                const casper::pdf::Signer::Certificates& a_certificates, const casper::pdf::Signer::PrivateKey& a_key,
                                casper::pdf::Signer::SigningInfo& o_info)
{
    o_info.digest_       = a_digest;
    o_info.signing_time_ = "";
    o_info.auth_attr_    = "";
    o_info.enc_digest_   = "";

    CalculateSigningAttributes(a_certificates.signing_, o_info);
    SignSigningAttributes(a_key, o_info);

    casper::openssl::P7::Sign(a_certificates.signing_, a_certificates.chain_, o_info.digest_, o_info.enc_digest_, o_info.signing_time_,
                              [this, a_uri, a_range] (const unsigned char* a_bytes, const size_t& a_size) {
                                Write(a_uri, a_range, a_bytes, a_size);
                              }
    );
}

/**
 * @brief Sign a PDF document using a previously calculated 'SIGNER INFO' SIGNED attributes.
 *
 * @param a_uri          PDF local URI.
 * @param a_range        See \link ByteRange \link.
 * @param a_info         See \link SigningInfo \link.
 * @param a_certificates Signing certificate and ( optionally ) all other certificates in chain.
 */
void casper::pdf::Signer::Sign (const std::string& a_uri,
                                const casper::pdf::Signer::ByteRange& a_range, const casper::pdf::Signer::SigningInfo& a_info,
                                const casper::pdf::Signer::Certificates& a_certificates)
{
    casper::openssl::P7::Sign(a_certificates.signing_, a_certificates.chain_, a_info.digest_, a_info.enc_digest_, a_info.signing_time_,
                              [this, a_uri, a_range] (const unsigned char* a_bytes, const size_t& a_size) {
                                Write(a_uri, a_range, a_bytes, a_size);
                              }
    );
}

// MARK: - [PUBLIC] - OTHER

/**
 * @brief Zero-out a /Contents in the PDF file
 *
 * @param a_uri   PDF local URI.
 * @param a_range /ByteRange info to zero-out.
 */
void casper::pdf::Signer::ZeroOut (const std::string& a_uri, const Signer::ByteRange& a_range)
{
    FILE* fp = fopen(a_uri.c_str(), "r+");
    if ( nullptr == fp ) {
        throw ::cc::Exception(sk_file_err_msg_fmt_unable_to_open_file_with_, a_uri.c_str(), strerror(errno));
    }
    try {
        ZeroOut(fp, a_range);
    } catch (const cc::Exception& a_cc_exception) {
        throw a_cc_exception;
    } catch (...) {
        cc::Exception::Rethrow(/* a_unhandled */ true, __FILE__, __LINE__, __FUNCTION__);
    }
    if ( 0 != fclose(fp) ) {
        throw ::cc::Exception(sk_file_err_msg_fmt_unable_to_close_file_with_, a_uri.c_str(), strerror(errno));
    }
}

// MARK: - [PUBLIC] - Data Extraction

/**
 * @brief Get \link ByteRange \link info from   PDF document.
 *
 * @param a_uri   PDF local URI.
 * @param a_page  Page number where to look for annotation.
 * @param o_range Loaded data, see \link ByteRange \link.
 */

void casper::pdf::Signer::GetByteRange (const std::string& a_uri, const ssize_t a_page, Signer::ByteRange& o_range)
{
    pdf::qpdf::Reader        reader;
    pdf::SignatureAnnotation annotation(signature_name_);
    // ... open PDF ...
    reader.Open(a_uri);
    // ... must be already present ...
    if ( false == reader.GetByteRange(a_page, annotation) ) {
        throw cc::Exception(sk_pdf_byte_range_not_found_, annotation.name_.c_str());
    }
    // ... copy data ...
    o_range = annotation.byte_range();
    // ... close PDF ...
    reader.Close();
}

/**
 * @brief Export a PCKS7 in PEM format.
 *
 * @param a_uri   PDF local URI.
 * @param a_range /ByteRange info where PKCS7 is at.
 * @param o_uri   PKCS7 decoded data.
 */
void casper::pdf::Signer::Export (const std::string& a_uri,
                                  const casper::pdf::Signer::ByteRange& a_range,
                                  const std::string& o_uri)
{
    FILE* fp = fopen(a_uri.c_str(), "r");
    if ( nullptr == fp ) {
        throw ::cc::Exception(sk_file_err_msg_fmt_unable_to_open_file_with_, a_uri.c_str(), strerror(errno));
    }
    unsigned char* bf = nullptr;
    try {
        
        if ( 0 != fseek(fp, 0, SEEK_END) ) {
            throw ::cc::Exception(sk_file_err_msg_fmt_unable_to_seek_to_end_of_file_, strerror(errno));
        }
        
        const size_t size = ftell(fp);
        rewind(fp);
        
        const size_t start  = a_range.before_start_ + a_range.before_size_ + 1;
        const size_t end    = a_range.after_start_ - 1;
        const size_t length = end - start;
        
        if ( size < length ) {
            throw cc::Exception("%s", sk_pdf_contents_not_enough_bytes_to_read_);
        }

        if ( 0 != fseek(fp, start, SEEK_SET) ) {
            throw ::cc::Exception(sk_file_err_msg_fmt_unable_to_seek_to_position_of_file_, start, strerror(errno));
        }

        bf = new unsigned char[length];
        
        const size_t bytes_read = fread(bf, sizeof(unsigned char), length, fp);
        if ( ferror(fp) ) {
            throw cc::Exception(sk_file_err_msg_fmt_read_error_, strerror(errno));
        }
        
        if ( bytes_read != length ) {
            throw cc::Exception(sk_file_err_msg_fmt_read_mismatch_, bytes_read, length);
        }

        if ( 0 != fclose(fp) ) {
            throw ::cc::Exception(sk_file_err_msg_fmt_unable_to_close_file_with_, a_uri.c_str(), strerror(errno));
        }
        fp = nullptr;
        
        // HEX 2 BIN
        const unsigned char* r_ptr  = bf;
        unsigned char*       w_ptr  = bf;
        size_t               len    = length;
        unsigned char        hex[3] = {0,0,0};
        size_t               adv    = 0;
        unsigned             v      = 0;
        const size_t         k1      = sizeof(unsigned char);
        const size_t         k2      = sizeof(unsigned char) * 2;
        while ( len ) {
            hex[0] = r_ptr[0];
            hex[1] = r_ptr[1];
            const unsigned char* hex_ptr = hex;
            while (*hex_ptr) {
                // get current character then increment
                char byte = *hex_ptr++;
                // transform hex character to the 4bit equivalent number, using the ascii table indexes
                if (byte >= '0' && byte <= '9') byte = byte - '0';
                else if (byte >= 'a' && byte <='f') byte = byte - 'a' + 10;
                else if (byte >= 'A' && byte <='F') byte = byte - 'A' + 10;
                // shift 4 to make space for new digit, and add the 4 bits of the new digit
                v = (v << 4) | (byte & 0xF);
            }
            w_ptr[0] = v;
            w_ptr   += k1;
            adv     += k1;
            r_ptr   += k2;
            len     -= k2;
        }
        
        casper::openssl::P7::Export(bf, adv, o_uri);
        
        delete [] bf;

    } catch (const cc::Exception& a_cc_exception) {
        delete [] bf;
        if ( fp != nullptr && 0 != fclose(fp) ) {
            throw ::cc::Exception(sk_file_err_msg_fmt_unable_to_close_file_with_, a_uri.c_str(), strerror(errno));
        }
        throw a_cc_exception;
    }
}

// MARK: - [PRIVATE] - WRITE

/**
 * @brief Write a PKCS7 object ( BER FORMAT ) o a PDF document.
 *
 * @param a_uri   PDF local URI.
 * @param a_range /ByteRange info where to write PKCS7 object.
 * @param a_bytes PCKS7 object bytes.
 * @param a_size  PCKS7 object size ( in bytes ).
 */
void casper::pdf::Signer::Write (const std::string& a_uri, const Signer::ByteRange& a_range,
                                 const unsigned char* a_bytes, const size_t a_size)
{
    FILE* fp = nullptr;
    
    try {
        
        // ... write PKCS7 bytes to file ( hex encoded ) ...
        const size_t pkcs7_hex_length  = ( 2 * sizeof(char) * a_size );
        const size_t start             = a_range.before_start_ + a_range.before_size_ + 1;
        const size_t end               = a_range.after_start_ - 1;
        const size_t length            = end - start;
        
        // ... ensure enough space ...
        if ( length < pkcs7_hex_length ) {
            throw ::cc::Exception("%s", sk_pkcs7_err_msg_fmt_unable_to_write_data_not_enough_space_);
        }
        
        // ... open file ...
        fp = fopen(a_uri.c_str(), "r+");
        if ( nullptr == fp ) {
            throw ::cc::Exception(sk_file_err_msg_fmt_unable_to_open_file_with_, a_uri.c_str(), strerror(errno));
        }
        
        // ... go to the beginning of /Contents ...
        if ( 0 != fseek(fp, start, SEEK_SET) ) {
            throw ::cc::Exception(sk_pkcs7_err_msg_fmt_unable_to_write_data_failed_seek_to_start_of_contents_, strerror(errno));
        }
        
        // ... write PKCS7 data ...
        {
            const unsigned char* ptr = a_bytes;
            size_t               len = a_size;
            size_t               bw   = 0;
            char data[2];
            while ( len-- ) {
                data[0]  = (*ptr & 0xF0) >> 4;
                data[0] += (data[0] > 9 ? 'A' - 10 : '0');
                data[1]  = (*ptr & 0x0F);
                data[1] += (data[1] > 9 ? 'A' - 10 : '0');
                bw = fwrite(data, sizeof(char), 2, fp);
                if ( 2 != bw ) {
                    throw cc::Exception(sk_pkcs7_err_msg_fmt_write_mismatch_, bw, static_cast<size_t>(2));
                } else if ( 0 != ferror(fp) ) {
                    throw cc::Exception(sk_pkcs7_err_msg_fmt_write_error_, strerror(errno));
                }
                ++ptr;
            }
        }

        // ... go to the beginning of the 'unused' bytes ...
        if ( 0 != fseek(fp, ( start + pkcs7_hex_length ), SEEK_SET) ) {
            throw ::cc::Exception(sk_pkcs7_err_msg_fmt_unable_to_write_data_failed_seek_to_end_of_contents_, strerror(errno));
        }

        // ... zero out signature remaining 'unused' space ...
        ZeroOut(fp, ( length - pkcs7_hex_length ));
        
        // ... close file ...
        if ( 0 != fclose(fp) ) {
            throw ::cc::Exception(sk_file_err_msg_fmt_unable_to_close_file_with_, a_uri.c_str(), strerror(errno));
        }
        fp = nullptr;
    } catch (const cc::Exception& a_cc_exception) {
        if ( fp != nullptr && 0 != fclose(fp) ) {
            throw ::cc::Exception(sk_file_err_msg_fmt_unable_to_close_file_with_, a_uri.c_str(), strerror(errno));
        }
        throw a_cc_exception;
    } catch (...) {
        if ( fp != nullptr && 0 != fclose(fp) ) {
            throw ::cc::Exception(sk_file_err_msg_fmt_unable_to_close_file_with_, a_uri.c_str(), strerror(errno));
        }
        cc::Exception::Rethrow(/* a_unhandled */ true, __FILE__, __LINE__, __FUNCTION__);
    }
    
    CC_ASSERT(nullptr == fp);
}

// MARK: - [PRIVATE] - ZERO-OUT

/**
 * @brief Zero-out a /Contents in the PDF file
 *
 * @param a_fp         FILE* Previously open file.
 * @param a_byte_range /ByteRange info where to write PKCS7 object.
 */
void casper::pdf::Signer::ZeroOut (FILE* a_fp, const Signer::ByteRange& a_byte_range)
{
    const size_t start  = a_byte_range.before_start_ + a_byte_range.before_size_ + 1;
    const size_t end    = a_byte_range.after_start_ - 1;
    const size_t length = end - start;
    
    // ... go to the beginning of the /Contents object ...
    if ( 0 != fseek(a_fp, start, SEEK_SET) ) {
        throw ::cc::Exception(sk_file_err_msg_fmt_unable_to_seek_to_position_of_file_, start, strerror(errno));
    }
    
    // ... zero-out until the end of the /Contents object ...
    ZeroOut(a_fp, length);
    
    // ... rewind to the beginning of the /Contents object ...
    if ( 0 != fseek(a_fp, start, SEEK_SET) ) {
        throw ::cc::Exception(sk_file_err_msg_fmt_unable_to_seek_to_position_of_file_, start, strerror(errno));
    }
}

/**
 * @brief Zero-out a file.
 *
 * @param a_fp   FILE* Previously open file.
 * @param a_size Number of bytes to write.
 */
void casper::pdf::Signer::ZeroOut (FILE* a_fp, const size_t& a_size)
{
    ssize_t remainder  = a_size;
    size_t  chunk_size = std::min(buffer_size_, static_cast<size_t>(remainder));
    size_t  bw         = 0;
    memset(buffer_, '0', buffer_size_);
    while ( remainder > 0 ) {
        bw = fwrite(buffer_, sizeof(unsigned char), chunk_size, a_fp);
        if ( chunk_size != bw ) {
            throw cc::Exception(sk_file_err_msg_fmt_write_mismatch_, bw, chunk_size);
        } else if ( 0 != ferror(a_fp) ) {
            throw cc::Exception(sk_file_err_msg_fmt_write_error_, strerror(errno));
        }
        remainder -= chunk_size;
        chunk_size = std::min(buffer_size_, static_cast<size_t>(remainder));
    }
}

// MARK: - [PRIVATE] - DIGEST CALCULATION

/**
 * @brief Calculate PDF digest.
 *
 * @param a_uri        PDF local URI.
 * @param a_byte_range /ByteRange info where PKCS7 object is or will be.
 * @param o_digest     SHA256 Base 64 encoded calculated digest value.
 */
void casper::pdf::Signer::CalculateDigest (const std::string& a_uri, const Signer::ByteRange& a_byte_range, std::string& o_digest)
{
    cc::fs::file::Reader fr;
    
    fr.Open(a_uri, cc::fs::file::Reader::Mode::Read);
    
    const std::vector<std::pair<size_t, size_t>> chunks = {
        // ... bytes before '/Contents'
        { a_byte_range.before_start_ , a_byte_range.before_size_ },
        // ... bytes after '/Contents'
        { a_byte_range.after_start_ , a_byte_range.after_size_ }
    };
    
    cc::hash::SHA256 sha256;
    
    sha256.Initialize();

    // ... two iterations required ...
    for ( auto it : chunks ) {        
        // ... go ot the beginning of byte range ...
        fr.Seek(it.first);
        size_t br  = 0;
        size_t rm  = it.second;
        size_t cs  = std::min(buffer_size_, rm);
        bool   eof = true;
        // ... read and update hash calculation ...
        while ( ( br = fr.Read(buffer_, cs, eof) ) && false == eof ) {
            sha256.Update(buffer_, br);
            rm -= br;
            cs  = std::min(buffer_size_, rm);
        }
    }
    
    fr.Close();
    
    o_digest = sha256.Finalize(::cc::hash::SHA256::OutputFormat::BASE64_RFC4648);
}

/**
 * @file certificate.cc
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

#include "casper/openssl/certificate.h"

#include "cc/exception.h"

#include "cc/macros.h"
#include "cc/types.h"

#include <openssl/pem.h>
#include <openssl/err.h>

#define CASPER_OPENSSL_CERTIFICATE_THROW_OPENSSL_ERROR(a_format, ...) \
{ \
    char __tmp_error__[130] = {0}; \
    char __tmp_msg__  [257] = {0}; \
    ERR_error_string(ERR_get_error(), __tmp_error__); \
    snprintf(__tmp_msg__, 256, a_format, __VA_ARGS__); \
    throw cc::Exception(std::string(__tmp_msg__) + " - " + std::string(__tmp_error__)); \
}

/**
 * @brief Default constructor.
 *
 * @param a_origin One of \link Certificate::Origin \link.
 * @param a_format One Of \link Certificate::Format \link.
 */
casper::openssl::Certificate::Certificate (const casper::openssl::Certificate::Origin a_origin, const casper::openssl::Certificate::Format a_format)
    : origin_(a_origin), format_(a_format)
{
    /* empty */
}

/**
 * @brief Constructor.
 *
 * @param a_origin One of \link Certificate::Origin \link.
 * @param a_format One Of \link Certificate::Format \link.
 * @param a_data   Certificate data, related to \link Certificate::Origin \link.
 */
casper::openssl::Certificate::Certificate (const casper::openssl::Certificate::Origin a_origin, const casper::openssl::Certificate::Format a_format,
                                   const std::string& a_data)
    : origin_(a_origin), format_(a_format), data_(a_data)
{
    /* empty */
}

/**
 * @brief Copy constructor.
 *
 * @param a_certificate Object to copy from.
 */
casper::openssl::Certificate::Certificate (const casper::openssl::Certificate& a_certificate)
    : origin_(a_certificate.origin_), format_(a_certificate.format_), data_(a_certificate.data_)
{
    /* empty */
}

/**
 * @brief Destructor.
 */
casper::openssl::Certificate::~Certificate ()
{
    /* empty */
}

// MARK: -

/**
 * @brief Load a \link Certificate \link.
 *
 * @param a_certificate Certificate to load.
 *
 * @param o_x509 Loaded \link X509 \link certificate destination.
 *
 * @return X509 size.
 */
size_t casper::openssl::Certificate::Load (const Certificate& a_certificate, X509** o_x509)
{    
    // ... first release previously loaded X509 certificat ...
    Unload(o_x509);
    // ... now, according to origin, load a X509 certificate ...
    if ( openssl::Certificate::Origin::Memory == a_certificate.origin_ ) {
        //
        // PEM_read_bio_X509(BIO *bp, X509 **x, pem_password_cb *cb, void *u);
        //
        // Returns X509* for success and NULL for failure
        BIO* pem_bio = BIO_new_mem_buf((void*)a_certificate.data_.c_str(), static_cast<int>(a_certificate.data_.length()));
        if ( ! PEM_read_bio_X509(pem_bio, o_x509, /* pem_password_cb */ nullptr, /* u */ nullptr) ) {
            BIO_free(pem_bio);
            CASPER_OPENSSL_CERTIFICATE_THROW_OPENSSL_ERROR("%s", "Error while loading X509 certificate!");
        } else {
            BIO_free(pem_bio);
        }
    } else if ( openssl::Certificate::Origin::File == a_certificate.origin_ ) {
        FILE* crt_fp = nullptr;
        try {
            crt_fp = fopen(a_certificate.data_.c_str(), "rb");
            if ( nullptr == crt_fp ) {
                throw ::cc::Exception("Unable to open '%s': %s !", a_certificate.data_.c_str(), strerror(errno));
            }
            (*o_x509) = PEM_read_X509(crt_fp, NULL, NULL, NULL);
            if ( 0 != fclose(crt_fp) ) {
                throw ::cc::Exception("Unable to close '%s': %s !", a_certificate.data_.c_str(), strerror(errno));
            }
            crt_fp = nullptr;
        } catch (const ::cc::Exception& a_cc_exception) {
            if ( nullptr != crt_fp && 0 != fclose(crt_fp) ) {
                throw ::cc::Exception("Unable to close '%s': %s !", a_certificate.data_.c_str(), strerror(errno));
            }
            cc::Exception::Rethrow(/* a_unhandled */ false, __FILE__, __LINE__, __FUNCTION__);
        }
        CC_ASSERT(nullptr == crt_fp);
    } else {
        throw cc::Exception("Loading certificate from origing " UINT8_FMT " not implemented!", static_cast<uint8_t>(a_certificate.origin_));
    }
    // ... ensure it was loaded ..
    if ( nullptr == (*o_x509) ) {
        throw cc::Exception("%s", "Unable to load certificate - nullptr!");
    }
    // ... return it's size ...
    return i2d_X509(*(o_x509), NULL);
}

/**
 * @brief Release a previous loaded \link X509 \link
 *
 * @param o_x509 Loaded \link X509 \link certificate to release.
 */
void casper::openssl::Certificate::Unload (X509** o_x509)
{
    CC_ASSERT(o_x509 != nullptr);
    if ( nullptr != (*o_x509) ) {
        X509_free((*o_x509));
        (*o_x509) = nullptr;
    }
}

/**
 * @brief Load a \link Certificate::Chain \link.
 *
 * @param a_chain Chain to load.
 *
 * @param o_x509 Loaded \link X509 \link certificate chain destination.
 *
 * @return X509 size.
 */

size_t casper::openssl::Certificate::Load (const Certificate::Chain& a_chain, std::vector<X509*>& o_chain)
{
    size_t size = 0;
    for ( auto certificate : a_chain ) {
        X509* x509 = nullptr;
        size += Load(certificate, &x509);
        o_chain.push_back(x509);
    }
    return size;
}

/**
 * @brief Release a previous loaded \link X509 \link chain.
 *
 * @param o_x509 Loaded \link X509 \link certificate chain to release.
 */

void casper::openssl::Certificate::Unload (std::vector<X509*>& o_chain)
{
    for ( auto x509 : o_chain ) {
        Unload(&x509);
    }
    o_chain.clear();
}

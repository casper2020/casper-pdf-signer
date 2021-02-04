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

#include "casper/openssl/p7.h"

#include "cc/b64.h"

#include "cc/exception.h"

#include "cc/macros.h"

#include "cc/crypto/rsa.h"

#include "cc/fs/file.h"

#include <openssl/pem.h>
#include <openssl/err.h>

#define CASPER_OPENSSL_P7_THROW_OPENSSL_ERROR(a_format, ...) \
{ \
    char __tmp_error__[130] = {0}; \
    char __tmp_msg__  [257] = {0}; \
    ERR_error_string(ERR_get_error(), __tmp_error__); \
    snprintf(__tmp_msg__, 256, a_format, __VA_ARGS__); \
    throw cc::Exception(std::string(__tmp_msg__) + " - " + std::string(__tmp_error__)); \
}

#define CASPER_OPENSSL_P7_THROW_OPENSSL_EXCEPTION(a_format, ...) \
{ \
    throw cc::Exception(a_format, __VA_ARGS__); \
}

// MARK: -

const char* const casper::openssl::P7::sk_p7_err_msg_unable_to_create_new_object_ = "Unable to create new '%s' - %s!";
const char* const casper::openssl::P7::sk_p7_err_msg_unable_to_set_type_          = "Unable to set type '%s'";
const char* const casper::openssl::P7::sk_p7_err_msg_unable_to_add_attribute_     = "Unable to add '%s' attribute";
const char* const casper::openssl::P7::sk_p7_err_msg_unable_to_load_private_key_  = "Error while loading RSA private key";
const char* const casper::openssl::P7::sk_p7_err_msg_unable_to_add_certificate_   = "Unable to add an X509 certificate";
const char* const casper::openssl::P7::sk_p7_err_msg_unable_to_add_signer_        = "Unable add 'signer' @ PKCS7 object";
const char* const casper::openssl::P7::sk_p7_err_msg_unable_to_set_content_       = "Unable to set PKCS7 'content'";
const char* const casper::openssl::P7::sk_p7_err_msg_unable_to_set_digest_        = "Unable to set PKSC7 'digest'";
const char* const casper::openssl::P7::sk_p7_err_msg_unable_to_set_si_field_      = "Unable to set SIGNER INFO '%s' field";
const char* const casper::openssl::P7::sk_p7_err_msg_unable_to_to_sign_si_        = "Unable to sign 'SIGNER INFO'";
const char* const casper::openssl::P7::sk_p7_err_msg_unable_to_encoded_to_der_    = "Unable to encode a PKCS7 object to 'DER' format";
const char* const casper::openssl::P7::sk_p7_err_msg_unable_to_export_to_pem_     = "Unable to export PKCS7 to PEM format";
const char* const casper::openssl::P7::sk_p7_err_msg_unable_to_open_file_with_    = "Unable to open file open '%s': %s !";
const char* const casper::openssl::P7::sk_p7_err_msg_unable_to_close_file_with_   = "Unable to close file open '%s': %s !";
const char* const casper::openssl::P7::sk_p7_err_msg_unable_to_load_              = "Unable load PKCS7";
const char* const casper::openssl::P7::sk_p7_exp_msg_unable_to_load_              = "Unable load PKCS7 - %s!";
const char* const casper::openssl::P7::sk_p7_err_msg_signature_validation_failed_ = "Signature validation failed!";

// MARK: -

/**
 * @brief Default constructor.
 */
casper::openssl::P7::P7 ()
{
    /* empty */
}

/**
 * @brief Destrutor.
 */
casper::openssl::P7::~P7 ()
{
    /* empty */
}

// MARK: - [PUBLIC] - PCKS7 Signed With Private Key

/**
 * @brief Produce a signed PCKS7 using a private key.
 *
 * @param a_certificate   Signing certificate.
 * @param a_chain         Other certificates in chain.
 * @param a_key           Private key info.
 * @param a_digest        Digest.
 * @param a_signing_time  Signing time used to calculate signing attributes.
 * @param a_callback      Function to call to deliver PCKS7 bytes.
 * @param o_enc_digest    Encripted digest.
 */
void casper::openssl::P7::Sign (const Certificate& a_certificate, const Certificate::Chain& a_chain, const PrivateKey& a_key,
                                const std::string& a_digest, const std::string& a_signing_time,
                                std::function<void(const unsigned char*, const size_t&)> a_callback,
                                std::string* o_enc_digest)
{
    X509*              x509 = nullptr;
    std::vector<X509*> x509_chain;
    
    PKCS7*             p7 = nullptr;
    
    FILE*              kfp = nullptr;
    EVP_PKEY*          key = nullptr;
    RSA*               rsa = nullptr;
    
    ASN1_UTCTIME*      st = nullptr;
    PKCS7_SIGNER_INFO* si = nullptr;
    cc::Exception*     ex = nullptr;
    
    unsigned char*     dh = nullptr;
    unsigned char*     sh = nullptr;
    
    BIO*               bo = nullptr;

    try {

        // ... load and add certificate ...
        (void)Certificate::Load(a_certificate, &x509);

        // ... load and add other certificates in chain ...
        (void)Certificate::Load(a_chain, x509_chain);
        
        // ... load private key ...
        key = EVP_PKEY_new();
        kfp = fopen(a_key.uri_.c_str(), "r");
        if ( nullptr == kfp ) {
            CASPER_OPENSSL_P7_THROW_OPENSSL_EXCEPTION(sk_p7_err_msg_unable_to_close_file_with_, a_key.uri_.c_str(), strerror(errno));
        }
        if ( 0 != a_key.password_.length() ) {
            if ( nullptr == PEM_read_RSAPrivateKey(kfp, &rsa, &casper::openssl::PrivateKey::PEMPasswordCallback, (void*)a_key.password_.c_str()) ) {
                CASPER_OPENSSL_P7_THROW_OPENSSL_ERROR("%s", sk_p7_err_msg_unable_to_load_private_key_);
            }
        } else {
            if ( ! PEM_read_RSAPrivateKey(kfp, &rsa, NULL, NULL) ) {
                CASPER_OPENSSL_P7_THROW_OPENSSL_ERROR("%s", sk_p7_err_msg_unable_to_load_private_key_);
            }
        }
        if ( 0 != fclose(kfp) ) {
            CASPER_OPENSSL_P7_THROW_OPENSSL_EXCEPTION(sk_p7_err_msg_unable_to_close_file_with_, a_key.uri_.c_str(), strerror(errno));
        }
        kfp = nullptr;
        EVP_PKEY_set1_RSA(key, rsa);

        // ... create and prepare a PKCS7 ...
        p7 = PKCS7_new();
        if ( nullptr == p7 ) {
            CASPER_OPENSSL_P7_THROW_OPENSSL_EXCEPTION(sk_p7_err_msg_unable_to_create_new_object_, "PKCS7", "nullptr");
        }

        // ... set 'type' ...
        if ( 1 != PKCS7_set_type(p7, NID_pkcs7_signed) ) {
            CASPER_OPENSSL_P7_THROW_OPENSSL_ERROR(sk_p7_err_msg_unable_to_set_type_, "signed");
        }

        // ... create and prepare a SIGNER_INFO ...
        si = PKCS7_add_signature(p7, x509, key, EVP_sha256());
        if ( nullptr == si ) {
            CASPER_OPENSSL_P7_THROW_OPENSSL_EXCEPTION(sk_p7_err_msg_unable_to_create_new_object_, "SIGNER_INFO", "nullptr");
        }

        // ... add content type ...
        if ( 1 != PKCS7_add_attrib_content_type(si, OBJ_nid2obj(NID_pkcs7_data)) ) {
            CASPER_OPENSSL_P7_THROW_OPENSSL_ERROR(sk_p7_err_msg_unable_to_add_attribute_, "content type");
        }
        
        // ... add signing time ...
        if ( 0 != a_signing_time.length() ) {
            st = ASN1_UTCTIME_new();
            ASN1_UTCTIME_set_string(st, a_signing_time.c_str());
            if ( 1 != PKCS7_add0_attrib_signing_time(si, st) ) {
                CASPER_OPENSSL_P7_THROW_OPENSSL_ERROR(sk_p7_err_msg_unable_to_add_attribute_, "signing time");
            }
            st = nullptr; // ... it will be relased once si is released ...
        }
                
        // ... decode and add digest bytes ...
        const size_t dsz = DecodeBase64(a_digest, &dh);
        if ( 1 != PKCS7_add1_attrib_digest(si, (const unsigned char*) dh, static_cast<int>(dsz)) ) {
            CASPER_OPENSSL_P7_THROW_OPENSSL_ERROR(sk_p7_err_msg_unable_to_add_attribute_, "digest");
        }
        
        // ... add certificate ..
        if ( 1 != PKCS7_add_certificate(p7, x509) ) {
            CASPER_OPENSSL_P7_THROW_OPENSSL_ERROR("%s", sk_p7_err_msg_unable_to_add_certificate_);
        }
        
        // ... add other certificates in chain ...
        for ( auto cert : x509_chain ) {
            if ( 1 != PKCS7_add_certificate(p7, cert) ) {
                CASPER_OPENSSL_P7_THROW_OPENSSL_ERROR("%s", sk_p7_err_msg_unable_to_add_certificate_);
            }
        }
        
        // ... continue PKCS7 setup ...
        PKCS7_set_detached(p7, 1);
        if ( 1 != PKCS7_content_new(p7, NID_pkcs7_data) ) {
            CASPER_OPENSSL_P7_THROW_OPENSSL_ERROR("%s", sk_p7_err_msg_unable_to_set_content_);
        }
        
        // ... finalize PKCS7 by signing signer info ...
        if ( 1 != PKCS7_SIGNER_INFO_sign(si) ) {
            CASPER_OPENSSL_P7_THROW_OPENSSL_ERROR("%s", sk_p7_err_msg_unable_to_to_sign_si_);
        }

        bo = BIO_new(BIO_s_mem());
        if ( nullptr == bo ) {
            CASPER_OPENSSL_P7_THROW_OPENSSL_EXCEPTION(sk_p7_err_msg_unable_to_create_new_object_, "BIO", "nullptr");
        }

        // ... PKCS7 -> DER format
        if ( 1 != i2d_PKCS7_bio(bo, p7) ) {
            CASPER_OPENSSL_P7_THROW_OPENSSL_ERROR("%s", sk_p7_err_msg_unable_to_encoded_to_der_);
        }
        
        unsigned char* bytes = nullptr; // just a ptr to bytes inside bo
        const size_t   size  = BIO_get_mem_data(bo, &bytes);
        
        a_callback(bytes, size);
            
        if ( nullptr != o_enc_digest ) {
            (*o_enc_digest) = cc::base64_rfc4648::encode((unsigned char*)si->enc_digest->data, si->enc_digest->length);
        }

    } catch (const cc::Exception& a_cc_exception) {
        ex = new cc::Exception(a_cc_exception);
    }
    
    if ( p7 != nullptr ) {
        PKCS7_free(p7);
    }

    Certificate::Unload(&x509);
    Certificate::Unload(x509_chain);
    
    if ( nullptr != rsa ) {
        RSA_free(rsa);
    }
    if ( nullptr != key ) {
        EVP_PKEY_free(key);
    }

    if ( nullptr != dh ) {
        delete [] dh;
        dh = nullptr;
    }
    
    if ( nullptr != sh ) {
        delete [] sh;
        sh = nullptr;
    }
        
    if ( nullptr != bo ) {
        BIO_free(bo);
    }
    
    if ( nullptr != kfp && 0 != fclose(kfp) ) {
        if ( ex != nullptr ) {
            delete ex;
        }
        CASPER_OPENSSL_P7_THROW_OPENSSL_EXCEPTION(sk_p7_err_msg_unable_to_close_file_with_, a_key.uri_.c_str(), strerror(errno));
    }
    
    if ( ex != nullptr ) {
        const cc::Exception e = cc::Exception(*ex);
        delete  ex;
        throw e;
    }
}


// MARK: - [PUBLIC] - SIGNER INFO - Signed Attributes

/**
 * @brief Get current time as the signing time.
 *
 * @param o_value Signing time YYMMDDHHMMSSZ - X509.
 */
void casper::openssl::P7::GetSigningTime (std::string& o_value)
{
    ASN1_UTCTIME* st = X509_gmtime_adj(NULL, 0);
    if ( nullptr == st ) {
        CASPER_OPENSSL_P7_THROW_OPENSSL_EXCEPTION(sk_p7_err_msg_unable_to_create_new_object_, "ASN1_UTCTIME", "nullptr");
    }
    o_value = std::string(reinterpret_cast<const char* const>(st->data), st->length);
    ASN1_UTCTIME_free(st);
}

/**
 * @brief Calculate attributes on \link PKCS7_SIGNER_INFO \link that will be signed.
 *
 * @param a_digest      Previously calculate digest.
 * @param a_certificate Signing certificate.
 */
void casper::openssl::P7::CalculateSigningAttributes (const std::string& a_digest, const Certificate* a_certificate,
                                                      std::string& o_signing_time, std::string& o_auth_attr)
{
    
    X509*              x509 = nullptr;
    
    PKCS7_SIGNER_INFO* si = nullptr;
    ASN1_UTCTIME*      st = nullptr;

    unsigned char*     dh = nullptr;
    unsigned char *    ab = nullptr;

    cc::Exception*     ex = nullptr;

    try {

        //
        // ... load certificate ...
        //
        if ( nullptr != a_certificate ) {
            (void)Certificate::Load(*a_certificate, &x509);
        }

        //
        // ... create and prepare a SIGNER_INFO ...
        //
        si = PKCS7_SIGNER_INFO_new();
        if ( nullptr == si ) {
            CASPER_OPENSSL_P7_THROW_OPENSSL_EXCEPTION(sk_p7_err_msg_unable_to_create_new_object_, "SIGNER_INFO", "nullptr");
        }
        
        if ( 1 != ASN1_INTEGER_set(si->version, 1) ) {
            CASPER_OPENSSL_P7_THROW_OPENSSL_ERROR(sk_p7_err_msg_unable_to_set_si_field_, "version");
        }
        
        if ( nullptr != x509 ) {
            if ( 1 != X509_NAME_set(&si->issuer_and_serial->issuer, X509_get_issuer_name(x509)) ) {
                CASPER_OPENSSL_P7_THROW_OPENSSL_ERROR(sk_p7_err_msg_unable_to_set_si_field_, "issuer");
            }
            
            ASN1_INTEGER_free(si->issuer_and_serial->serial);
            si->issuer_and_serial->serial = ASN1_INTEGER_dup(X509_get_serialNumber(x509));
        }
         
        // ... set digest algorithm ...
        if ( 1 != X509_ALGOR_set0(si->digest_alg, OBJ_nid2obj(EVP_MD_type(EVP_sha256())), V_ASN1_NULL, NULL) ) {
            CASPER_OPENSSL_P7_THROW_OPENSSL_ERROR(sk_p7_err_msg_unable_to_set_si_field_, "digest alg");
        }

        // ... add content type ...
        if ( 1 != PKCS7_add_attrib_content_type(si, OBJ_nid2obj(NID_pkcs7_data)) ) {
            CASPER_OPENSSL_P7_THROW_OPENSSL_ERROR(sk_p7_err_msg_unable_to_add_attribute_, "content type");
        }
        
        // ... add signing time ...
        if ( 0 != o_signing_time.length() ) {
            st = ASN1_UTCTIME_new();
            ASN1_UTCTIME_set_string(st, o_signing_time.c_str());
        } else {
            st = X509_gmtime_adj(NULL, 0);
        }
        if ( 1 != PKCS7_add0_attrib_signing_time(si, st) ) {
            CASPER_OPENSSL_P7_THROW_OPENSSL_ERROR(sk_p7_err_msg_unable_to_add_attribute_, "signing time");
        }
        o_signing_time = std::string(reinterpret_cast<const char* const>(st->data), st->length);
        st = nullptr; // ... it will be relased once si is released ...

        // ... decode and add document hash bytes ...
        const size_t dsz = DecodeBase64(a_digest, &dh);
        if ( 1 != PKCS7_add1_attrib_digest(si, (const unsigned char*) dh, static_cast<int>(dsz)) ) {
            CASPER_OPENSSL_P7_THROW_OPENSSL_ERROR(sk_p7_err_msg_unable_to_add_attribute_, "digest");
        }
                
        // ... get auth_attr value ...
        const int auth_attr_len = ASN1_item_i2d((ASN1_VALUE *)si->auth_attr, &ab, ASN1_ITEM_rptr(PKCS7_ATTR_SIGN));
        // ... convert it to base 64 ...
        o_auth_attr = cc::base64_rfc4648::encode((unsigned char*)ab, auth_attr_len);

    } catch (const cc::Exception& a_cc_exception) {
        ex = new cc::Exception(a_cc_exception);
    }
    
    if ( nullptr != x509 ) {
        (void)Certificate::Unload(&x509);
    }
    
    if ( nullptr != si ) {
        PKCS7_SIGNER_INFO_free(si);
    }
    
    if ( nullptr != st ) {
        ASN1_UTCTIME_free(st);
    }
    
    if ( nullptr != dh ) {
        delete [] dh;
    }
    
    if ( nullptr != ab ) {
        OPENSSL_free(ab);
    }
    
    if ( ex != nullptr ) {
        const cc::Exception e = cc::Exception(*ex);
        delete  ex;
        throw e;
    }

}

/**
 * @brief Produce a signed PCKS7 using an externally signed hash.
 *
 * @param a_certificate   Signing certificate.
 * @param a_chain         Other certificates in chain.
 * @param a_digest        Digest.
 * @param a_enc_digest    Encripted digest.
 * @param a_signing_time  Signing time used to calculate signing attributes.
 * @param a_callback      Function to call to deliver PCKS7 bytes.
 */
void casper::openssl::P7::Sign (const Certificate& a_certificate, const Certificate::Chain& a_chain,
                                const std::string& a_digest, const std::string& a_enc_digest, const std::string& a_signing_time,
                                std::function<void(const unsigned char*, const size_t&)> a_callback)
{
    X509* x509 = nullptr;
    std::vector<X509*> x509_chain;
    
    PKCS7*             p7   = nullptr;
    PKCS7_SIGNER_INFO* si   = nullptr;
    bool               f_si = true;
    cc::Exception*     ex   = nullptr;
    ASN1_UTCTIME*      st   = nullptr;
    
    unsigned char*     dh  = nullptr;
    unsigned char*     sh  = nullptr;
    
    BIO*               bo  = nullptr;

    try {

        // ... load and add certificate ...
        (void)Certificate::Load(a_certificate, &x509);

        // ... load and add other certificates in chain ...
        (void)Certificate::Load(a_chain, x509_chain);

        // ... create and prepare a PKCS7 ...
        p7 = PKCS7_new();
        if ( nullptr == p7 ) {
            CASPER_OPENSSL_P7_THROW_OPENSSL_EXCEPTION(sk_p7_err_msg_unable_to_create_new_object_, "PKCS7", "nullptr");
        }

        if ( 1 != PKCS7_set_type(p7, NID_pkcs7_signed) ) {
            CASPER_OPENSSL_P7_THROW_OPENSSL_ERROR(sk_p7_err_msg_unable_to_set_type_, "signed");
        }
        
        if ( 1 != PKCS7_content_new(p7, NID_pkcs7_data) ) {
            CASPER_OPENSSL_P7_THROW_OPENSSL_ERROR("%s", sk_p7_err_msg_unable_to_set_content_);
        }

        if ( 1 != PKCS7_set_digest(p7, EVP_sha256()) ) {
            CASPER_OPENSSL_P7_THROW_OPENSSL_ERROR("%s", sk_p7_err_msg_unable_to_set_digest_);
        }

        // ... create and prepare a SIGNER_INFO ...
        si = PKCS7_SIGNER_INFO_new();
        if ( nullptr == si ) {
            CASPER_OPENSSL_P7_THROW_OPENSSL_EXCEPTION(sk_p7_err_msg_unable_to_create_new_object_, "SIGNER_INFO", "nullptr");
        }
        
        if ( 1 != ASN1_INTEGER_set(si->version, 1) ) {
            CASPER_OPENSSL_P7_THROW_OPENSSL_ERROR(sk_p7_err_msg_unable_to_set_si_field_, "version");
        }
        
        if ( 1 != X509_NAME_set(&si->issuer_and_serial->issuer, X509_get_issuer_name(x509)) ) {
            CASPER_OPENSSL_P7_THROW_OPENSSL_ERROR(sk_p7_err_msg_unable_to_set_si_field_, "issuer");
        }
        
        ASN1_INTEGER_free(si->issuer_and_serial->serial);
        if ( ! ( si->issuer_and_serial->serial = ASN1_INTEGER_dup(X509_get_serialNumber(x509)) ) ) {
            CASPER_OPENSSL_P7_THROW_OPENSSL_ERROR(sk_p7_err_msg_unable_to_set_si_field_, "serial");
        }
            
        if ( 1 != X509_ALGOR_set0(si->digest_alg, OBJ_nid2obj(EVP_MD_type(EVP_sha256())), V_ASN1_NULL, NULL) ) {
            CASPER_OPENSSL_P7_THROW_OPENSSL_ERROR(sk_p7_err_msg_unable_to_set_si_field_, "digest alg");
        }
        
        if ( 1 != PKCS7_add_signer(p7, si) ) {
            CASPER_OPENSSL_P7_THROW_OPENSSL_ERROR("%s", sk_p7_err_msg_unable_to_add_signer_);
        }
        f_si = false;
        
        // ... add certificate ...
        if ( 1 != PKCS7_add_certificate(p7, x509) ) {
            CASPER_OPENSSL_P7_THROW_OPENSSL_ERROR("%s", sk_p7_err_msg_unable_to_add_certificate_);
        }
                
        // ... add other certificates in chain ...
        for ( auto cert : x509_chain ) {
            if ( 1 != PKCS7_add_certificate(p7, cert) ) {
                CASPER_OPENSSL_P7_THROW_OPENSSL_ERROR("%s", sk_p7_err_msg_unable_to_add_certificate_);
            }
        }

        // ... continue SIGNER_INFO setup ...
        if ( 1 != X509_ALGOR_set0(si->digest_enc_alg, OBJ_nid2obj(NID_rsaEncryption), V_ASN1_NULL, NULL) ) {
            CASPER_OPENSSL_P7_THROW_OPENSSL_ERROR(sk_p7_err_msg_unable_to_set_si_field_, "digest enc alg");
        }
        
        // ... continue PKCS7 setup ...
        PKCS7_set_detached(p7, 1);
            
        // ... add content type ...
        if ( 1 != PKCS7_add_attrib_content_type(si, OBJ_nid2obj(NID_pkcs7_data)) ) {
            CASPER_OPENSSL_P7_THROW_OPENSSL_ERROR(sk_p7_err_msg_unable_to_add_attribute_, "content type");
        }

        // ... add signing time ...
        if ( 0 != a_signing_time.length() ) {
            st = ASN1_UTCTIME_new();
            ASN1_UTCTIME_set_string(st, a_signing_time.c_str());
            if ( 1 != PKCS7_add0_attrib_signing_time(si, st) ) {
                CASPER_OPENSSL_P7_THROW_OPENSSL_ERROR(sk_p7_err_msg_unable_to_add_attribute_, "signing time");
            }
            st = nullptr; // ... it will be relased once si is released ...
        }
        
        // ... decode and add digest bytes ...
        const size_t dsz = DecodeBase64(a_digest, &dh);
        if ( 1 != PKCS7_add1_attrib_digest(si, (const unsigned char*) dh, static_cast<int>(dsz)) ) {
            CASPER_OPENSSL_P7_THROW_OPENSSL_ERROR(sk_p7_err_msg_unable_to_add_attribute_, "digest");
        }
        
        // ... decode and add signed digest bytes ...
        const size_t esz = DecodeBase64(a_enc_digest, &sh);
        ASN1_STRING_free(si->enc_digest);
        si->enc_digest = ASN1_OCTET_STRING_new();
        if ( nullptr == si->enc_digest ) {
            CASPER_OPENSSL_P7_THROW_OPENSSL_EXCEPTION(sk_p7_err_msg_unable_to_create_new_object_, "ASN1_OCTET_STRING", "nullptr");
        }
        ASN1_OCTET_STRING_set(si->enc_digest, sh, static_cast<int>(esz));

        // ... prepare for conversion ...
        bo = BIO_new(BIO_s_mem());
        if ( nullptr == bo ) {
            CASPER_OPENSSL_P7_THROW_OPENSSL_EXCEPTION(sk_p7_err_msg_unable_to_create_new_object_, "BIO", "nullptr");
        }
        
        // ... PKCS7 -> DER format
        if ( 1 != i2d_PKCS7_bio(bo, p7) ) {
            CASPER_OPENSSL_P7_THROW_OPENSSL_ERROR("%s", sk_p7_err_msg_unable_to_encoded_to_der_);
        }
        
        unsigned char* bytes = nullptr; // just a ptr to bytes inside bo
        const size_t   size   = BIO_get_mem_data(bo, &bytes);
        
        a_callback(bytes, size);

    } catch (const cc::Exception& a_cc_exception) {
        ex = new cc::Exception(a_cc_exception);
    }
        
    if ( p7 != nullptr ) {
        PKCS7_free(p7);
    }
    
    if ( true == f_si && nullptr != si ) {
        PKCS7_SIGNER_INFO_free(si);
    }
    
    if ( nullptr != st ) {
        ASN1_UTCTIME_free(st);
    }

    Certificate::Unload(&x509);
    Certificate::Unload(x509_chain);
    
    if ( nullptr != dh ) {
        delete [] dh;
        dh = nullptr;
    }

    if ( nullptr != sh ) {
        delete [] sh;
        sh = nullptr;
    }
    
    if ( nullptr != bo ) {
        BIO_free(bo);
    }
    
    if ( ex != nullptr ) {
        const cc::Exception e = cc::Exception(*ex);
        delete  ex;
        throw e;
    }
}

// MARK: - [PUBLIC] - Export a PKCS7 to a file in PEM format

/**
 * @brief Export a PKCS7 to a file in PEM format.
 *
 * @param a_pkcs7 Object to export.
 * @param a_uri Local file URI - will be overwritten.
 */
void casper::openssl::P7::Export (const PKCS7* a_pkcs7, const std::string& a_uri)
{
    PKCS7* p7 = const_cast<PKCS7*>(a_pkcs7);
    FILE* fp = fopen(a_uri.c_str(), "w");
    if ( nullptr == fp ) {
        CASPER_OPENSSL_P7_THROW_OPENSSL_EXCEPTION(sk_p7_err_msg_unable_to_open_file_with_, a_uri.c_str(), strerror(errno));
    }
    if ( 1 != PEM_write_PKCS7(fp, p7) ) {
        CASPER_OPENSSL_P7_THROW_OPENSSL_ERROR("%s", sk_p7_err_msg_unable_to_export_to_pem_);
    }
    if ( 0 != fclose(fp) ) {
        CASPER_OPENSSL_P7_THROW_OPENSSL_EXCEPTION(sk_p7_err_msg_unable_to_close_file_with_, a_uri.c_str(), strerror(errno));
    }
}

/**
 * @brief Export a PKCS7 to a file in PEM format.
 *
 * @param a_pkcs7  Bytes to export.
 * @param a_length Number of bytes available to export.
 * @param a_uri    Local file URI - will be overwritten.
 */
void casper::openssl::P7::Export (const unsigned char* a_pkcs7, const size_t a_length, const std::string& a_uri)
{
    PKCS7*         p7 = nullptr;
    BIO*           bi = nullptr;
    cc::Exception* ex = nullptr;
    
    try {
        
        bi = BIO_new(BIO_s_mem());
        auto bw = BIO_write(bi, a_pkcs7, static_cast<int>(a_length));
        if ( bw != a_length ) {
            CASPER_OPENSSL_P7_THROW_OPENSSL_EXCEPTION(sk_p7_exp_msg_unable_to_load_, "unable to write all bytes to BIO!");
        }
        if ( nullptr == ( p7 = d2i_PKCS7_bio(bi, NULL) ) ) {
            CASPER_OPENSSL_P7_THROW_OPENSSL_ERROR("%s", sk_p7_err_msg_unable_to_load_);
        }
        Export(p7, a_uri);

    } catch (const cc::Exception& a_cc_exception) {
        ex = new cc::Exception(a_cc_exception);
    }
    
    if ( p7 != nullptr ) {
        PKCS7_free(p7);
    }
    
    if ( nullptr != bi ) {
        BIO_free(bi);
    }
    
    if ( ex != nullptr ) {
        const cc::Exception e = cc::Exception(*ex);
        delete  ex;
        throw e;
    }
}

// MARK: - [PRIVATE] - Base 64 helpers.

/**
 * @brief Decode a base 64 encoded string to a byte buffer.
 *
 * @param a_value  BASE 64 encoded string.
 * @param o_buffer Output buffer - caller must delete [] it when it's no longer needed.
 *
 * @return Output buffer size.
 */
size_t casper::openssl::P7::DecodeBase64 (const std::string& a_value, unsigned char** o_buffer)
{
    // ... sanity check ...
    if ( nullptr == o_buffer || (*o_buffer) != nullptr ) {
        throw cc::Exception("%s", "Invalid 'o_buffer' param!");
    }
    // ...
    size_t sz = 0;
    try {
        try {
            const size_t mds = cppcodec::base64_rfc4648::decoded_max_size(a_value.length());
            (*o_buffer)  = new unsigned char[mds];
            sz = cppcodec::base64_rfc4648::decode((*o_buffer) , mds, a_value.c_str(), a_value.length());
        } catch (const cppcodec::parse_error& a_parse_error) {
            const size_t mds = cppcodec::base64_url_unpadded::decoded_max_size(a_value.length());
            delete [] (*o_buffer) ;
            (*o_buffer)  = new unsigned char[mds];
            sz = cppcodec::base64_url_unpadded::decode((*o_buffer) , mds, a_value.c_str(), a_value.length());
        }
    } catch (...) {
        if ( nullptr != (*o_buffer)  ) {
            delete [] (*o_buffer) ;
        }
        (*o_buffer)  = nullptr;
        cc::Exception::Rethrow(/* a_unhandled */ true, __FILE__, __LINE__, __FUNCTION__);
    }
    // ... done ...
    return sz;
}

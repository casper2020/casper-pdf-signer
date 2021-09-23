/*
 * Copyright 2006-2020 The OpenSSL Project Authors. All Rights Reserved.
 *
 * Licensed under the OpenSSL license (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://www.openssl.org/source/license.html
 */

#include "crypto/ts/ts_rsp_sign.c"

ESS_SIGNING_CERT_V2 *casper_ess_signing_cert_v2_new_init(const EVP_MD *hash_alg, X509 *signcert, STACK_OF(X509) *certs)
{
    return ess_signing_cert_v2_new_init(hash_alg, signcert, certs);
}

int casper_ess_add_signing_cert_v2(PKCS7_SIGNER_INFO *si, ESS_SIGNING_CERT_V2 *sc)
{
    return ess_add_signing_cert_v2(si, sc);
}

/*******************************************************************************
*   (c) 2018 - 2023 Zondax AG
*
*  Licensed under the Apache License, Version 2.0 (the "License");
*  you may not use this file except in compliance with the License.
*  You may obtain a copy of the License at
*
*      http://www.apache.org/licenses/LICENSE-2.0
*
*  Unless required by applicable law or agreed to in writing, software
*  distributed under the License is distributed on an "AS IS" BASIS,
*  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*  See the License for the specific language governing permissions and
*  limitations under the License.
********************************************************************************/

#include "crypto.h"
#include "coin.h"
#include "cx.h"
#include "zxmacros.h"
#include "crypto_helper.h"
#include <zxformat.h>

uint32_t hdPath[HDPATH_LEN_DEFAULT];

typedef struct {
    uint8_t r[32];
    uint8_t s[32];
    uint8_t v;

    // DER signature max size should be 73
    // https://bitcoin.stackexchange.com/questions/77191/what-is-the-maximum-size-of-a-der-encoded-ecdsa-signature#77192
    uint8_t der_signature[73];

} __attribute__((packed)) signature_t;

zxerr_t crypto_extractPublicKey(uint8_t *pubkey, uint16_t pubkeyLen) {
    if (pubkey == NULL || pubkeyLen < SECP256K1_PUBKEY_LEN) {
        return zxerr_unknown;
    }
    cx_ecfp_public_key_t cx_publicKey = {0};
    cx_ecfp_private_key_t cx_privateKey = {0};
    uint8_t privateKeyData[64] = {0};

    volatile zxerr_t err = zxerr_unknown;
    // Generate keys
    CATCH_CXERROR(os_derive_bip32_with_seed_no_throw(
        HDW_NORMAL,
        CX_CURVE_256K1,
        hdPath,
        HDPATH_LEN_DEFAULT,
        privateKeyData,
        NULL,
        NULL,
        0))
    CATCH_CXERROR(cx_ecfp_init_private_key_no_throw(CX_CURVE_SECP256K1, privateKeyData, SECP256K1_PRIVKEY_LEN, &cx_privateKey))
    CATCH_CXERROR(cx_ecfp_init_public_key_no_throw(CX_CURVE_SECP256K1, NULL, 0, &cx_publicKey))
    CATCH_CXERROR(cx_ecfp_generate_pair_no_throw(CX_CURVE_SECP256K1, &cx_publicKey, &cx_privateKey, 1))
    MEMCPY(pubkey, cx_publicKey.W, SECP256K1_PUBKEY_LEN);
    err = zxerr_ok;

catch_cx_error:
    MEMZERO(&cx_privateKey, sizeof(cx_privateKey));
    MEMZERO(privateKeyData, sizeof(privateKeyData));
    return err;
}


zxerr_t crypto_sign(uint8_t *signature, uint32_t *signatureLength, uint16_t signatureMaxlen, const uint8_t *message, uint16_t messageLen) {
    if (signature == NULL || message == NULL || signatureMaxlen < sizeof(signature_t)) {
        return zxerr_unknown;
    }

    cx_sha3_t sha3 = {0};
    cx_ecfp_private_key_t cx_privateKey;
    uint8_t privateKeyData[64] = {0};
    uint8_t messageDigest[KECCAK_HASH_SIZE] = {0};
    unsigned int info = 0;
    signature_t *const signStruct = (signature_t *) signature;
    *signatureLength = sizeof(signStruct->der_signature);

    volatile zxerr_t err = zxerr_unknown;
    // Generate keys
    CATCH_CXERROR(os_derive_bip32_no_throw(CX_CURVE_SECP256K1,
                                           hdPath,
                                           HDPATH_LEN_DEFAULT,
                                           privateKeyData, NULL))

    CATCH_CXERROR(cx_ecfp_init_private_key_no_throw(CX_CURVE_SECP256K1, privateKeyData, SECP256K1_PRIVKEY_LEN, &cx_privateKey))

    // Hash the message
    CATCH_CXERROR(cx_keccak_init_no_throw(&sha3, KECCAK_256_SIZE))
    CATCH_CXERROR(cx_hash_no_throw((cx_hash_t*)&sha3, CX_LAST, message, messageLen, messageDigest, sizeof(messageDigest)))

    // Sign
    CATCH_CXERROR(cx_ecdsa_sign_no_throw(&cx_privateKey,
                                         CX_RND_RFC6979 | CX_LAST,
                                         CX_SHA256,
                                         messageDigest,
                                         sizeof(messageDigest),
                                         signStruct->der_signature,
                                         signatureLength,
                                         &info))

    err_convert_e convertErr = convertDERtoRSV(signStruct->der_signature, info,  signStruct->r, signStruct->s, &signStruct->v);
    if (convertErr == no_error) {
        // Return RSV + DER signature
        *signatureLength = sizeof_field(signature_t, r) + sizeof_field(signature_t, s) + sizeof_field(signature_t, v) + *signatureLength;
        err = zxerr_ok;
    }

catch_cx_error:
    MEMZERO(&cx_privateKey, sizeof(cx_privateKey));
    MEMZERO(privateKeyData, sizeof(privateKeyData));
    return err;
}

zxerr_t keccak_hash(const unsigned char *in, unsigned int inLen,
                    unsigned char *out, unsigned int outLen) {
    if (in == NULL || out == NULL || outLen < KECCAK_HASH_SIZE) {
         return zxerr_encoding_failed;
    }

    cx_sha3_t ctx;
    if (cx_keccak_init_no_throw(&ctx, KECCAK_256_SIZE) != CX_OK) {
        return zxerr_ledger_api_error;
    }

    if (cx_hash_no_throw((cx_hash_t *)&ctx, CX_LAST, in, inLen, out, outLen) != CX_OK) {
        return zxerr_ledger_api_error;
    }
    return zxerr_ok;
}

zxerr_t crypto_fillAddress(uint8_t *buffer, uint16_t bufferLen, uint16_t *addrResponseLen)
{
    if (bufferLen < SECP256K1_PUBKEY_LEN + ADDRESS_LEN) {
        return zxerr_unknown;
    }

    MEMZERO(buffer, bufferLen);
    CHECK_ZXERR(crypto_extractPublicKey(buffer, bufferLen))

    const uint8_t addrLen = crypto_encodePubkey(buffer + SECP256K1_PUBKEY_LEN,
                                 bufferLen - SECP256K1_PUBKEY_LEN,
                                 // Skip the prefix to generate the address
                                 buffer + 1);

    if (addrLen == 0) {
        MEMZERO(buffer, bufferLen);
        return zxerr_encoding_failed;
    }

    *addrResponseLen = SECP256K1_PUBKEY_LEN + addrLen;
    return zxerr_ok;
}

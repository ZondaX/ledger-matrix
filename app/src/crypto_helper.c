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
#include "crypto_helper.h"
#include "coin.h"
#include "base58.h"

zxerr_t keccak_hash(const unsigned char *in, unsigned int inLen,
                    unsigned char *out, unsigned int outLen);

// automatically generated LUT
// from https://github.com/MatrixAINetwork/go-matrix/blob/6b61d8dbb8dfde44e896d359b17377d1a60f44db/crc8/crc8.go#L26
// no reflect in or out
static const uint8_t crc8_init = 0x00;
static const uint8_t crc8_xor_out = 0x00;
// static const uint8_t crc8_check = 0xF4;  //Not used ?
static const uint8_t crc8_poly7[] = {
        0, 7, 14, 9, 28, 27, 18, 21, 56, 63, 54, 49, 36, 35, 42, 45, 112, 119, 126, 121, 108, 107, 98, 101, 72, 79, 70,
        65, 84, 83, 90, 93, 224, 231, 238, 233, 252, 251, 242, 245, 216, 223, 214, 209, 196, 195, 202, 205, 144, 151,
        158, 153, 140, 139, 130, 133, 168, 175, 166, 161, 180, 179, 186, 189, 199, 192, 201, 206, 219, 220, 213, 210,
        255, 248, 241, 246, 227, 228, 237, 234, 183, 176, 185, 190, 171, 172, 165, 162, 143, 136, 129, 134, 147, 148,
        157, 154, 39, 32, 41, 46, 59, 60, 53, 50, 31, 24, 17, 22, 3, 4, 13, 10, 87, 80, 89, 94, 75, 76, 69, 66, 111,
        104, 97, 102, 115, 116, 125, 122, 137, 142, 135, 128, 149, 146, 155, 156, 177, 182, 191, 184, 173, 170, 163,
        164, 249, 254, 247, 240, 229, 226, 235, 236, 193, 198, 207, 200, 221, 218, 211, 212, 105, 110, 103, 96, 117,
        114, 123, 124, 81, 86, 95, 88, 77, 74, 67, 68, 25, 30, 23, 16, 5, 2, 11, 12, 33, 38, 47, 40, 61, 58, 51, 52, 78,
        73, 64, 71, 82, 85, 92, 91, 118, 113, 120, 127, 106, 109, 100, 99, 62, 57, 48, 55, 34, 37, 44, 43, 6, 1, 8, 15,
        26, 29, 20, 19, 174, 169, 160, 167, 178, 181, 188, 187, 150, 145, 152, 159, 138, 141, 132, 131, 222, 217, 208,
        215, 194, 197, 204, 203, 230, 225, 232, 239, 250, 253, 244, 243};

uint8_t crc8(const uint8_t *data, size_t data_len) {
    uint8_t crc = crc8_init;
    for (size_t i = 0; i < data_len; i++) {
        crc = crc8_poly7[crc ^ data[i]];
    }
    return crc ^ crc8_xor_out;
}

uint8_t crypto_encodePubkey(uint8_t *buffer, uint16_t buffer_len, const uint8_t *pubkey) {
    // https://github.com/MatrixAINetwork/TxSend-Sign-Demos/blob/master/Address%20Format.md
    if (buffer == NULL || pubkey == NULL || buffer_len < ADDRESS_LEN) {
        return 0;
    }

    uint8_t pubkeyHash[KECCAK_HASH_SIZE] = {0};
    // We need to skip the prefix for Keccak256 hash
    if (keccak_hash(pubkey, SECP256K1_PUBKEY_LEN - 1, pubkeyHash, KECCAK_HASH_SIZE) != zxerr_ok) {
        return 0;
    }

    // ETH address is obtained with the last 20 bytes
    const uint8_t *ethAddress = pubkeyHash + 12;

    buffer[0] = 'M';
    buffer[1] = 'A';
    buffer[2] = 'N';
    buffer[3] = '.';

    uint8_t *addrPtr = buffer + 4;

    size_t outlen = 100;
    if (encode_base58(ethAddress, ETH_ADDRESS_LEN, (unsigned char *) addrPtr, &outlen) != 0) {
        return 0;
    }
    addrPtr += outlen;

    // Calculate CRC
    *addrPtr = (uint8_t) encode_base58_clip(crc8((uint8_t *) buffer, addrPtr - buffer));
    addrPtr++;

    // zero terminate and return
    *addrPtr = 0;
    return (uint8_t) (addrPtr - buffer);
}

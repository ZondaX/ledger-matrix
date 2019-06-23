/*******************************************************************************
*   (c) 2018 ZondaX GmbH
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

#include <cctype>
#include <stdexcept>
#include <cstring>
#include "test_utils.h"
#include "keccak.h"

uint8_t hex2dec(char c) {
    c = std::tolower(c);

    if (!std::isxdigit(c)) {
        throw std::invalid_argument("invalid hexdigit");
    }

    if (std::isdigit(c)) {
        return c - '0';
    }

    return c - 'a' + 10;
}

size_t parseHexString(const char *s, uint8_t *out) {
    size_t len = strlen(s);
    if (len % 2 == 1) {
        return 0;
    }

    for (int i = 0; i < len; i += 2) {
        out[i >> 1u] = (hex2dec(s[i]) << 4u) + hex2dec(s[i + 1]);
    }
    return len >> 1u;
};

// https://github.com/debris/tiny-keccak
// Parameters are based on
// https://github.com/ethereum/solidity/blob/6bbedab383f7c8799ef7bcf4cad2bb008a7fcf2c/libdevcore/Keccak256.cpp
void keccak(uint8_t *out, size_t out_len, uint8_t *in, size_t in_len) {
    keccak_hash(out, out_len, in, in_len, 136, 0x01);
}

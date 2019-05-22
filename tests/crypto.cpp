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

#include "gmock/gmock.h"

#include <cstdio>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <array>
#include <cctype>

#include "lib/base58.h"
#include "lib/crypto.h"
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

//////////////////////////
//////////////////////////
//////////////////////////
//////////////////////////
//////////////////////////
//////////////////////////
//////////////////////////
//////////////////////////
//////////////////////////

TEST(crypto, parseHexString) {
    char s[] = "1234567890";
    uint8_t data[100];

    auto length = parseHexString(s, data);

    ASSERT_THAT(length, testing::Eq(5));

    ASSERT_THAT(data[0], testing::Eq(0x12));
    ASSERT_THAT(data[1], testing::Eq(0x34));
    ASSERT_THAT(data[2], testing::Eq(0x56));
    ASSERT_THAT(data[3], testing::Eq(0x78));
    ASSERT_THAT(data[4], testing::Eq(0x90));
}

TEST(crypto, parseHexString2) {
    char s[] = "be333be7ee";
    uint8_t data[100];

    auto length = parseHexString(s, data);

    ASSERT_THAT(length, testing::Eq(5));

    ASSERT_THAT(data[0], testing::Eq(0xbe));
    ASSERT_THAT(data[1], testing::Eq(0x33));
    ASSERT_THAT(data[2], testing::Eq(0x3b));
    ASSERT_THAT(data[3], testing::Eq(0xe7));
    ASSERT_THAT(data[4], testing::Eq(0xee));
}

TEST(crypto, base58encoder) {
    uint8_t data[] = {1, 2, 6, 4, 5, 6};
    uint8_t out[100];
    memset(out, 1, sizeof(out));
    size_t outlen = sizeof(out);

    // Encode and zero terminate
    encode_base58(data, sizeof(data), out, &outlen);
    out[outlen] = 0;

    EXPECT_THAT(outlen, testing::Eq(7));
    EXPECT_THAT((char *) out, testing::StrEq("W7R4Rp9"));
}

TEST(crypto, base58encoder2) {
    uint8_t data[20];
    parseHexString("be333be7ee87c31f73d7359dd6228270845c563a", data);

    uint8_t out[40];
    size_t outlen = 40;

    // Encode and zero terminate
    encode_base58(data, sizeof(data), out, &outlen);
    out[outlen] = 0;

    EXPECT_THAT(outlen, testing::Eq(28));
    EXPECT_THAT((char *) out, testing::StrEq("3egxFNEMZLVGA6fSLibszJMDUQSV"));
}

TEST(crypto, crc8) {
    uint8_t data[] = {'h', 'e', 'l', 'l', 'o', '!'};

    EXPECT_THAT(crc8(data, 0), testing::Eq(0));
    EXPECT_THAT(crc8(data, 1), testing::Eq(31));
    EXPECT_THAT(crc8(data, 2), testing::Eq(97));
    EXPECT_THAT(crc8(data, 3), testing::Eq(35));
    EXPECT_THAT(crc8(data, 4), testing::Eq(234));
    EXPECT_THAT(crc8(data, 5), testing::Eq(146));
    EXPECT_THAT(crc8(data, 6), testing::Eq(16));
}

TEST(crypto, keccak) {
    uint8_t data[] = {'h', 'e', 'l', 'l', 'o', '!'};
    uint8_t hash[32];
    uint8_t expected[32];

    const char *s = "96b8d442f4c09a08d266bf37b18219465cfb341c1b3ab9792a6103a93583fdf7";
    parseHexString(s, expected);

    keccak(hash, sizeof(hash), data, sizeof(data));

    for(int i=0; i<32; i++){
        EXPECT_THAT(hash[i], testing::Eq(expected[i]));
    }
}

TEST(crypto, ethAddress) {
    uint8_t pubKey[64];
    uint8_t expectedAddr[20];
    uint8_t ethAddr[20];

    parseHexString("bf3888e9b2ef0b7b22498c01ead61485da023675502b098c764be22d585d379a"
                   "8cbf57f818d7b0154ccce67916bf752d65b27632ea9c4f9ce7ecb661ffd05945",
                   pubKey);
    parseHexString("be333be7ee87c31f73d7359dd6228270845c563a", expectedAddr);

    ethAddressFromPubKey(ethAddr, pubKey);

    for(int i=0; i<20; i++){
        EXPECT_THAT(ethAddr[i], testing::Eq(expectedAddr[i]));
    }
}

TEST(crypto, manAddress) {
    uint8_t ethAddr[20];
    parseHexString("be333be7ee87c31f73d7359dd6228270845c563a", ethAddr);

    char addr[100];
    uint8_t address_len = manAddressFromEthAddr(addr, ethAddr);

    EXPECT_THAT(address_len, testing::Eq(33));
    EXPECT_THAT(strlen(addr), testing::Eq(33));
    EXPECT_THAT((char *) addr, testing::StrEq("MAN.3egxFNEMZLVGA6fSLibszJMDUQSVf"));
}

/*******************************************************************************
*   (c) 2018 - 2022 Zondax AG
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

// #{TODO} --> Apply tests that check this app's encoding/libraries

#include "gmock/gmock.h"

#include <vector>
#include <iostream>
#include <hexutils.h>
#include "parser_txdef.h"
#include "parser.h"
#include "parser_impl.h"
#include "crypto_helper.h"
#include "keccak.h"
#include "coin.h"
#include "base58.h"

using namespace std;

struct AddressTestcase {
        string pubkey;
        string ethAddress;
        string manAddress;
};

TEST(crypto, crc8) {
    uint8_t data[] = {'h', 'e', 'l', 'l', 'o', '!'};

    EXPECT_THAT(crc8(data, 0), testing::Eq(0x00));
    EXPECT_THAT(crc8(data, 1), testing::Eq(0x1F));
    EXPECT_THAT(crc8(data, 2), testing::Eq(0x61));
    EXPECT_THAT(crc8(data, 3), testing::Eq(0x23));
    EXPECT_THAT(crc8(data, 4), testing::Eq(0xEA));
    EXPECT_THAT(crc8(data, 5), testing::Eq(0x92));
    EXPECT_THAT(crc8(data, 6), testing::Eq(0x10));
}

TEST(crypto, keccak) {
    uint8_t data[] = {'h', 'e', 'l', 'l', 'o', '!'};
    uint8_t hash[32];
    uint8_t expected[32];

    const char *s = "96b8d442f4c09a08d266bf37b18219465cfb341c1b3ab9792a6103a93583fdf7";
    parseHexString(expected, sizeof(expected), s);

    // keccak_hash(hash, sizeof(hash), data, sizeof(data), 136, 0x01);
    keccak_hash(data, sizeof(data), hash, sizeof(hash));

    for (int i = 0; i < 32; i++) {
        EXPECT_THAT(hash[i], testing::Eq(expected[i]));
    }
}

TEST(Address, MatrixAIAddressTests) {
        vector<AddressTestcase> addresses {
                {   "b9a1fdb8a1324712faee3d3f920a78c681683b5750f062a14e27674791180932e9c612e857185a522457c599476cc4c76c1c5168498dac3cfef94769e5a7a21f",
                    "f5141687294ab97269b780b1e0373b1514923140",
                    "MAN.4R2tdcPR6Sdv28Nz4627M6Hf2ko1J"},
                {   "8d8f665363ab86f53333f19b0d51ec4ead1b8a35db82475a61d121cad747bda3cf467a173edab99066a8e0c9da68ca617d9d8d5f97150fb4864a26d80b127d68",
                    "160174f2cdbd278cc0cc628039b2f6e878ebc5af",
                    "MAN.JnKi1JoxAsekLEHi4PoS9PJfXhgj"},
                {   "f3ac945fadef1d8192ef6e6261bca03974c726f3811e16b04923ea431486623c7fa7e054683450609b5923acddcd7520fea191e457961fc409f5fde609af0c78",
                    "46567c6175c93318dbc5077c8a7ed4a6967582fe",
                    "MAN.yqTz5KWRZ78gk5BtZicdD2iVh8yv"},
                {   "9a8c338c9d79230a0228f3fbd8fafd3e5bc8dab1e56e0ab2e67d6d7ad2731ee08c83b9d91fb832f8904c651a69ad98667311fc571f5c8e0c1deaf7704e5f57b4",
                    "6b4aa7f991505565f29af014e5cb008747cf6040",
                    "MAN.2VhMBhbkbUX4RUkCwXdpMBpz7YsqM"},
                {   "7893e8dedf05d29528ae9befad268dca11eb31f2d5b6777432459a441346ab5997b27357ad0da31097a9296e3aeec69da8d7e94657b9c1d88b3774b40f074e5a",
                    "0a158def2f47a5d175b93263476385bd6c4de84f",
                    "MAN.99cFQtVohoV2jEH9RHpvxvA6oU2e"},
                {   "198a43efe8753f8d2e10a51501b19e1915a39f3bdc24a67963e51a92dd2cc5e48df208bd788b14d152f7b49456b0c2134a7aee9ba5917ed9501fcc31d8764d3d",
                    "dc8fddc105a10fb36508a12ae16692999e8d9a23",
                    "MAN.45DtyHgbhCHvhjrwYC3koxnDV9zSw"},
                {   "9e4988f87433a0978faa2434a2426a70d6f57ec3070c74e80274c378d0b450c3bdd0173f7353b8d562aa770a22134d5ac7fe67fb1a74e0f717698a584459ffb4",
                    "de2d93e898d1587b2d44fa02bae0fdd29e06836c",
                    "MAN.46Xdot1NEbXH4aVbWCHNPG2egWGjf"},
                {   "c256c163c67f5da8fbcb8402ede0b1a6ad5dc3c404561e20afb6b21640ace8149b151af6cf1a3ead018844ee30128106b57f9b979a208df3a7ad09c81354a50f",
                    "49899b440489575fc9b2e537a43a56a0f0fae5ac",
                    "MAN.22RRWCHnxwLTmnoL8qBZoSZwsBSjs"},
                {   "e44d742e5f5b1218d24b896c458e4510540f6dbac90d89ea51139676f6d0801411f89fc1e40be376d1cecf1ca6788b0d1ce35d55a5c7d01e71bbb73c408ff264",
                    "4a925f462fbf2f44d5b736794c7096b740620721",
                    "MAN.23FtpLr2Ex6fHMBJX1rWPdK6xWT2w"},
                {   "827d434bf57fce3eeab62df8cec6931f29251c7b52afee56992a2b61d9ef1a62f71af58595e35df02e39995f951f1c38d943ce485193c7897d214d1962039bbc",
                    "88c861b6875d31458abc9cb32142a213fb83ad1d",
                    "MAN.2uXV4MumUxkJT271hmgy3KngRPn45"},
                {   "34c46fac735d0da4ed321168542ca774766b0ce7f180c0dfa4aa070b00a9cafd94b7510ad3307899cdf2f646eac19e86bd29b2e320b056b94a9fa70bc04e6a59",
                    "424125c4dccc4353b4a4b2c2b2efb95abefc7816",
                    "MAN.vY6TcL95jBqRadapzZTeH79d7DfG"},
                {   "b237ce0918eb97d7517ea97f688b91f3181647bb7fe4bd2d9b22577ca38ead775874584d8a77abbaa7584dc44844f8eadd985229e33210e79504daff01744d0c",
                    "687cf8a94036a54e02f35ec21374388c3ff2377b",
                    "MAN.2TRxiv83S1hHCocx4JDHAF5vge2em"},
                {   "c4ff5098d27a64be9e5048275295644beb7feea09c22da4b3cd4dad6e49f931082a858829ba3cb89e3b048b574e94ae6736dd47c1dbfb01dab773c6a79182189",
                    "69caa94a556c6cdd19db8a7e8950798724d01a66",
                    "MAN.2UV3t9hPCK4NTeDSgZcQjKw753DfR"},
                {   "1bbc26700c6308d99d4f442f9645ee89e4202dc008de815ef347a66872b7664caf7f2d12f34187cf7eebd695941fc3370aec204f4b2e9aa8e774dba993ead711",
                    "885275ab8884215828b73151a4cd4f175581993e",
                    "MAN.2u9twyUyEvUgegEpcs4rkBE2p2aRN"},
                {   "e5e716b95dcef3924947667d7c02e9e34190b69f01d285c2cd35b4b0ecac4c644e6d1bcdb20306637499eca6dcee40664df692e47049b22ff3cf1ea110798805",
                    "f89fa1b42ee9cbee979b90eeeae023400a282b6a",
                    "MAN.4Tu337dFNCAjxv6yiN6zeHrjexhPG"},
                {   "91c5822f1e8e096d5834c19f53933d9e1d9c653a52c7b7f27e35a202bb4d7d7585f3fdd3d697185b9cd78a5d571281d7d96225042aa4bf26fec7b32d130416e7",
                    "2be694c5b9a87f7287c305e6b8dc757608f2ad04",
                    "MAN.cUTaQZsmCAdpshzWnFiatff8QZHv"},
        };

            // parseHexString(pubKey, sizeof(pubKey),
            // "bf3888e9b2ef0b7b22498c01ead61485da023675502b098c764be22d585d379a"
            // "8cbf57f818d7b0154ccce67916bf752d65b27632ea9c4f9ce7ecb661ffd05945");

            // a7f6857e5d36c2b080953a74be333be7ee87c31f73d7359dd6228270845c563a
            //                         be333be7ee87c31f73d7359dd6228270845c563a

        for (const auto& testcase : addresses) {
                uint8_t pubkey[100] = {0};
                parseHexString(pubkey, sizeof(pubkey), testcase.pubkey.c_str());

                // Verify ETH address
                uint8_t pubkeyHash[KECCAK_HASH_SIZE] = {0};
                keccak_hash(pubkey, (SECP256K1_PUBKEY_LEN - 1), pubkeyHash, KECCAK_HASH_SIZE);
                const uint8_t *ethAddressPtr = pubkeyHash + 12;

                uint8_t expectedEthAddr[ETH_ADDRESS_LEN] = {0};
                parseHexString(expectedEthAddr, sizeof(expectedEthAddr), testcase.ethAddress.c_str());

                for (int i = 0; i < ETH_ADDRESS_LEN; i++) {
                    EXPECT_THAT(*(ethAddressPtr + i), testing::Eq(expectedEthAddr[i]));
                }

                // Verify MAN address
                uint8_t actualAddress[ADDRESS_LEN] = {0};
                const uint8_t address_len = crypto_encodePubkey(actualAddress, sizeof(actualAddress), pubkey);

                const string matrix_address(actualAddress, actualAddress + address_len);
                EXPECT_EQ(matrix_address, testcase.manAddress);

                // Check CRC
                const char crcByte = (uint8_t) encode_base58_clip(crc8((uint8_t *) actualAddress, address_len - 1));
                EXPECT_THAT(actualAddress[address_len - 1], testing::Eq(crcByte));
        }
}

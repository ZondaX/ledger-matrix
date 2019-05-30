/*******************************************************************************
*   (c) 2019 ZondaX GmbH
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
#include <cctype>

#include "lib/mantx.h"
#include "lib/uint256.h"
#include "utils.h"

using ::testing::TestWithParam;
using ::testing::Values;

struct MANTxTestCase {
    const char *name;
    const char *data;
//    uint16_t expectedFieldCount;
//    const char *expectedFields;
//    const char *expectedValueStr;
};

class MANTxParamTest : public testing::TestWithParam<MANTxTestCase> {
public:
    struct PrintToStringParamName {
        std::string operator()(const ::testing::TestParamInfo<MANTxTestCase> &info) const {
            std::stringstream ss;
            ss << info.index << "_" << info.param.name;
            return ss.str();
        }
    };
};

INSTANTIATE_TEST_SUITE_P(
    InstantiationName,
    MANTxParamTest,
    Values(
        MANTxTestCase{"exampleTx",
                      "f901e1"  // Root List
                      "80" // Nonce
                      "850430e23400" // Gas Price
                      "83033450" // Gas Limit
                      "a04d414e2e576b62756a7478683759426e6b475638485a767950514b3363415079" // To
                      "83989680" // Value
                      "b9019d5b7b22456e7472757374416464726573223a224d414e2e32556f7a3867386a61754d61326d746e777872"
                      "7363686a3271504a7245222c224973456e7472757374476173223a747275652c224973456e7472757374536967"
                      "6e223a66616c73652c225374617274486569676874223a313232322c22456e64486569676874223a3132323232"
                      "322c22456e73747275737453657454797065223a302c22757365537461727454696d65223a22222c2275736545"
                      "6e6454696d65223a22222c22456e7472757374436f756e74223a307d2c7b22456e747275737441646472657322"
                      "3a224d414e2e32556f7a3867386a61754d61326d746e7778727363686a3271504a7245222c224973456e747275"
                      "7374476173223a747275652c224973456e74727573745369676e223a66616c73652c2253746172744865696768"
                      "74223a3132323232332c22456e64486569676874223a3132323232392c22456e73747275737453657454797065"
                      "223a302c22757365537461727454696d65223a22222c22757365456e6454696d65223a22222c22456e74727573"
                      "74436f756e74223a307d5d"
                      "03" // ChainID
                      "80" // R
                      "80" // S
                      "80" // Enter Type
                      "80" // IsEntrust
                      "86016850894a0f" // Commit Time
                      "c4c30580c0"
        }
    ),
    MANTxParamTest::PrintToStringParamName()
);

TEST(MANTxParamTest, PrintUInt256) {
    uint256_t a;

    shiftl256(&a, 5, &a);

    char out[100];
    tostring256(&a, 10, out, 100);
    std::cout << out << std::endl;
}

// TODO: Add testcase for missing field

TEST_P(MANTxParamTest, stream) {
    auto params = GetParam();

    uint8_t data[10000];
    uint16_t dataSize = parseHexString(params.data, data);

    mantx_context_t ctx;
    auto err = mantx_parse(&ctx, data, dataSize);
    EXPECT_THAT(err, testing::Eq(MANTX_NO_ERROR));

    std::cout << std::endl;

    char out[1000];
    mantx_print(&ctx, data, MANTX_FIELD_TO, out, 1000);
    std::cout << maxtx_getFieldName(MANTX_FIELD_TO) << " " << out << std::endl;
}

TEST_P(MANTxParamTest, iterateDisplayStream) {
    auto params = GetParam();

    uint8_t data[10000];
    uint16_t dataSize = parseHexString(params.data, data);

    mantx_context_t ctx;
    auto err = mantx_parse(&ctx, data, dataSize);
    EXPECT_THAT(err, testing::Eq(MANTX_NO_ERROR));

    std::cout << std::endl;

    char displayKey[1000];
    char displayValue[1000];

    for (int8_t displayIdx = 0; displayIdx < 13; displayIdx++) {
        err = mantx_getItem(&ctx, data, displayIdx, displayKey, 1000, displayValue, 1000);
        if (err == MANTX_NO_ERROR) {
            std::cout << displayKey << " = " << displayValue << std::endl;
        }
    }
}

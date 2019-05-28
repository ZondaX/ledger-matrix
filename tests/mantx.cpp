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
                      "f901e180850430e2340083033450a04d414e2e576b62756a7478683759426e6b475638485a767950514b33634"
                      "1507983989680b9019d5b7b22456e7472757374416464726573223a224d414e2e32556f7a3867386a61754d61"
                      "326d746e7778727363686a3271504a7245222c224973456e7472757374476173223a747275652c224973456e7"
                      "4727573745369676e223a66616c73652c225374617274486569676874223a313232322c22456e644865696768"
                      "74223a3132323232322c22456e73747275737453657454797065223a302c22757365537461727454696d65223"
                      "a22222c22757365456e6454696d65223a22222c22456e7472757374436f756e74223a307d2c7b22456e747275"
                      "7374416464726573223a224d414e2e32556f7a3867386a61754d61326d746e7778727363686a3271504a72452"
                      "22c224973456e7472757374476173223a747275652c224973456e74727573745369676e223a66616c73652c22"
                      "5374617274486569676874223a3132323232332c22456e64486569676874223a3132323232392c22456e73747"
                      "275737453657454797065223a302c22757365537461727454696d65223a22222c22757365456e6454696d6522"
                      "3a22222c22456e7472757374436f756e74223a307d5d038080808086016850894a0fc4c30580c0"
        }
    ),
    MANTxParamTest::PrintToStringParamName()
);

TEST_P(MANTxParamTest, stream) {
    auto params = GetParam();

    uint8_t data[10000];
    uint16_t dataSize = parseHexString(params.data, data);

    mantx_context_t ctx;
    auto err = mantx_parse(&ctx, data, dataSize);
    EXPECT_THAT(err, testing::Eq(MANTX_NO_ERROR));
}

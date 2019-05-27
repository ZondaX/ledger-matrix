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

#include "lib/rlp.h"
#include "utils.h"

using ::testing::TestWithParam;
using ::testing::Values;

struct RLPValueTestCase {
    const char *data;
    uint8_t expectedKind;
    uint64_t expectedLen;
    uint64_t expectedDataOffset;
    uint64_t expectedConsumed;
};

class RLPDecodeTest : public testing::TestWithParam<RLPValueTestCase> {
public:
    void SetUp() override {}

    void TearDown() override {}
};

INSTANTIATE_TEST_SUITE_P(
        InstantiationName,
        RLPDecodeTest,
        Values(
                RLPValueTestCase{"00", RLP_KIND_BYTE, 0, 0, 1},
                RLPValueTestCase{"01", RLP_KIND_BYTE, 0, 0, 1},
                RLPValueTestCase{"7F", RLP_KIND_BYTE, 0, 0, 1},

                RLPValueTestCase{"80", RLP_KIND_STRING, 0, 1, 1},
                RLPValueTestCase{"B7", RLP_KIND_STRING, 55, 1, 56},
                RLPValueTestCase{"B90400", RLP_KIND_STRING, 1024, 3, 1027},

                RLPValueTestCase{"C0", RLP_KIND_LIST, 0, 1, 1},
                RLPValueTestCase{"C8", RLP_KIND_LIST, 8, 1, 9},
                RLPValueTestCase{"F7", RLP_KIND_LIST, 55, 1, 56},
                RLPValueTestCase{"F90400", RLP_KIND_LIST, 1024, 3, 1027}
        )
);

TEST_P(RLPDecodeTest, decodeElement) {
    auto params = GetParam();

    uint8_t data[100];
    parseHexString(params.data, data);

    uint8_t kind;
    uint64_t len;
    uint64_t dataOffset;

    auto bytesConsumed = rlp_decode(data, &kind, &len, &dataOffset);
    EXPECT_THAT(kind, testing::Eq(params.expectedKind));
    EXPECT_THAT(len, testing::Eq(params.expectedLen));
    EXPECT_THAT(dataOffset, testing::Eq(params.expectedDataOffset));
    EXPECT_THAT(bytesConsumed, testing::Eq(params.expectedConsumed));
}

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////

struct RLPStreamTestCase {
    const char *data;
    uint8_t expected;
};

class RLPStreamTest : public testing::TestWithParam<RLPStreamTestCase> {
public:
    void SetUp() override {}

    void TearDown() override {}
};

INSTANTIATE_TEST_SUITE_P(
        InstantiationName,
        RLPStreamTest,
        Values(
                RLPStreamTestCase{"05", 5},
                RLPStreamTestCase{"80", 0},
                RLPStreamTestCase{"820505", 0},
                RLPStreamTestCase{"83050505", 0},
                RLPStreamTestCase{"8405050505", 0},
                RLPStreamTestCase{"850505050505", 0},

                RLPStreamTestCase{"8D6162636465666768696A6B6C6D", 0},
                RLPStreamTestCase{"C50583343434", 0},
                RLPStreamTestCase{"C601C402C203C0", 0},
                RLPStreamTestCase{
                        "f901e180850430e2340083033450a04d414e2e576b62756a7478683759426e6b475638485a767950514b336341507983989680b9019d5b7b22456e7472757374416464726573223a224d414e2e32556f7a3867386a61754d61326d746e7778727363686a3271504a7245222c224973456e7472757374476173223a747275652c224973456e74727573745369676e223a66616c73652c225374617274486569676874223a313232322c22456e64486569676874223a3132323232322c22456e73747275737453657454797065223a302c22757365537461727454696d65223a22222c22757365456e6454696d65223a22222c22456e7472757374436f756e74223a307d2c7b22456e7472757374416464726573223a224d414e2e32556f7a3867386a61754d61326d746e7778727363686a3271504a7245222c224973456e7472757374476173223a747275652c224973456e74727573745369676e223a66616c73652c225374617274486569676874223a3132323232332c22456e64486569676874223a3132323232392c22456e73747275737453657454797065223a302c22757365537461727454696d65223a22222c22757365456e6454696d65223a22222c22456e7472757374436f756e74223a307d5d038080808086016850894a0fc4c30580c0",
                        0}

        )
);

//01 {name: 'nonce', length: 32, allowLess: true, index: 0, default: <Buffer>},
//02 {name: 'gasPrice',length: 32,allowLess: true,index: 1,default: <Buffer>},
//03 {name: 'gasLimit',alias: 'gas',length: 32,allowLess: true,index: 2,default: <Buffer>},
//04 {name: 'to', allowZero: true, index: 3, default: '' },
//05 {name: 'value',length: 32,allowLess: true,index: 4,default: <Buffer>},
//06 {name: 'data',alias: 'input',allowZero: true,index: 5,default: <Buffer>},
//07 {name: 'v', allowZero: true, index: 6, default: <Buffer 1c> },
//08 {name: 'r',length: 32,allowZero: true,allowLess: true,index: 7,default: <Buffer>},
//09 {name: 's',length: 32,allowZero: true,allowLess: true,index: 8,default: <Buffer>},
//10 {name: 'TxEnterType',allowZero: true,allowLess: true,index: 9,default: <Buffer>},
//11 {name: 'IsEntrustTx',allowZero: true,allowLess: true,index: 10,default: <Buffer>},
//12 {name: 'CommitTime',allowZero: true,allowLess: true,index: 11,default: <Buffer>},
//13 {name: 'extra_to',allowZero: true,allowLess: true,index: 12,default: <Buffer>}

void consumeStream(const uint8_t *data, uint64_t dataLen, uint8_t depth) {
    uint64_t offset = 0;

    while (offset < dataLen) {
        uint8_t kind;
        uint64_t len;
        uint64_t dataOffset;
        auto bytesConsumed = rlp_decode(data + offset, &kind, &len, &dataOffset);

        for (auto i = 0; i < depth; i++) {
            std::cout << "  ";
        }

        switch (kind) {
            case RLP_KIND_BYTE:
                std::cout << "BYTE   " << dataOffset << " " << len << std::endl;
                break;
            case RLP_KIND_STRING:
                std::cout << "STRING " << dataOffset << " " << len << std::endl;
                break;
            case RLP_KIND_LIST:
                std::cout << "LIST   " << dataOffset << " " << len << std::endl;
                consumeStream(data + dataOffset, len, depth + 1);
                break;
        }

        offset += bytesConsumed;
    }

}

TEST_P(RLPStreamTest, stream) {
    auto params = GetParam();

    uint8_t data[10000];
    uint64_t dataSize = parseHexString(params.data, data);

    std::cout << std::endl;

    consumeStream(data, dataSize, 0);
}

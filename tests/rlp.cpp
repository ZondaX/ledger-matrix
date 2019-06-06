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
    uint16_t len;
    uint16_t dataOffset;

    auto bytesConsumed = rlp_decode(data, &kind, &len, &dataOffset);
    EXPECT_THAT(kind, testing::Eq(params.expectedKind));
    EXPECT_THAT(len, testing::Eq(params.expectedLen));
    EXPECT_THAT(dataOffset, testing::Eq(params.expectedDataOffset));
    EXPECT_THAT(bytesConsumed, testing::Eq(params.expectedConsumed));
}

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////

struct RLPStreamTestCase {
    const char *name;
    const char *data;
    uint16_t expectedFieldCount;
    const char *expectedFields;
    const char *expectedValueStr;
};

class RLPStreamParamTest : public testing::TestWithParam<RLPStreamTestCase> {
public:
    struct PrintToStringParamName {
        std::string operator()(const ::testing::TestParamInfo<RLPStreamTestCase> &info) const {
            std::stringstream ss;
            ss << info.index << "_" << info.param.name;
            return ss.str();
        }
    };
};

INSTANTIATE_TEST_SUITE_P(
    InstantiationName,
    RLPStreamParamTest,
    Values(
        RLPStreamTestCase{"byte_5", "05", 1, "BYTE@0[0|0]", "BYTE=5"},
        RLPStreamTestCase{"byte_6", "06", 1, "BYTE@0[0|0]", "BYTE=6"},
        RLPStreamTestCase{"string_empty", "80", 1, "STRING@0[1|0]", "STRING="},
        RLPStreamTestCase{"string", "820505", 1, "STRING@0[1|2]", "STRING=\x5\x5"},
        RLPStreamTestCase{"string", "83050505", 1, "STRING@0[1|3]", "STRING=\x5\x5\x5"},
        RLPStreamTestCase{"string", "8405050505", 1, "STRING@0[1|4]", "STRING=\x5\x5\x5\x5"},
        RLPStreamTestCase{"string", "850505050505", 1, "STRING@0[1|5]", "STRING=\x5\x5\x5\x5\x5"},
        RLPStreamTestCase{"string", "8D6162636465666768696A6B6C6D", 1, "STRING@0[1|13]", "STRING=abcdefghijklm"},
        RLPStreamTestCase{"string", "820505820505", 2, "STRING@0[1|2]STRING@3[1|2]", "STRING=\x5\x5"},
        // [5, '444']
        RLPStreamTestCase{"list", "C50583343434", 1, "LIST@0[1|5]", "LIST=BYTE@1[0|0]STRING@2[1|3]"},
        // [1, [2, [3, ]]]
        RLPStreamTestCase{"list", "C601C402C203C0", 1, "LIST@0[1|6]", "LIST=BYTE@1[0|0]LIST@2[1|4]"},
        // matrix tx
        RLPStreamTestCase{"list",
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
                          "3a22222c22456e7472757374436f756e74223a307d5d038080808086016850894a0fc4c30580c0",
                          1,
                          "LIST@0[3|481]",
                          "LIST="
                          "STRING@3[1|0]"
                          "STRING@4[1|5]"
                          "STRING@10[1|3]"
                          "STRING@14[1|32]"
                          "STRING@47[1|3]"
                          "STRING@51[3|413]"
                          "BYTE@467[0|0]"
                          "STRING@468[1|0]"
                          "STRING@469[1|0]"
                          "STRING@470[1|0]"
                          "STRING@471[1|0]"
                          "STRING@472[1|6]"
                          "LIST@479[1|4]"}
    ),
    RLPStreamParamTest::PrintToStringParamName()
);

std::string getKind(uint8_t kind) {
    switch (kind) {
        case RLP_KIND_BYTE:
            return "BYTE";
        case RLP_KIND_STRING:
            return "STRING";
        case RLP_KIND_LIST:
            return "LIST";
        default:
            return "?????";
    }
}

std::string dumpRLPFields(rlp_field_t *fields, uint8_t fieldCount) {
    std::stringstream ss;
    for (int i = 0; i < fieldCount; i++) {
        rlp_field_t *f = fields + i;
        ss << getKind(f->kind) << "@";
        ss << f->fieldOffset << "[" << f->valueOffset << "|"  << f->valueLen << "]";
    }
    return ss.str();
};

TEST_P(RLPStreamParamTest, stream) {
    auto params = GetParam();

    uint8_t data[10000];
    uint64_t dataSize = parseHexString(params.data, data);

    rlp_field_t fields[32];
    uint16_t fieldCount;
    auto err = rlp_parseStream(data, 0, dataSize, fields, 32, &fieldCount);
    EXPECT_THAT(err, testing::Eq(RLP_NO_ERROR));
    EXPECT_THAT(fieldCount, testing::Eq(params.expectedFieldCount));

    auto s = dumpRLPFields(fields, fieldCount);
    std::cout << s << std::endl;
    EXPECT_THAT(s, testing::Eq(params.expectedFields));
}

TEST_P(RLPStreamParamTest, streamReadValues) {
    auto params = GetParam();

    uint8_t data[10000];
    uint64_t dataSize = parseHexString(params.data, data);

    rlp_field_t fields[32];
    uint16_t fieldCount;
    auto err = rlp_parseStream(data, 0, dataSize, fields, 32, &fieldCount);
    EXPECT_THAT(err, testing::Eq(RLP_NO_ERROR));
    EXPECT_THAT(fieldCount, testing::Eq(params.expectedFieldCount));

    for (int i = 0; i < fieldCount; i++) {
        std::stringstream ss;
        auto currentField = fields + i;

        ss << getKind(fields[i].kind) << "=";

        switch (fields[i].kind) {
            case RLP_KIND_BYTE: {
                uint8_t value;
                err = rlp_readByte(data, currentField, &value);
                EXPECT_THAT(err, testing::Eq(RLP_NO_ERROR));
                ss << (int) value;
                break;
            }
            case RLP_KIND_STRING: {
                char value[1000];
                err = rlp_readString(data, currentField, value, sizeof(value));
                EXPECT_THAT(err, testing::Eq(RLP_NO_ERROR));
                ss << (char *) value;
                break;
            }
            case RLP_KIND_LIST: {
                // When a list is found, we can use the value as a stream again
                // no need to extract the data
                rlp_field_t listFields[32];
                uint16_t listFieldCount;

                err = rlp_readList(data, currentField, listFields, 32, &listFieldCount);
                EXPECT_THAT(err, testing::Eq(RLP_NO_ERROR));
                EXPECT_THAT(fieldCount, testing::Eq(params.expectedFieldCount));

                ss << dumpRLPFields(listFields, listFieldCount);
                break;
            }
        }

        EXPECT_THAT(ss.str(), testing::Eq(params.expectedValueStr));
    }
}

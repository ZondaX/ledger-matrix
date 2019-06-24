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
#include "test_utils.h"

using ::testing::TestWithParam;
using ::testing::Values;

struct MANTxTestCase {
    const char *name;
    const char *data;
    const char *expectedStr;
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
        MANTxTestCase{
            "Example1",
            "f8498710000000000040850430e23400825208a14d414e2e32556f7a3867386a"
            "61754d61326d746e7778727363686a3271504a72458398968080038080808086016850894a0f"
            "c4c38080c0",
            ////
            "[0:0]  Nonce = 4503599627370560\n"
            "[1:0]  Gas Price = 18000000000\n"
            "[2:0]  Gas Limit = 21000\n"
            "[3:0]  To = MAN.2Uoz8g8jauMa2mtnwxrschj2qPJrE\n"
            "[4:0]  Value = 10000000\n"
            "[6:0]  ChainID = 3\n"
            "[7:0]  EnterType = 0\n"
            "[8:0]  IsEntrustTx = 0\n"
            "[9:0]  CommitTime = 15Jan2019 08:03:21\n"
            "[10:0]  TxType = Normal\n"
            "[11:0]  LockHeight = 0\n"},
        /////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////
        MANTxTestCase{
            "Example2",
            "f8b78710000000000041850430e2340083033450a04d414e2e576b62756a747868375942"
            "6e6b475638485a767950514b336341507980b8705b7b22456e747275737441646"
            "4726573223a224d414e2e3661706346595162595a68774c5a7a33626234546a666b67346d794a2"
            "22c224973456e7472757374476173223a747275652c22456e73747275737453657454797065223"
            "a322c22456e7472757374436f756e74223a32307d5d038080808086016850894a0fc4c30580c0",
            ///////
            "[0:0]  Nonce = 4503599627370561\n"
            "[1:0]  Gas Price = 18000000000\n"
            "[2:0]  Gas Limit = 210000\n"
            "[3:0]  To = MAN.Wkbujtxh7YBnkGV8HZvyPQK3cAPy\n"
            "[4:0]  Value = 0\n"
            "[5:0]  Data = [{\"EntrustAddres\":\"MAN.6apcFYQbYZhwLZz3\n"
            "[5:1]  Data = bb4Tjfkg4myJ\",\"IsEntrustGas\":true,\"Enst\n"
            "[5:2]  Data = rustSetType\":2,\"EntrustCount\":20}]\n"
            "[6:0]  ChainID = 3\n"
            "[7:0]  EnterType = 0\n"
            "[8:0]  IsEntrustTx = 0\n"
            "[9:0]  CommitTime = 15Jan2019 08:03:21\n"
            "[10:0]  TxType = Authorize\n"
            "[11:0]  LockHeight = 0\n"
        },
        /////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////
        MANTxTestCase{
            "Example3",
            "f8d28710000000000042850430e2340083033450a04d414e2e576b62756a7478683759426e6b475638485a767950514b336341507980b88b5b7b22456e7472757374416464726573223a224d414e2e32556f7a3867386a61754d61326d746e7778727363686a3271504a7245222c224973456e7472757374476173223a747275652c22456e73747275737453657454797065223a312c22537461727454696d65223a313535393134353630302c22456e6454696d65223a313535393233323030307d5d038080808086016850894a0fc4c30580c0",
            /////
            "[0:0]  Nonce = 4503599627370562\n"
            "[1:0]  Gas Price = 18000000000\n"
            "[2:0]  Gas Limit = 210000\n"
            "[3:0]  To = MAN.Wkbujtxh7YBnkGV8HZvyPQK3cAPy\n"
            "[4:0]  Value = 0\n"
            "[5:0]  Data = [{\"EntrustAddres\":\"MAN.2Uoz8g8jauMa2mtn\n"
            "[5:1]  Data = wxrschj2qPJrE\",\"IsEntrustGas\":true,\"Ens\n"
            "[5:2]  Data = trustSetType\":1,\"StartTime\":1559145600,\n"
            "[5:3]  Data = \"EndTime\":1559232000}]\n"
            "[6:0]  ChainID = 3\n"
            "[7:0]  EnterType = 0\n"
            "[8:0]  IsEntrustTx = 0\n"
            "[9:0]  CommitTime = 15Jan2019 08:03:21\n"
            "[10:0]  TxType = Authorize\n"
            "[11:0]  LockHeight = 0\n"
        },
        /////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////
        MANTxTestCase{
            "Example4",
            "f8cf8710000000000043850430e2340083033450a04d414e2e576b62756a7478683759426e6b475638485a767950514b336341507980b8885b7b22456e7472757374416464726573223a224d414e2e3661706346595162595a68774c5a7a33626234546a666b67346d794a222c224973456e7472757374476173223a747275652c22456e73747275737453657454797065223a302c225374617274486569676874223a323232323232322c22456e64486569676874223a323232323232357d5d038080808086016850894a0fc4c30580c0",
            /////
            "[0:0]  Nonce = 4503599627370563\n"
            "[1:0]  Gas Price = 18000000000\n"
            "[2:0]  Gas Limit = 210000\n"
            "[3:0]  To = MAN.Wkbujtxh7YBnkGV8HZvyPQK3cAPy\n"
            "[4:0]  Value = 0\n"
            "[5:0]  Data = [{\"EntrustAddres\":\"MAN.6apcFYQbYZhwLZz3\n"
            "[5:1]  Data = bb4Tjfkg4myJ\",\"IsEntrustGas\":true,\"Enst\n"
            "[5:2]  Data = rustSetType\":0,\"StartHeight\":2222222,\"E\n"
            "[5:3]  Data = ndHeight\":2222225}]\n"
            "[6:0]  ChainID = 3\n"
            "[7:0]  EnterType = 0\n"
            "[8:0]  IsEntrustTx = 0\n"
            "[9:0]  CommitTime = 15Jan2019 08:03:21\n"
            "[10:0]  TxType = Authorize\n"
            "[11:0]  LockHeight = 0\n"
        },
        /////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////
        MANTxTestCase{
            "Example5",
            "f84a8710000000000043850430e2340083033450a14d414e2e32556f7a3867386a61754d61326d746e7778727363686a3271504a72458398968080038080803186016850894a0fc4c38080c0",
            /////
            "[0:0]  Nonce = 4503599627370563\n"
            "[1:0]  Gas Price = 18000000000\n"
            "[2:0]  Gas Limit = 210000\n"
            "[3:0]  To = MAN.2Uoz8g8jauMa2mtnwxrschj2qPJrE\n"
            "[4:0]  Value = 10000000\n"
            "[6:0]  ChainID = 3\n"
            "[7:0]  EnterType = 0\n"
            "[8:0]  IsEntrustTx = 49\n"
            "[9:0]  CommitTime = 15Jan2019 08:03:21\n"
            "[10:0]  TxType = Normal\n"
            "[11:0]  LockHeight = 0\n"
        },
        /////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////
        MANTxTestCase{
            "Example6",
            "f8c28710000000000044850430e2340083033450a14d414e2e32556f7a3867386a61754d61326d746e7778727363686a3271504a72458398968080038080808086016850894a0ff87bf8798080f875e6a04d414e2e6a4c5446686f434a43474368706964553269433151357a436d56464c8398968080e6a04d414e2e66344657484562576b583873536438796a5a6a5948655a576e6164788398968080e6a04d414e2e675141414855655442787667627a6638744667557461764463654a508398968080",
            /////
            "[0:0]  Nonce = 4503599627370564\n"
            "[1:0]  Gas Price = 18000000000\n"
            "[2:0]  Gas Limit = 210000\n"
            "[3:0]  To = MAN.2Uoz8g8jauMa2mtnwxrschj2qPJrE\n"
            "[4:0]  Value = 10000000\n"
            "[6:0]  ChainID = 3\n"
            "[7:0]  EnterType = 0\n"
            "[8:0]  IsEntrustTx = 0\n"
            "[9:0]  CommitTime = 15Jan2019 08:03:21\n"
            "[10:0]  TxType = Normal\n"
            "[11:0]  LockHeight = 0\n"
        },
        /////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////
        MANTxTestCase{
            "Example7",
            "f8488710000000000044850430e2340083033450a14d414e2e32556f7a3867386a61754d61326d746e7778727363686a3271504a724583989680800380808080845cf6bd8bc4c30380c0",
            /////
            "[0:0]  Nonce = 4503599627370564\n"
            "[1:0]  Gas Price = 18000000000\n"
            "[2:0]  Gas Limit = 210000\n"
            "[3:0]  To = MAN.2Uoz8g8jauMa2mtnwxrschj2qPJrE\n"
            "[4:0]  Value = 10000000\n"
            "[6:0]  ChainID = 3\n"
            "[7:0]  EnterType = 0\n"
            "[8:0]  IsEntrustTx = 0\n"
            "[9:0]  CommitTime = 19Jan1970 01:14:34\n"
            "[10:0]  TxType = Revocable\n"
            "[11:0]  LockHeight = 0\n"
        },
        /////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////
        MANTxTestCase{
            "Example8",
            "f8668710000000000045850430e2340083033450a04d414e2e576b62756a7478683759426e6b475638485a767950514b336341507980a0746dd5858305e95c2ad24ac22658786012963590e683258ab1b0b073a131adad038080808086016850894a0fc4c30480c0",
            /////
            "[0:0]  Nonce = 4503599627370565\n"
            "[1:0]  Gas Price = 18000000000\n"
            "[2:0]  Gas Limit = 210000\n"
            "[3:0]  To = MAN.Wkbujtxh7YBnkGV8HZvyPQK3cAPy\n"
            "[4:0]  Value = 0\n"
            "[5:0]  Data = 776DD5858305E95C2AD24AC22658786012963590E683258AB1B0B073A131ADAD\n"
            "[6:0]  ChainID = 3\n"
            "[7:0]  EnterType = 0\n"
            "[8:0]  IsEntrustTx = 0\n"
            "[9:0]  CommitTime = 15Jan2019 08:03:21\n"
            "[10:0]  TxType = Revert\n"
            "[11:0]  LockHeight = 0\n"
        },
        /////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////
        MANTxTestCase{"exampleTx1",
                      "f901e1"  // Root List
                      "80" // Nonce - 3
                      "850430e23400" // Gas Price - 4
                      "83033450" // Gas Limit - 10
                      "a04d414e2e576b62756a7478683759426e6b475638485a767950514b3363415079" // To - 14
                      "83989680" // Value - 47
                      "b9019d5b7b22456e7472757374416464726573223a224d414e2e32556f7a3867386a61754d61326d746e777872"
                      "7363686a3271504a7245222c224973456e7472757374476173223a747275652c224973456e7472757374536967"
                      "6e223a66616c73652c225374617274486569676874223a313232322c22456e64486569676874223a3132323232"
                      "322c22456e73747275737453657454797065223a302c22757365537461727454696d65223a22222c2275736545"
                      "6e6454696d65223a22222c22456e7472757374436f756e74223a307d2c7b22456e747275737441646472657322"
                      "3a224d414e2e32556f7a3867386a61754d61326d746e7778727363686a3271504a7245222c224973456e747275"
                      "7374476173223a747275652c224973456e74727573745369676e223a66616c73652c2253746172744865696768"
                      "74223a3132323232332c22456e64486569676874223a3132323232392c22456e73747275737453657454797065"
                      "223a302c22757365537461727454696d65223a22222c22757365456e6454696d65223a22222c22456e74727573"
                      "74436f756e74223a307d5d"   // Data - 51
                      "03" // ChainID - 467
                      "80" // R - 468
                      "80" // S - 469
                      "80" // Enter Type - 470
                      "80" // IsEntrust - 471
                      "86016850894a0f" // Commit Time - 472
                      "c4" // ExtraTo List - embeds a list - 479
                      /* */ "c3" // ExtraTo embedded list - 480
                      /*   */ "05"     // byte
                      /*   */ "80c0",  // string, list
            //////////// expected output:
                      "[0:0]  Nonce = 0\n"
                      "[1:0]  Gas Price = 18000000000\n"
                      "[2:0]  Gas Limit = 210000\n"
                      "[3:0]  To = MAN.Wkbujtxh7YBnkGV8HZvyPQK3cAPy\n"
                      "[4:0]  Value = 10000000\n"
                      "[5:0]  Data = [{\"EntrustAddres\":\"MAN.2Uoz8g8jauMa2mtn\n"
                      "[5:1]  Data = wxrschj2qPJrE\",\"IsEntrustGas\":true,\"IsE\n"
                      "[5:2]  Data = ntrustSign\":false,\"StartHeight\":1222,\"E\n"
                      "[5:3]  Data = ndHeight\":122222,\"EnstrustSetType\":0,\"u\n"
                      "[5:4]  Data = seStartTime\":\"\",\"useEndTime\":\"\",\"Entrus\n"
                      "[5:5]  Data = tCount\":0},{\"EntrustAddres\":\"MAN.2Uoz8g\n"
                      "[5:6]  Data = 8jauMa2mtnwxrschj2qPJrE\",\"IsEntrustGas\"\n"
                      "[5:7]  Data = :true,\"IsEntrustSign\":false,\"StartHeigh\n"
                      "[5:8]  Data = t\":122223,\"EndHeight\":122229,\"EnstrustS\n"
                      "[5:9]  Data = etType\":0,\"useStartTime\":\"\",\"useEndTime\n"
                      "[5:10]  Data = \":\"\",\"EntrustCount\":0}]\n"
                      "[6:0]  ChainID = 3\n"
                      "[7:0]  EnterType = 0\n"
                      "[8:0]  IsEntrustTx = 0\n"
                      "[9:0]  CommitTime = 15Jan2019 08:03:21\n"
                      "[10:0]  TxType = Authorize\n"
                      "[11:0]  LockHeight = 0\n"
        },
        MANTxTestCase{"exampleTx2",
                      "f8c2"
                      "8710000000000039"
                      "850430e23400"
                      "83033450"
                      "a14d414e2e32556f7a3867386a61754d61326d746e7778727363686a3271504a7245"
                      "83989680"
                      "80"
                      "03"
                      "80"
                      "80"
                      "80"
                      "80"
                      "86016850894a0f"
                      "f87b"
                      /**/ "f879"
                      /*   */ "8080f875e6a04d414e2e6a4c5446686f434a43474368706964553269433151"
                      "357a436d56464c8398968080e6a04d414e2e66344657484562576b583873536438796a5a6a5948655a57"
                      "6e6164788398968080e6a04d414e2e675141414855655442787667627a6638744667557461764463654a"
                      "508398968080",
            //////////// expected output:
                      "[0:0]  Nonce = 4503599627370553\n"
                      "[1:0]  Gas Price = 18000000000\n"
                      "[2:0]  Gas Limit = 210000\n"
                      "[3:0]  To = MAN.2Uoz8g8jauMa2mtnwxrschj2qPJrE\n"
                      "[4:0]  Value = 10000000\n"
                      "[6:0]  ChainID = 3\n"
                      "[7:0]  EnterType = 0\n"
                      "[8:0]  IsEntrustTx = 0\n"
                      "[9:0]  CommitTime = 15Jan2019 08:03:21\n"
                      "[10:0]  TxType = Normal\n"
                      "[11:0]  LockHeight = 0\n"
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
    uint8_t pageCount = 0;
    mantx_print(&ctx, data, MANTX_FIELD_TO, out, 1000, 0, &pageCount);
    EXPECT_THAT(pageCount, testing::Eq(1));
    std::cout << "To:" << " " << out << std::endl;
}

TEST_P(MANTxParamTest, iterateDisplayStream) {
    auto params = GetParam();

    uint8_t data[10000];
    uint16_t dataSize = parseHexString(params.data, data);

    mantx_context_t ctx;
    auto err = mantx_parse(&ctx, data, dataSize);
    EXPECT_THAT(err, testing::Eq(MANTX_NO_ERROR));

    std::stringstream ss;
    std::cout << std::endl;

    char displayKey[80];
    char displayValue[80];

    int8_t displayIdx = 0;
    int8_t pageIdx = 0;
    uint8_t pageCount = 0;
    while (true) {
        err = mantx_getItem(&ctx, data, displayIdx,
                            displayKey, sizeof(displayKey),
                            displayValue, sizeof(displayValue),
                            pageIdx, &pageCount);

        if (err != MANTX_NO_ERROR) {
            if (err != MANTX_ERROR_UNEXPECTED_DISPLAY_IDX) {
                ss << "ERR! " << (int) err << std::endl;
            }
            break;
        }

        if (pageIdx < pageCount) {
            ss << "[" << (int) displayIdx << ":" << (int) pageIdx << "]  ";
            ss << displayKey << " = " << displayValue;
            ss << std::endl;
            pageIdx++;
        }

        if (pageIdx >= pageCount) {
            pageIdx = 0;
            displayIdx++;
        }
    }

    std::cout << ss.str() << std::endl;

    EXPECT_THAT(ss.str(), testing::Eq(params.expectedStr));
}

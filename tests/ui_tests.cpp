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

#include "gmock/gmock.h"
#include "common.h"

using ::testing::TestWithParam;

class JsonTests : public JsonTests_Base {};


INSTANTIATE_TEST_SUITE_P (
    JsonTestCases,
    JsonTests,
    ::testing::ValuesIn(GetJsonTestCases("testcases.json")),
    JsonTests::PrintToStringParamName()
);

TEST_P(JsonTests, Normal) { check_testcase(GetParam(), false); }
// TEST_P(JsonTests, Expert) { check_testcase(GetParam(), true); }

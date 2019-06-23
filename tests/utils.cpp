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
#include "lib/utils.h"

TEST(utils, convertToHexInPlace) {
    uint8_t data_in[] = {0, 1, 2, 5, 9, 250};
    uint8_t data_out[11];

    MEMCPY(data_out, data_in, sizeof(data_in));
    convertToHexstringInPlace(data_out, sizeof(data_in), sizeof(data_out));

    std::stringstream ss;
    ss << (char*) data_out << std::endl;

    std::cout << std::endl;
    std::cout << ss.str() << std::endl;
}

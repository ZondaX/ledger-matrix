/*******************************************************************************
*  (c) 2018 - 2023 Zondax AG
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
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "rlp_def.h"

#define MANTX_ROOTFIELD_COUNT 13
#define MANTX_EXTRAFIELD_COUNT 3
#define MANTX_EXTRALISTFIELD_COUNT 10
#define MANTX_DISPLAY_COUNT 12
#define MANTX_EXTRATOFIELD_COUNT 3

#define DECIMAL_BASE 10

typedef enum {
    MANTX_TXTYPE_NORMAL = 0,
    MANTX_TXTYPE_BROADCAST = 1,
    MANTX_TXTYPE_MINER_REWARD = 2,
    MANTX_TXTYPE_SCHEDULED = 3,
    MANTX_TXTYPE_REVERT = 4,
    MANTX_TXTYPE_AUTHORIZED = 5,
    MANTX_TXTYPE_CANCEL_AUTH = 6,
    MANTX_TXTYPE_CREATE_CURR = 9,
    MANTX_TXTYPE_VERIFY_REWARD = 10,
    MANTX_TXTYPE_INTEREST_REWARD = 11,
    MANTX_TXTYPE_TXFEE_REWARD = 12,
    MANTX_TXTYPE_LOTTERY_REWARD = 13,
    MANTX_TXTYPE_SET_BLACKLIST = 14,
    MANTX_TXTYPE_SUPERBLOCK = 122,
} tx_type_e;

typedef enum {
    MANTX_FIELD_NONCE = 0,
    MANTX_FIELD_GASPRICE,
    MANTX_FIELD_GASLIMIT,
    MANTX_FIELD_TO,
    MANTX_FIELD_VALUE,
    MANTX_FIELD_DATA,
    ///
    MANTX_FIELD_V,
    // MANTX_FIELD_R = 7,
    // MANTX_FIELD_S = 8,
    ///
    MANTX_FIELD_ENTERTYPE,
    MANTX_FIELD_ISENTRUSTTX,
    MANTX_FIELD_COMMITTIME,
    // MANTX_FIELD_EXTRA,     // This field is a list so it is not shown

    // These field may or may not be available
    MANTX_FIELD_EXTRA_TXTYPE,
    MANTX_FIELD_EXTRA_LOCKHEIGHT,
    MANTX_FIELD_EXTRA_TO,
} tx_fields_e;


typedef struct {
    rlp_t root;
    rlp_t fields[MANTX_ROOTFIELD_COUNT];
    rlp_t extraFields[MANTX_EXTRAFIELD_COUNT];
    rlp_t extraToListFields[MANTX_EXTRALISTFIELD_COUNT];
    uint16_t rootFieldsItems;
    uint16_t extraFieldsItems;
    uint16_t extraToFieldsItems;
    uint8_t extraTxType;
} parser_tx_t;


#ifdef __cplusplus
}
#endif

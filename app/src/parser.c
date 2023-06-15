/*******************************************************************************
*   (c) 2018 - 2023 Zondax AG
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

#include <stdio.h>
#include <zxmacros.h>
#include <zxformat.h>
#include <zxtypes.h>

#include "coin.h"
#include "parser_common.h"
#include "parser_impl.h"
#include "parser.h"
#include "rlp.h"
#include "crypto.h"
#include "timeutils.h"

static parser_tx_t tx_obj;

zxerr_t keccak_hash(const unsigned char *in, unsigned int inLen,
                    unsigned char *out, unsigned int outLen);

parser_error_t parser_init_context(parser_context_t *ctx,
                                   const uint8_t *buffer,
                                   uint16_t bufferSize) {
    ctx->offset = 0;
    ctx->buffer = NULL;
    ctx->bufferLen = 0;

    if (bufferSize == 0 || buffer == NULL) {
        // Not available, use defaults
        return parser_init_context_empty;
    }

    MEMZERO(&tx_obj, sizeof(tx_obj));
    ctx->tx_obj = &tx_obj;

    ctx->buffer = buffer;
    ctx->bufferLen = bufferSize;
    return parser_ok;
}

parser_error_t parser_parse(parser_context_t *ctx,
                            const uint8_t *data,
                            size_t dataLen) {
    CHECK_ERROR(parser_init_context(ctx, data, dataLen))
    return _read(ctx, &tx_obj);
}

parser_error_t parser_validate(parser_context_t *ctx) {
    // Iterate through all items to check that all can be shown and are valid
    uint8_t numItems = 0;
    CHECK_ERROR(parser_getNumItems(ctx, &numItems))

    char tmpKey[40];
    char tmpVal[40];

    for (uint8_t idx = 0; idx < numItems; idx++) {
        uint8_t pageCount = 0;
        CHECK_ERROR(parser_getItem(ctx, idx, tmpKey, sizeof(tmpKey), tmpVal, sizeof(tmpVal), 0, &pageCount))
    }
    return parser_ok;
}

parser_error_t parser_getNumItems(const parser_context_t *ctx, uint8_t *num_items) {
    if (ctx == NULL || ctx->tx_obj == NULL || num_items == NULL) {
        return parser_unexpected_error;
    }
    *num_items = MANTX_DISPLAY_COUNT + ctx->tx_obj->extraToFieldsItems * 3;
    return parser_ok;
}

static void cleanOutput(char *outKey, uint16_t outKeyLen,
                        char *outVal, uint16_t outValLen)
{
    MEMZERO(outKey, outKeyLen);
    MEMZERO(outVal, outValLen);
    snprintf(outKey, outKeyLen, "?");
    snprintf(outVal, outValLen, " ");
}

static parser_error_t checkSanity(uint8_t numItems, uint8_t displayIdx)
{
    if ( displayIdx >= numItems) {
        return parser_display_idx_out_of_range;
    }
    return parser_ok;
}

static parser_error_t printCommitTime(const uint256_t *value, char* outVal, uint16_t outValLen) {

    // Restrict commit time to uint64_t
    if (value == NULL || value->elements[0].elements[0] != 0 ||
                         value->elements[0].elements[1] != 0 ||
                         value->elements[1].elements[0] != 0) {
        return parser_unexpected_error;
    }

    const uint64_t time = value->elements[1].elements[1];
    if (printTime(outVal, outValLen, time) != zxerr_ok) {
        return parser_unexpected_error;
    }

    return parser_ok;
}

static parser_error_t printDataField(const rlp_t *rlp, const tx_fields_e extraTxType,
                                    char *outVal, uint16_t outValLen,
                                    uint8_t pageIdx, uint8_t *pageCount) {
    if (rlp == NULL || pageCount == NULL) {
        return parser_unexpected_error;
    }

    switch (extraTxType) {
        case MANTX_TXTYPE_AUTHORIZED:
        case MANTX_TXTYPE_CREATE_CURR:
        case MANTX_TXTYPE_CANCEL_AUTH:
            pageStringExt(outVal, outValLen, (const char*) rlp->ptr, rlp->rlpLen, pageIdx, pageCount);
            break;

        case MANTX_TXTYPE_NORMAL:
        case MANTX_TXTYPE_SCHEDULED:
        case MANTX_TXTYPE_REVERT:
            pageStringHex(outVal, outValLen, (const char*) rlp->ptr, rlp->rlpLen, pageIdx, pageCount);
            break;

        // case MANTX_TXTYPE_BROADCAST:
        // case MANTX_TXTYPE_MINER_REWARD:
        // case MANTX_TXTYPE_VERIFY_REWARD:
        // case MANTX_TXTYPE_INTEREST_REWARD:
        // case MANTX_TXTYPE_TXFEE_REWARD:
        // case MANTX_TXTYPE_LOTTERY_REWARD:
        // case MANTX_TXTYPE_SET_BLACKLIST:
        // case MANTX_TXTYPE_SUPERBLOCK:
        default:
            return parser_unexpected_type;
    }

    if (rlp->rlpLen == 0) {
        *pageCount = 1;
        snprintf(outVal, outValLen, "Empty");
    }
    return parser_ok;
}

static parser_error_t printTxType(tx_fields_e extraTxType, char *outVal, uint16_t outValLen) {
    switch (extraTxType) {
        case MANTX_TXTYPE_NORMAL:
            snprintf(outVal, outValLen, "Normal");
            break;
//        case MANTX_TXTYPE_BROADCAST:
//            snprintf(outVal, outValLen, "Broadcast");
//            break;
//        case MANTX_TXTYPE_MINER_REWARD:
//            snprintf(outVal, outValLen, "Miner reward");
//            break;
        case MANTX_TXTYPE_SCHEDULED:
            snprintf(outVal, outValLen, "Scheduled");
            break;
        case MANTX_TXTYPE_REVERT:
            snprintf(outVal, outValLen, "Revert");
            break;
        case MANTX_TXTYPE_AUTHORIZED:
            snprintf(outVal, outValLen, "Authorize");
            break;
        case MANTX_TXTYPE_CANCEL_AUTH:
            snprintf(outVal, outValLen, "Cancel Auth");
            break;
        case MANTX_TXTYPE_CREATE_CURR:
            snprintf(outVal, outValLen, "Create curr");
            break;
//        case MANTX_TXTYPE_VERIF_REWARD:
//            snprintf(outVal, outValLen, "Verif reward");
//            break;
//        case MANTX_TXTYPE_INTEREST_REWARD:
//            snprintf(outVal, outValLen, "Interest reward");
//            break;
//        case MANTX_TXTYPE_TXFEE_REWARD:
//            snprintf(outVal, outValLen, "Tx Fee reward");
//            break;
//        case MANTX_TXTYPE_LOTTERY_REWARD:
//            snprintf(outVal, outValLen, "Lottery reward");
//            break;
//        case MANTX_TXTYPE_SET_BLACKLIST:
//            snprintf(outVal, outValLen, "Set blacklist");
//            break;
//        case MANTX_TXTYPE_SUPERBLOCK:
//            snprintf(outVal, outValLen, "Super block");
//            break;
        default:
            return parser_unexpected_type;
    }

    return parser_ok;
}

static parser_error_t printExtraFields(const parser_tx_t *txObj, uint8_t displayIdx,
                                       char *outKey, uint16_t outKeyLen,
                                       char *outVal, uint16_t outValLen,
                                       uint8_t pageIdx, uint8_t *pageCount) {
    if (displayIdx >= MANTX_DISPLAY_COUNT + txObj->extraToFieldsItems * 3) {
        return parser_no_data;
    }

    const uint8_t extraToIdx = (displayIdx - MANTX_DISPLAY_COUNT) / 3;
    const uint8_t fieldIdx = (displayIdx - MANTX_DISPLAY_COUNT) % 3;

    // Retrieve internal items
    rlp_t tmpList[MANTX_EXTRATOFIELD_COUNT] = {0};
    uint16_t tmpItems = 0;
    uint256_t value = {0};
    CHECK_ERROR(rlp_readList(&txObj->extraToListFields[extraToIdx], &tmpList[0], &tmpItems, MANTX_EXTRATOFIELD_COUNT))

    parser_error_t err = parser_unexpected_error;
    switch (fieldIdx) {
        case 0:
            snprintf(outKey, outKeyLen, "To [%d]", extraToIdx);
            pageStringExt(outVal, outValLen, (const char*) tmpList[fieldIdx].ptr, tmpList[fieldIdx].rlpLen,
                          pageIdx, pageCount);
            err = parser_ok;
            break;

        case 1:
            snprintf(outKey, outKeyLen, "Amount [%d]", extraToIdx);
            CHECK_ERROR(rlp_readUInt256(&tmpList[fieldIdx], &value))
            err = tostring256(&value, DECIMAL_BASE, outVal, outValLen) ? parser_ok : parser_unexpected_error;
            break;

        case 2:
            snprintf(outKey, outKeyLen, "Payload [%d]", extraToIdx);
            pageStringExt(outVal, outValLen, (const char*) tmpList[fieldIdx].ptr, tmpList[fieldIdx].rlpLen,
                          pageIdx, pageCount);

            err = parser_ok;
            break;

        default:
            return parser_no_data;
    }

    if (tmpList[fieldIdx].rlpLen == 0) {
        *pageCount = 1;
        snprintf(outVal, outValLen, "Empty");
    }

    return err;
}

parser_error_t parser_getItem(const parser_context_t *ctx,
                              uint8_t displayIdx,
                              char *outKey, uint16_t outKeyLen,
                              char *outVal, uint16_t outValLen,
                              uint8_t pageIdx, uint8_t *pageCount) {

    *pageCount = 1;
    uint8_t numItems = 0;
    CHECK_ERROR(parser_getNumItems(ctx, &numItems))
    CHECK_APP_CANARY()

    CHECK_ERROR(checkSanity(numItems, displayIdx))
    cleanOutput(outKey, outKeyLen, outVal, outValLen);

    uint256_t value = {0};
    const rlp_t *rlpPtr = displayIdx < MANTX_ROOTFIELD_COUNT ? &ctx->tx_obj->fields[displayIdx] : NULL;

    switch (displayIdx) {
        case MANTX_FIELD_NONCE:
            snprintf(outKey, outKeyLen, "Nonce");
            CHECK_ERROR(rlp_readUInt256(rlpPtr, &value))
            return tostring256(&value, DECIMAL_BASE, outVal, outValLen) ? parser_ok : parser_unexpected_error;

        case MANTX_FIELD_GASPRICE:
            snprintf(outKey, outKeyLen, "Gas Price");
            CHECK_ERROR(rlp_readUInt256(rlpPtr, &value))
            return tostring256(&value, DECIMAL_BASE, outVal, outValLen) ? parser_ok : parser_unexpected_error;

        case MANTX_FIELD_GASLIMIT:
            snprintf(outKey, outKeyLen, "Gas Limit");
            CHECK_ERROR(rlp_readUInt256(rlpPtr, &value))
            return tostring256(&value, DECIMAL_BASE, outVal, outValLen) ? parser_ok : parser_unexpected_error;

        case MANTX_FIELD_TO:
            snprintf(outKey, outKeyLen, "To");
            pageStringExt(outVal, outValLen, (const char*) rlpPtr->ptr, rlpPtr->rlpLen, pageIdx, pageCount);
            return parser_ok;

        case MANTX_FIELD_VALUE:
            snprintf(outKey, outKeyLen, "Value");
            CHECK_ERROR(rlp_readUInt256(rlpPtr, &value))
            return tostring256(&value, DECIMAL_BASE, outVal, outValLen) ? parser_ok : parser_unexpected_error;

        case MANTX_FIELD_DATA:
            snprintf(outKey, outKeyLen, "Data");
            return printDataField(rlpPtr, ctx->tx_obj->extraTxType, outVal, outValLen, pageIdx, pageCount);

        case MANTX_FIELD_V:
            snprintf(outKey, outKeyLen, "ChainID");
            if (rlpPtr->kind != RLP_KIND_BYTE ||
                rlpPtr->rlpLen != 0) {
                return parser_unexpected_value;
            }
            snprintf(outVal, outValLen, "%d", *rlpPtr->ptr);
            return parser_ok;

        case MANTX_FIELD_ENTERTYPE:
            snprintf(outKey, outKeyLen, "EnterType");
            CHECK_ERROR(rlp_readUInt256(rlpPtr, &value))
            return tostring256(&value, DECIMAL_BASE, outVal, outValLen) ? parser_ok : parser_unexpected_error;

        case MANTX_FIELD_ISENTRUSTTX:
            snprintf(outKey, outKeyLen, "IsEntrustTx");
            rlpPtr = &ctx->tx_obj->fields[displayIdx + 2];
            CHECK_ERROR(rlp_readUInt256(rlpPtr, &value))
            return tostring256(&value, DECIMAL_BASE, outVal, outValLen) ? parser_ok : parser_unexpected_error;

        case MANTX_FIELD_COMMITTIME:
            snprintf(outKey, outKeyLen, "CommitTime");
            rlpPtr = &ctx->tx_obj->fields[displayIdx + 2];
            CHECK_ERROR(rlp_readUInt256(rlpPtr, &value))
            return printCommitTime(&value, outVal, outValLen);

        case MANTX_FIELD_EXTRA_TXTYPE:
            snprintf(outKey, outKeyLen, "TxType");
            return printTxType(ctx->tx_obj->extraTxType, outVal, outValLen);

        case MANTX_FIELD_EXTRA_LOCKHEIGHT:
            snprintf(outKey, outKeyLen, "Lock Height");
            rlpPtr = &ctx->tx_obj->extraFields[1];
            CHECK_ERROR(rlp_readUInt256(rlpPtr, &value))
            return tostring256(&value, DECIMAL_BASE, outVal, outValLen) ? parser_ok : parser_unexpected_error;

        default:
            // If extra fields are present, then print. Otherwise, return error
            return printExtraFields(ctx->tx_obj, displayIdx, outKey, outKeyLen,
                                    outVal, outValLen, pageIdx, pageCount);
    }

    return parser_no_data;
}

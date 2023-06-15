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

#include "parser_impl.h"
#include "rlp.h"
static parser_error_t validateTxType(uint8_t value);

parser_error_t _read(parser_context_t *ctx, parser_tx_t *v) {
    if (ctx == NULL || v == NULL) {
        return parser_unexpected_error;
    }

    uint16_t rootList = 0;
    // We expect a single root list
    CHECK_ERROR(rlp_parseStream(ctx, &v->root, &rootList, 1))
    if (ctx->offset < ctx->bufferLen) {
        return parser_unexpected_unparsed_bytes;
    }

    if (v->root.kind != RLP_KIND_LIST) {
        return parser_unexpected_type;
    }

    // Parse items within Root
    CHECK_ERROR(rlp_readList(&v->root, v->fields, &v->rootFieldsItems, MANTX_ROOTFIELD_COUNT))
    if (v->rootFieldsItems != MANTX_ROOTFIELD_COUNT) {
        return parser_unexpected_number_items;
    }

    // Parse Extra fields
    const rlp_t *extraFieldsList = &v->fields[MANTX_ROOTFIELD_COUNT - 1];
    rlp_t extraFieldsInnerElements = {0};
    CHECK_ERROR(rlp_readList(extraFieldsList, &extraFieldsInnerElements, &v->extraFieldsItems, 1))
    CHECK_ERROR(rlp_readList(&extraFieldsInnerElements, &v->extraFields[0], &v->extraFieldsItems, MANTX_EXTRAFIELD_COUNT))
    if (v->extraFieldsItems != MANTX_EXTRAFIELD_COUNT) {
        return parser_unexpected_number_items;
    }

    uint256_t tmp = {0};
    CHECK_ERROR(rlp_readUInt256(v->extraFields, &tmp))
    // Extract last byte
    v->extraTxType = tmp.elements[1].elements[1];
    CHECK_ERROR(validateTxType(v->extraTxType))

    // EXTRA TO [ [List_0], [List_1], [List_2]... [List_N]  ]
    // [List_i] = [To: String | Amount: String | Payload: String | Empty
    const rlp_t *extraToList = &v->extraFields[MANTX_EXTRAFIELD_COUNT - 1];
    if (extraToList->kind == RLP_KIND_LIST && extraToList->rlpLen > 0) {
        CHECK_ERROR(rlp_readList(extraToList, &v->extraToListFields[0], &v->extraToFieldsItems, MANTX_EXTRALISTFIELD_COUNT))
        // Verify inner elements
        for (uint8_t i = 0; i < v->extraToFieldsItems; i++) {
            rlp_t tmpList[MANTX_EXTRATOFIELD_COUNT] = {0};
            uint16_t tmpItems = 0;
            CHECK_ERROR(rlp_readList(&v->extraToListFields[i], &tmpList[0], &tmpItems, MANTX_EXTRATOFIELD_COUNT))
            if (tmpItems != MANTX_EXTRATOFIELD_COUNT) {
                return parser_unexpected_value;
            }
        }
    }

    return parser_ok;
}

parser_error_t validateTxType(uint8_t value) {
    // Bounds check
    if (value > MANTX_TXTYPE_SUPERBLOCK) {
        return parser_value_out_of_range;
    }

    uint8_t left = MANTX_TXTYPE_NORMAL;
    uint8_t right = MANTX_TXTYPE_SUPERBLOCK;
    while (left <= right) {
        uint8_t mid = (left + right) / 2;
        if (mid == value) {
            return parser_ok;
        } else if (mid < value) {
            left = mid + 1;
        } else {
            right = mid - 1;
        }
    }

    return parser_unexpected_value;
}

const char *parser_getErrorDescription(parser_error_t err) {
    switch (err) {
        case parser_ok:
            return "No error";
        case parser_no_data:
            return "No more data";
        case parser_init_context_empty:
            return "Initialized empty context";
        case parser_unexpected_buffer_end:
            return "Unexpected buffer end";
        case parser_unexpected_unparsed_bytes:
            return "Unexpected unparsed bytes";
        case parser_unexpected_version:
            return "Unexpected version";
        case parser_unexpected_characters:
            return "Unexpected characters";
        case parser_unexpected_field:
            return "Unexpected field";
        case parser_duplicated_field:
            return "Unexpected duplicated field";
        case parser_value_out_of_range:
            return "Value out of range";
        case parser_unexpected_chain:
            return "Unexpected chain";
        case parser_missing_field:
            return "missing field";

        case parser_display_idx_out_of_range:
            return "display index out of range";
        case parser_display_page_out_of_range:
            return "display page out of range";

        default:
            return "Unrecognized error code";
    }
}

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
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#define CLA                             0x88

#define HDPATH_LEN_DEFAULT   5
#define HDPATH_0_DEFAULT     (0x80000000u | 0x2c)   //44
#define HDPATH_1_DEFAULT     (0x80000000u | 0x13e)  //318

#define HDPATH_2_DEFAULT     (0x80000000u | 0u)
#define HDPATH_3_DEFAULT     (0u)
#define HDPATH_4_DEFAULT     (0u)

#define ADDRESS_LEN    50u
#define ETH_ADDRESS_LEN    20

#define SECP256K1_PRIVKEY_LEN       32u
#define SECP256K1_PUBKEY_LEN        65u

#define SECP256K1_SIGNATURE_LEN     64u
#define SECP256K1_DER_SIGNATURE_MAXLEN  73u

#define KECCAK_HASH_SIZE    32u
#define KECCAK_256_SIZE     256u


#define COIN_AMOUNT_DECIMAL_PLACES 18
#define COIN_TICKER "MAN "

#define MENU_MAIN_APP_LINE1 "Matrix AI"
#define MENU_MAIN_APP_LINE2 "Ready"
#define MENU_MAIN_APP_LINE2_SECRET "???"
#define APPVERSION_LINE1 "Matrix AI"
#define APPVERSION_LINE2 "v" APPVERSION

#ifdef __cplusplus
}
#endif

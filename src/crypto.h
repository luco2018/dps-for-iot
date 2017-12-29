/*
 *******************************************************************
 *
 * Copyright 2017 Intel Corporation All rights reserved.
 *
 *-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 *-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
 */

#ifndef _CRYPTO_H
#define _CRYPTO_H

#ifdef __cplusplus
extern "C" {
#endif

#define AES_128_KEY_LEN 16 /**< AES 128 key length, in bytes */

#define EC_MAX_COORD_LEN 66 /**< Maximum length of an EC coordinate (x, y, or d) */
#define EC_CURVE_P256 1 /**< NIST P-256 also known as secp256r1 */
#define EC_CURVE_P384 2 /**< NIST P-384 also known as secp384r1 */
#define EC_CURVE_P521 3 /**< NIST P-521 also known as secp521r1 */

#ifdef __cplusplus
}
#endif

#endif

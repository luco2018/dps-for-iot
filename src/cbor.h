/*
 *******************************************************************
 *
 * Copyright 2016 Intel Corporation All rights reserved.
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

#ifndef _CBOR_H
#define _CBOR_H

#include <stdint.h>
#include <stddef.h>
#include <dps/private/dps.h>

#ifdef __cplusplus
extern "C" {
#endif

#define CBOR_UINT   (0 << 5)
#define CBOR_NEG    (1 << 5)
#define CBOR_BYTES  (2 << 5)
#define CBOR_STRING (3 << 5)
#define CBOR_ARRAY  (4 << 5)
#define CBOR_MAP    (5 << 5)
#define CBOR_OPT    (6 << 5)
#define CBOR_OTHER  (7 << 5)

#define CBOR_MAX_LENGTH (1 + sizeof(uint64_t))

DPS_Status CBOR_EncodeLength(DPS_Buffer* buffer, uint64_t len, uint8_t maj);

DPS_Status CBOR_Copy(DPS_Buffer* buffer, const uint8_t* data, size_t len);

DPS_Status CBOR_EncodeUint(DPS_Buffer* buffer, uint64_t n);

DPS_Status CBOR_EncodeInt(DPS_Buffer* buffer, int64_t i);

DPS_Status CBOR_EncodeBytes(DPS_Buffer* buffer, const uint8_t* data, size_t len);

DPS_Status CBOR_EncodeString(DPS_Buffer* buffer, const char* str);

DPS_Status CBOR_EncodeArray(DPS_Buffer* buffer, size_t len);

DPS_Status CBOR_EncodeMap(DPS_Buffer* buffer, size_t len);

DPS_Status CBOR_EncodeBoolean(DPS_Buffer* buffer, int b);

DPS_Status CBOR_DecodeUint8(DPS_Buffer* buffer, uint8_t* n);

DPS_Status CBOR_DecodeUint16(DPS_Buffer* buffer, uint16_t* n);

DPS_Status CBOR_DecodeUint32(DPS_Buffer* buffer, uint32_t* n);

DPS_Status CBOR_DecodeUint(DPS_Buffer* buffer, uint64_t* n);

DPS_Status CBOR_DecodeInt(DPS_Buffer* buffer, int64_t* i);

DPS_Status CBOR_DecodeInt8(DPS_Buffer* buffer, int8_t* n);

DPS_Status CBOR_DecodeInt16(DPS_Buffer* buffer, int16_t* n);

DPS_Status CBOR_DecodeInt32(DPS_Buffer* buffer, int32_t* n);

DPS_Status CBOR_DecodeMap(DPS_Buffer* buffer, size_t* size);

DPS_Status CBOR_DecodeBoolean(DPS_Buffer* buffer, int* b);

DPS_Status CBOR_ReserveBytes(DPS_Buffer* buffer, size_t len, uint8_t** ptr);

/*
 * For symmetry with CBOR_DecodeInt8()
 */
#define CBOR_EncodeInt8(buffer, n) CBOR_EncodeInt(buffer, (int64_t)n)

/*
 * For symmetry with CBOR_DecodeInt16()
 */
#define CBOR_EncodeInt16(buffer, n) CBOR_EncodeInt(buffer, (int64_t)n)

/*
 * For symmetry with CBOR_DecodeInt32()
 */
#define CBOR_EncodeInt32(buffer, n) CBOR_EncodeInt(buffer, (int64_t)n)

/*
 * For symmetry with CBOR_DecodeUint8()
 */
#define CBOR_EncodeUint8(buffer, n) CBOR_EncodeUint(buffer, (uint64_t)n)

/*
 * For symmetry with CBOR_DecodeUint16()
 */
#define CBOR_EncodeUint16(buffer, n) CBOR_EncodeUint(buffer, (uint64_t)n)

/*
 * For symmetry with CBOR_DecodeUint32()
 */
#define CBOR_EncodeUint32(buffer, n) CBOR_EncodeUint(buffer, (uint64_t)n)

/**
 *
 * @param buffer  Buffer to decode from
 * @param data    Returns pointer into buffer storage to the decoded bytes 
 * @param size    Returns length of the decoded bytes
 *
 * @return - DPS_OK if the bytes were decoded
 *         - DPS_ERR_EOD if there was insufficient data in the buffer
 *
 */
DPS_Status CBOR_DecodeBytes(DPS_Buffer* buffer, uint8_t** data, size_t* size);

/**
 *
 * @param buffer  Buffer to decode from
 * @param data    Returns pointer into buffer storage to the decoded string 
 * @param size    Returns length of the decoded string
 *
 * @return - DPS_OK if the string was decoded
 *         - DPS_ERR_EOD if there was insufficient data in the buffer
 *
 */
DPS_Status CBOR_DecodeString(DPS_Buffer* buffer, char** data, size_t* size);

/**
 *
 * @param buffer  Buffer to decode from
 * @param size    Returns number of array items
 *
 * @return - DPS_OK if the array was decoded
 *         - DPS_ERR_EOD if there was insufficient data in the buffer
 *
 */
DPS_Status CBOR_DecodeArray(DPS_Buffer* buffer, size_t* size);

/**
 * Fix up the length of a byte string after the byte string has been appended
 * to the buffer.  CBOR_EncodeBytes() should have been called immediately
 * before with NULL passed as the data arg. The new length must be less than or
 * equal to the length passed in the call to CBOR_EncodeBytes().
 */
DPS_Status CBOR_FixupLength(DPS_Buffer* buffer, size_t origSize, size_t newSizcons);

#ifdef __cplusplus
}
#endif

#endif

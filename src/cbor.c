#include <dps_dbg.h>
#include <cbor.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

/*
 * Debug control for this module
 */
DPS_DEBUG_CONTROL(DPS_DEBUG_OFF);


#define CBOR_UINT   (0 << 5)
#define CBOR_NEG    (1 << 5)
#define CBOR_BYTES  (2 << 5)
#define CBOR_STRING (3 << 5)
#define CBOR_ARRAY  (4 << 5)
#define CBOR_MAP    (5 << 5)
#define CBOR_OPT    (6 << 5)
#define CBOR_OTHER  (7 << 5)

#define CBOR_LEN1   24
#define CBOR_LEN2   25
#define CBOR_LEN4   26
#define CBOR_LEN8   27

static int Requires(uint64_t n)
{
    if (n < 24) {
        return 0;
    }
    if (n < UINT8_MAX) {
        return 1;
    }
    if (n < UINT16_MAX) {
        return 2;
    }
    if (n < UINT32_MAX) {
        return 4;
    }
    return 8;
}

static DPS_Status EncodeUint(DPS_Buffer* buffer, uint64_t n, uint8_t maj)
{
    uint8_t* p = buffer->pos;
    int lenReq = Requires(n);

    if ((lenReq + 1) > DPS_BufferSpace(buffer)) {
        return DPS_ERR_OVERFLOW;
    }
    switch (lenReq) {
        case 0:
            *p++ = maj | n;
            break;
        case 1:
            *p++ = maj | CBOR_LEN1;
            *p++ = n;
            break;
        case 2:
            *p++ = maj | CBOR_LEN2;
            *p++ = n >> 8;
            *p++ = n;
            break;
        case 4:
            *p++ = maj | CBOR_LEN4;
            *p++ = n >> 24;
            *p++ = n >> 16;
            *p++ = n >> 8;
            *p++ = n;
            break;
        case 8:
            *p++ = maj | CBOR_LEN8;
            *p++ = n >> 56;
            *p++ = n >> 48;
            *p++ = n >> 40;
            *p++ = n >> 32;
            *p++ = n >> 24;
            *p++ = n >> 16;
            *p++ = n >> 8;
            *p++ = n;
    }
    buffer->pos = p;
    return DPS_OK;
}

static DPS_Status DecodeUint(DPS_Buffer* buffer, uint64_t* n, uint8_t* maj)
{
    size_t avail = DPS_BufferAvail(buffer);
    uint8_t* p = buffer->pos;
    uint8_t info;

    if (avail < 1) {
        return DPS_ERR_EOD;
    }
    info = *p++;
    *maj = info & 0xE0;
    info &= 0x1F;

    if (info < 24) {
        *n = info;
    } else if (info == CBOR_LEN1) {
        if (avail < 2) {
            return DPS_ERR_EOD;
        }
        *n = *p++;
    } else if (info == CBOR_LEN2) {
        if (avail < 3) {
            return DPS_ERR_EOD;
        }
        *n = (uint64_t)*p++ << 8 | *p++;
    } else if (info == CBOR_LEN4) {
        if (avail < 5) {
            return DPS_ERR_EOD;
        }
        *n = (uint64_t)*p++ << 24 | (uint64_t)*p++ << 16 | (uint64_t)*p++ << 8 | *p++;
    } else if (info == CBOR_LEN8) {
        if (avail < 9) {
            return DPS_ERR_EOD;
        }
        *n = (uint64_t)*p++ << 56 | (uint64_t)*p++ << 48 | (uint64_t)*p++ << 40 | (uint64_t)*p++ << 32 | (uint64_t)*p++ << 24 | (uint64_t)*p++ << 16 | (uint64_t)*p++ << 8 | *p++;
    } else {
        return DPS_ERR_INVALID;
    }
    buffer->pos = p;
    return DPS_OK;
}

DPS_Status CBOR_EncodeUint8(DPS_Buffer* buffer, uint8_t n)
{
    return EncodeUint(buffer, n, CBOR_UINT);
}

DPS_Status CBOR_EncodeUint(DPS_Buffer* buffer, uint64_t n)
{
    return EncodeUint(buffer, n, CBOR_UINT);
}

DPS_Status CBOR_EncodeInt(DPS_Buffer* buffer, int64_t i)
{
    if (i >= 0) {
        return EncodeUint(buffer, (uint64_t)i, CBOR_UINT);
    } else {
        return EncodeUint(buffer, ~(uint64_t)i, CBOR_NEG);
    }
}

DPS_Status CBOR_FixupLength(DPS_Buffer* buffer, size_t origLen, size_t newLen)
{
    uint8_t* pos = buffer->pos;
    uint8_t* p;
    uint8_t maj;
    uint64_t len;
    int lenReq = Requires(origLen);
    
    if (origLen < newLen) {
        return DPS_ERR_INVALID;
    }
    /*
     * Back-off the pointer to the start of the byte string
     */
    p = pos - newLen - (1 + lenReq);
    buffer->pos = p;
    if (DecodeUint(buffer, &len, &maj) != DPS_OK || len != origLen) {
        return DPS_ERR_INVALID;
    }
    if (lenReq == 0) {
        *p++ = maj | newLen;
    } else {
        ++p;
        if (lenReq == 8) {
            *p++ = newLen >> 56;
            *p++ = newLen >> 48;
            *p++ = newLen >> 40;
            *p++ = newLen >> 32;
        }
        if (lenReq >= 4) {
            *p++ = newLen >> 24;
            *p++ = newLen >> 16;
        }
        if (lenReq >= 2) {
            *p++ = newLen >> 8;
        }
        *p++ = newLen;
    }
    buffer->pos += newLen;
    assert(buffer->pos == pos);
    return DPS_OK;
}

DPS_Status CBOR_EncodeBytes(DPS_Buffer* buffer, const uint8_t* data, size_t len)
{
    DPS_Status ret = EncodeUint(buffer, (uint32_t)len, CBOR_BYTES);
    if (ret == DPS_OK) {
        if (DPS_BufferSpace(buffer) < len) {
            ret = DPS_ERR_OVERFLOW;
        } else if (data) {
            memcpy(buffer->pos, data, len);
            buffer->pos += len;
        }
    }
    return ret;
}

DPS_Status CBOR_ReserveBytes(DPS_Buffer* buffer, size_t len, uint8_t** ptr)
{
    DPS_Status ret = EncodeUint(buffer, (uint32_t)len, CBOR_BYTES);
    if (ret == DPS_OK) {
        if (DPS_BufferSpace(buffer) < len) {
            ret = DPS_ERR_OVERFLOW;
        } else {
            *ptr = buffer->pos;
            buffer->pos += len;
        }
    }
    return ret;
}

DPS_Status CBOR_EncodeString(DPS_Buffer* buffer, const char* str)
{
    size_t len = strlen(str) + 1;
    DPS_Status ret = EncodeUint(buffer, (uint32_t)len, CBOR_STRING);
    if (ret == DPS_OK) {
        if (DPS_BufferSpace(buffer) < len) {
            ret = DPS_ERR_OVERFLOW;
        } else if (str) {
            memcpy(buffer->pos, str, len);
            buffer->pos += len;
        }
    }
    return ret;
}

DPS_Status CBOR_DecodeUint(DPS_Buffer* buffer, uint64_t* n)
{
    uint8_t maj;
    DPS_Status ret;

    ret = DecodeUint(buffer, n, &maj);
    if ((ret == DPS_OK) && (maj != CBOR_UINT)) {
        ret = DPS_ERR_INVALID;
    }
    return ret;
}

DPS_Status CBOR_DecodeUint8(DPS_Buffer* buffer, uint8_t* n)
{
    uint64_t n64;
    uint8_t maj;
    DPS_Status ret;

    ret = DecodeUint(buffer, &n64, &maj);
    if ((ret == DPS_OK) && ((maj != CBOR_UINT) || (n64 > UINT8_MAX))) {
        ret = DPS_ERR_INVALID;
    }
    *n = (uint8_t)n64;
    return ret;
}

DPS_Status CBOR_DecodeInt(DPS_Buffer* buffer, int64_t* i)
{
    uint8_t maj;
    uint64_t n;
    DPS_Status ret;

    ret = DecodeUint(buffer, &n, &maj);
    if (ret == DPS_OK) {
        if (maj == CBOR_UINT) {
            *i = (int64_t)n;
            if (*i < 0) {
                return DPS_ERR_INVALID;
            }
        } else if (maj == CBOR_NEG) {
            *i = (int64_t)(~n);
            if (*i > 0) {
                return DPS_ERR_INVALID;
            }
        } else {
            return DPS_ERR_INVALID;
        }
    }
    return ret;
}

DPS_Status CBOR_DecodeBytes(DPS_Buffer* buffer, uint8_t** data, size_t* size)
{
    uint8_t maj;
    DPS_Status ret;
    uint64_t len;

    *data = NULL;
    *size = 0;
    ret = DecodeUint(buffer, &len, &maj);
    if (ret == DPS_OK) {
        if ((maj != CBOR_BYTES) || (len > DPS_BufferAvail(buffer))) {
            ret = DPS_ERR_INVALID;
        } else {
            if (len) {
                *data = buffer->pos;
                *size = len;
                buffer->pos += len;
            }
        }
    }
    return ret;
}

DPS_Status CBOR_DecodeString(DPS_Buffer* buffer, char** data, size_t* size)
{
    uint8_t maj;
    DPS_Status ret;
    uint64_t len;

    ret = DecodeUint(buffer, &len, &maj);
    if (ret == DPS_OK) {
        if ((maj != CBOR_STRING) || (len > DPS_BufferAvail(buffer))) {
            ret = DPS_ERR_INVALID;
        } else {
            *data = (char*)buffer->pos;
            *size = len;
            buffer->pos += len;
        }
    }
    return ret;
}

#pragma once

#include <stdint.h>
#include "stdbool.h"
#include <stdlib.h>

// code is download from
// https://github.com/cmcqueen/cobs-c/
// with changes

#define COBS_ENCODE_DST_BUF_LEN_MAX(SRC_LEN)    (((SRC_LEN) == 0u) ? 1u : ((SRC_LEN) + (((SRC_LEN) + 253u) / 254u)))
#define COBS_DECODE_DST_BUF_LEN_MAX(SRC_LEN)    (((SRC_LEN) == 0u) ? 0u : ((SRC_LEN) - 1u))
/*
 * For in-place encoding, the source data must be offset in the buffer by
 * the following amount (or more).
 */
#define COBS_ENCODE_SRC_OFFSET(SRC_LEN)         (((SRC_LEN) + 253u)/254u)

typedef enum CobsEncodeStatus
{
    COBS_ENCODE_OK                  = 0x00,
    COBS_ENCODE_NULL_POINTER        = 0x01,
    COBS_ENCODE_OUT_BUFFER_OVERFLOW = 0x02
} CobsEncodeStatus_e;

typedef struct CobsEncodeResult
{
    size_t              out_len;
    CobsEncodeStatus_e  status;
} CobsEncodeResult_e;

typedef enum CobsDecodeStatus
{
    COBS_DECODE_OK                  = 0x00,
    COBS_DECODE_NULL_POINTER        = 0x01,
    COBS_DECODE_OUT_BUFFER_OVERFLOW = 0x02,
    COBS_DECODE_ZERO_BYTE_IN_INPUT  = 0x04,
    COBS_DECODE_INPUT_TOO_SHORT     = 0x08
} CobsDecodeStatus_e;

typedef struct CobsDecodeResult
{
    size_t              out_len;
    CobsDecodeStatus_e  status;
} CobsDecodeResult_e;


#ifdef __cplusplus
extern "C" {
#endif

/* COBS-encode a string of input bytes.
 *
 * dst_buf_ptr:    The buffer into which the result will be written
 * dst_buf_len:    Length of the buffer into which the result will be written
 * src_ptr:        The byte string to be encoded
 * src_len         Length of the byte string to be encoded
 *
 * returns:        A struct containing the success status of the encoding
 *                 operation and the length of the result (that was written to
 *                 dst_buf_ptr)
 */
CobsEncodeResult_e cobsEncode(void * pDstBuf, size_t dstBufLen, const void * pSrcBuf, size_t srcLen);

/* Decode a COBS byte string.
 *
 * dst_buf_ptr:    The buffer into which the result will be written
 * dst_buf_len:    Length of the buffer into which the result will be written
 * src_ptr:        The byte string to be decoded
 * src_len         Length of the byte string to be decoded
 *
 * returns:        A struct containing the success status of the dwecoding
 *                 operation and the length of the result (that was written to
 *                 dst_buf_ptr)
 */
CobsDecodeResult_e cobsDecode(void * pDstBuf, size_t dstBufLen, const void * pSrcBuf, size_t srcLen);

#ifdef __cplusplus
}
#endif

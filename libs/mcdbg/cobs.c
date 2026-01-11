#include "cobs.h"

CobsEncodeResult_e cobsEncode(void * pDstBuf, size_t dstBufLen, const void * pSrcBuf, size_t srcLen)
{
    CobsEncodeResult_e  result          = { 0u, COBS_ENCODE_OK };
    const uint8_t *     pSrcRead        = pSrcBuf;
    const uint8_t *     pSrcEnd         = pSrcRead + srcLen;
    uint8_t *           pDstBufStart    = pDstBuf;
    uint8_t *           pDstBufEnd      = pDstBufStart + dstBufLen;
    uint8_t *           pDestCodeWrite  = pDstBuf;
    uint8_t *           pDestWrite      = pDestCodeWrite + 1u;
    uint8_t             srcByte         = 0u;
    uint8_t             searchLen       = 1u;
    /* First, do a NULL pointer check and return immediately if it fails. */
    if ((pDstBuf == NULL) || (pSrcBuf == NULL))
    {
        result.status = COBS_ENCODE_NULL_POINTER;
        return result;
    }
    if (srcLen != 0u)
    {
        /* Iterate over the source bytes */
        for (;;)
        {
            /* Check for running out of output buffer space */
            if (pDestWrite >= pDstBufEnd)
            {
                result.status |= COBS_ENCODE_OUT_BUFFER_OVERFLOW;
                break;
            }

            srcByte = *pSrcRead++;
            if (srcByte == 0u)
            {
                /* We found a zero byte */
                *pDestCodeWrite = searchLen;
                pDestCodeWrite = pDestWrite++;
                searchLen = 1u;
                if (pSrcRead >= pSrcEnd)
                {
                    break;
                }
            }
            else
            {
                /* Copy the non-zero byte to the destination buffer */
                *pDestWrite++ = srcByte;
                searchLen++;
                if (pSrcRead >= pSrcEnd)
                {
                    break;
                }
                if (searchLen == 0xFFu)
                {
                    /* We have a long string of non-zero bytes, so we need
                     * to write out a length code of 0xFF. */
                    *pDestCodeWrite = searchLen;
                    pDestCodeWrite = pDestWrite++;
                    searchLen = 1u;
                }
            }
        }
    }
    /* We've reached the end of the source data (or possibly run out of output buffer)
     * Finalise the remaining output. In particular, write the code (length) byte.
     * Update the pointer to calculate the final output length.
     */
    if (pDestCodeWrite >= pDstBufEnd)
    {
        /* We've run out of output buffer to write the code byte. */
        result.status |= COBS_ENCODE_OUT_BUFFER_OVERFLOW;
        pDestWrite = pDstBufEnd;
    }
    else
    {
        /* Write the last code (length) byte. */
        *pDestCodeWrite = searchLen;
    }
    /* Calculate the output length, from the value of pDestCodeWrite */
    result.out_len = (size_t)(pDestWrite - pDstBufStart);
    return result;
}


CobsDecodeResult_e cobsDecode(void * pDstBuf, size_t dstBufLen, const void * pSrcBuf, size_t srcBufLen)
{
    CobsDecodeResult_e  result              = { 0u, COBS_DECODE_OK };
    const uint8_t *     pSrcRead        = pSrcBuf;
    const uint8_t *     pSrcEnd         = pSrcRead + srcBufLen;
    uint8_t *           pDstBufStart   = pDstBuf;
    uint8_t *           pDestBufEnd     = pDstBufStart + dstBufLen;
    uint8_t *           pDestWrite       = pDstBuf;
    size_t              remainingBytes;
    uint8_t             srcByte;
    uint8_t             i;
    uint8_t             lenCode;
    /* First, do a NULL pointer check and return immediately if it fails. */
    if ((pDstBuf == NULL) || (pSrcBuf == NULL))
    {
        result.status = COBS_DECODE_NULL_POINTER;
        return result;
    }
    if (srcBufLen != 0u)
    {
        for (;;)
        {
            lenCode = *pSrcRead++;
            if (lenCode == 0u)
            {
                result.status |= COBS_DECODE_ZERO_BYTE_IN_INPUT;
                break;
            }
            lenCode--;

            /* Check length code against remaining input bytes */
            remainingBytes = (size_t)(pSrcEnd - pSrcRead);
            if (lenCode > remainingBytes)
            {
                result.status |= COBS_DECODE_INPUT_TOO_SHORT;
                lenCode = (uint8_t)remainingBytes;
            }

            /* Check length code against remaining output buffer space */
            remainingBytes = (size_t)(pDestBufEnd - pDestWrite);
            if (lenCode > remainingBytes)
            {
                result.status |= COBS_DECODE_OUT_BUFFER_OVERFLOW;
                lenCode = (uint8_t)remainingBytes;
            }

            for (i = lenCode; i != 0u; i--)
            {
                srcByte = *pSrcRead++;
                if (srcByte == 0u)
                {
                    result.status |= COBS_DECODE_ZERO_BYTE_IN_INPUT;
                }
                *pDestWrite++ = srcByte;
            }

            if (pSrcRead >= pSrcEnd)
                break;

            /* Add a zero to the end */
            if (lenCode != 0xFEu)
            {
                if (pDestWrite >= pDestBufEnd)
                {
                    result.status |= COBS_DECODE_OUT_BUFFER_OVERFLOW;
                    break;
                }
                *pDestWrite++ = '\0';
            }
        }
    }
    result.out_len = (size_t)(pDestWrite - pDstBufStart);
    return result;
}

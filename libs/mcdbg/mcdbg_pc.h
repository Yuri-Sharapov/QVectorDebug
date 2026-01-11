#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#pragma pack(push, 1)
typedef struct McDbgPacket
{
    uint8_t     type;
    uint32_t    len;
    uint8_t*    payload;
    uint16_t    crc;
} McDbgPacket_t;

typedef struct McDbgFrame
{
    uint8_t     type;
    uint16_t    seq;
    uint8_t     payload[12];
    uint16_t    crc;
} McDbgFrame_t;
#pragma pack(pop)

typedef enum McDbgType
{
    TYPE_CLI_IOCTRL = 0,
    TYPE_DBG_IOCTRL = 1,
    TYPE_CLI = 2,
    TYPE_DBG_FRAME1 = 3
} McDbgType_t;

int mcdbgPackMsg(uint8_t* pBuf, size_t bufLen, McDbgType_t type, uint8_t* pPayload, size_t payloadLen);
bool mcdbgCheckMsg(uint8_t* pBuf, size_t len);
int mcdbgEncode(uint8_t* pDst, size_t dstLen, uint8_t* pSrc, size_t srcLen);
int mcdbgDecode(uint8_t* pDst, size_t dstLen, uint8_t* pSrc, size_t srcLen);

#ifdef __cplusplus
}
#endif

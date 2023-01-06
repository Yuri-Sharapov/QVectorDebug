#pragma once

#include "platform.h"
#include "build_config.h"

namespace crc
{

uint16_t crc16_ccitt_update(uint16_t crc, uint8_t data);
uint16_t crc16_ccitt(uint8_t *pData, uint16_t size);

uint16_t crc16_update(uint16_t crc, uint8_t data);
uint16_t crc16(uint8_t *pData, uint16_t size);

uint8_t crc8_ccitt_update(uint8_t crc, uint8_t data);
uint8_t crc8_ccitt(uint8_t *pData, uint16_t size);

}

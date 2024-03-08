/*
 * This file is part of the QVectorDebug (https://github.com/Yuri-Sharapov/QVectorDebug).
 * Copyright (c) 2021 Yuri Sharapov.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "port.h"

const unsigned char Crc8Table[256] = {
    0x00, 0x31, 0x62, 0x53, 0xC4, 0xF5, 0xA6, 0x97,
    0xB9, 0x88, 0xDB, 0xEA, 0x7D, 0x4C, 0x1F, 0x2E,
    0x43, 0x72, 0x21, 0x10, 0x87, 0xB6, 0xE5, 0xD4,
    0xFA, 0xCB, 0x98, 0xA9, 0x3E, 0x0F, 0x5C, 0x6D,
    0x86, 0xB7, 0xE4, 0xD5, 0x42, 0x73, 0x20, 0x11,
    0x3F, 0x0E, 0x5D, 0x6C, 0xFB, 0xCA, 0x99, 0xA8,
    0xC5, 0xF4, 0xA7, 0x96, 0x01, 0x30, 0x63, 0x52,
    0x7C, 0x4D, 0x1E, 0x2F, 0xB8, 0x89, 0xDA, 0xEB,
    0x3D, 0x0C, 0x5F, 0x6E, 0xF9, 0xC8, 0x9B, 0xAA,
    0x84, 0xB5, 0xE6, 0xD7, 0x40, 0x71, 0x22, 0x13,
    0x7E, 0x4F, 0x1C, 0x2D, 0xBA, 0x8B, 0xD8, 0xE9,
    0xC7, 0xF6, 0xA5, 0x94, 0x03, 0x32, 0x61, 0x50,
    0xBB, 0x8A, 0xD9, 0xE8, 0x7F, 0x4E, 0x1D, 0x2C,
    0x02, 0x33, 0x60, 0x51, 0xC6, 0xF7, 0xA4, 0x95,
    0xF8, 0xC9, 0x9A, 0xAB, 0x3C, 0x0D, 0x5E, 0x6F,
    0x41, 0x70, 0x23, 0x12, 0x85, 0xB4, 0xE7, 0xD6,
    0x7A, 0x4B, 0x18, 0x29, 0xBE, 0x8F, 0xDC, 0xED,
    0xC3, 0xF2, 0xA1, 0x90, 0x07, 0x36, 0x65, 0x54,
    0x39, 0x08, 0x5B, 0x6A, 0xFD, 0xCC, 0x9F, 0xAE,
    0x80, 0xB1, 0xE2, 0xD3, 0x44, 0x75, 0x26, 0x17,
    0xFC, 0xCD, 0x9E, 0xAF, 0x38, 0x09, 0x5A, 0x6B,
    0x45, 0x74, 0x27, 0x16, 0x81, 0xB0, 0xE3, 0xD2,
    0xBF, 0x8E, 0xDD, 0xEC, 0x7B, 0x4A, 0x19, 0x28,
    0x06, 0x37, 0x64, 0x55, 0xC2, 0xF3, 0xA0, 0x91,
    0x47, 0x76, 0x25, 0x14, 0x83, 0xB2, 0xE1, 0xD0,
    0xFE, 0xCF, 0x9C, 0xAD, 0x3A, 0x0B, 0x58, 0x69,
    0x04, 0x35, 0x66, 0x57, 0xC0, 0xF1, 0xA2, 0x93,
    0xBD, 0x8C, 0xDF, 0xEE, 0x79, 0x48, 0x1B, 0x2A,
    0xC1, 0xF0, 0xA3, 0x92, 0x05, 0x34, 0x67, 0x56,
    0x78, 0x49, 0x1A, 0x2B, 0xBC, 0x8D, 0xDE, 0xEF,
    0x82, 0xB3, 0xE0, 0xD1, 0x46, 0x77, 0x24, 0x15,
    0x3B, 0x0A, 0x59, 0x68, 0xFF, 0xCE, 0x9D, 0xAC
};

static unsigned char crc8(unsigned char *pcBlock, unsigned char len)
{
    unsigned char crc = 0xFF;

    while (len--)
        crc = Crc8Table[crc ^ *pcBlock++];

    return crc;
}

void Port::portReadyRead()
{
    QByteArray array = m_port.readAll();
    if (m_currentProtocolType == TYPE_VECTOR)
        protocolParseData(array);
    else
        protocolParseFEsc(array);
}

Port::Port(QObject *parent) :
    QObject(parent)
{
    m_rxData.clear();
    m_rxRawData.clear();
    m_rxRawData.reserve(1024 * 128);
}

Port::~Port()
{
    emit finished();
}

bool Port::openPort(long _baudrate, QString _name)
{

    m_port.setPortName(_name);
    if (m_port.open(QIODevice::ReadWrite))
    {

        m_port.setBaudRate(_baudrate);
        m_port.setStopBits(QSerialPort::StopBits::TwoStop);

        connect(&m_port, &QSerialPort::readyRead, this, &Port::portReadyRead);

        m_timerNs.start();
        m_timerNs_1 = 0;
        m_rxRawData.clear();
        m_port.clear();
        m_errorsCnt = 0;

        return true;
    }
    else
    {
        return false;
    }
}

void Port::closePort(void)
{
    m_port.close();
}

void Port::process()
{

}

void Port::write(const QByteArray &data)
{
    if (m_port.isOpen())
    {
        m_port.write(data);
    }
}

void Port::protocolParseData(const QByteArray &data)
{
    enum protocolState_e
    {
        PROTOCOL_START,
        PROTOCOL_DATA
    };

    static protocolState_e _state = PROTOCOL_START;

    for (int i = 0; i < data.count(); i++)
    {
        uint8_t _c = static_cast<uint8_t>(data.constData()[i]);
        switch(_state)
        {
        case PROTOCOL_START:
            if (_c == PROTOCOL_START_BYTE)
            {
                _state = PROTOCOL_DATA;
                m_rxData.clear();
            }
            break;
        case PROTOCOL_DATA:
            if (static_cast<size_t>(m_rxData.count()) < sizeof(ProtocolData) - 1)
            {
                m_rxData.push_back(_c);
            }
            else
            {
                uint8_t _crc = 10;
                for (int k = 0; k < m_rxData.count(); k++)
                {
                    _crc += static_cast<uint8_t>(m_rxData.constData()[k]);
                }
                if (_crc == _c)
                {
                    m_rxData.push_back(_c);
                    const ProtocolData* _pData = reinterpret_cast<ProtocolData*>(m_rxData.data());

                    ChartVar _chartVar;
                    _chartVar.data[0] = _pData->data[0];
                    _chartVar.data[1] = _pData->data[1];
                    _chartVar.data[2] = _pData->data[2];
                    _chartVar.data[3] = _pData->data[3];

                    _chartVar.timeNs = m_timerNs.nsecsElapsed();

                    m_rxRawData.push_back(_chartVar);
                    // update gui if it need
                    if (_chartVar.timeNs - m_timerNs_1 > 1000000 * GUI_UPDATE_PERIOD_MS)
                    {

                        m_timerNs_1 = _chartVar.timeNs;
                        emit updatePlot(_chartVar.timeNs, _chartVar.data[0], _chartVar.data[1], _chartVar.data[2], _chartVar.data[3]);
                    }
                }
                else
                {
                    m_errorsCnt++;
                }
                _state = PROTOCOL_START;
            }
            break;
        default:
            _state = PROTOCOL_START;
            break;
        }
    }
}

void Port::protocolParseFEsc(const QByteArray &data)
{
    enum protocolState_e
    {
        PROTOCOL_START,
        PROTOCOL_TYPE,
        PROTOCOL_DATA
    };

    static protocolState_e _state = PROTOCOL_START;
    static bool _isExt = false;

    for (int i = 0; i < data.count(); i++)
    {
        uint8_t _c = static_cast<uint8_t>(data.constData()[i]);
        switch(_state)
        {
        case PROTOCOL_START:
            if (_c == PROTOCOL_START_BYTE)
            {
                _state = PROTOCOL_TYPE;
                m_rxData.clear();
                m_rxData.push_back(_c);
            }
            break;
        case PROTOCOL_TYPE:
            if (_c == TELEMETRY_STD)
            {
                _isExt = false;
                _state = PROTOCOL_DATA;
                m_rxData.push_back(_c);
            }
            else if (_c == TELEMETRY_EXT)
            {
                _isExt = true;
                _state = PROTOCOL_DATA;
                m_rxData.push_back(_c);
            }
            else
            {
                _state = PROTOCOL_START;
            }
            break;
        case PROTOCOL_DATA:
            if (_isExt)
            {
                if (static_cast<size_t>(m_rxData.count()) < sizeof(TlExt_t) - 1)
                {
                    m_rxData.push_back(_c);
                }
                else
                {
                    uint8_t _crc = crc8((uint8_t*)m_rxData.data(), sizeof(TlExt_t) - 1);

                    if (_crc == _c)
                    {
                        m_rxData.push_back(_c);
                        const TlExt_t* _pData = reinterpret_cast<TlExt_t*>(m_rxData.data());

                        ChartVar _chartVar;
                        _chartVar.data[0] = _pData->inputVoltage;
                        _chartVar.data[1] = _pData->inputCurrent;
                        _chartVar.data[2] = _pData->ppm;
                        _chartVar.data[3] = _pData->rpm;
                        _chartVar.data[4] = _pData->position;
                        _chartVar.data[5] = _pData->currentA;
                        _chartVar.data[6] = _pData->currentB;

                        _chartVar.timeNs = m_timerNs.nsecsElapsed();

                        m_rxRawData.push_back(_chartVar);
                        // update gui if it need
                        if (_chartVar.timeNs - m_timerNs_1 > 1000000 * GUI_UPDATE_PERIOD_MS)
                        {

                            m_timerNs_1 = _chartVar.timeNs;
                            emit updatePlot(_chartVar.timeNs, _chartVar.data[0], _chartVar.data[1], _chartVar.data[2], _chartVar.data[3]);
                        }
                    }
                    else
                    {
                        m_errorsCnt++;
                    }
                    _state = PROTOCOL_START;
                }
            }
            else
            {
                if (static_cast<size_t>(m_rxData.count()) < sizeof(TlStd_t) - 1)
                {
                    m_rxData.append(_c);
                }
                else
                {
                    uint8_t _crc = crc8((uint8_t*)m_rxData.data(), sizeof(TlStd_t) - 1);

                    if (_crc == _c)
                    {
                        m_rxData.push_back(_c);
                        const TlStd_t* _pData = reinterpret_cast<TlStd_t*>(m_rxData.data());

                        ChartVar _chartVar;
                        _chartVar.data[0] = _pData->inputVoltage;
                        _chartVar.data[1] = _pData->inputCurrent;
                        _chartVar.data[2] = _pData->ppm;
                        _chartVar.data[3] = _pData->rpm;

                        _chartVar.timeNs = m_timerNs.nsecsElapsed();

                        m_rxRawData.push_back(_chartVar);
                        // update gui if it need
                        if (_chartVar.timeNs - m_timerNs_1 > 1000000 * GUI_UPDATE_PERIOD_MS)
                        {

                            m_timerNs_1 = _chartVar.timeNs;
                            emit updatePlot(_chartVar.timeNs, _chartVar.data[0], _chartVar.data[1], _chartVar.data[2], _chartVar.data[3]);
                        }
                    }
                    else
                    {
                        m_errorsCnt++;
                    }
                    _state = PROTOCOL_START;
                }
            }
            break;
        default:
            _state = PROTOCOL_START;
            break;
        }
    }
}

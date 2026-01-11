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

void Port::portReadyRead()
{
    if (m_currentProtocolType == TYPE_V1)
    {
        protocolParseData(m_port.readAll());
    }
    else
    {
        m_rxData.append(m_port.readAll());
        while (1)
        {
            int zeroIndex = m_rxData.indexOf('\0');
            if (zeroIndex < 0)
                break; // полного пакета ещё нет
            // Один полный COBS-пакет (без завершающего 0)
            QByteArray cobsPacket = m_rxData.left(zeroIndex);
            // Удаляем пакет + 0 из буфера
            m_rxData.remove(0, zeroIndex + 1);
            // Декодируем
            protocolParseMcdbg(cobsPacket);
        }
    }
}

void Port::cliTx(const QByteArray &array)
{
    if (!m_port.isOpen())
        return;
    const size_t bufSize = 2048;
    static uint8_t srcBuf[bufSize];
    static uint8_t dstBuf[bufSize];
    int srcSize = mcdbgPackMsg(srcBuf, bufSize, TYPE_CLI, (uint8_t*)array.data(), array.size());
    int endSize = mcdbgEncode(dstBuf, bufSize, srcBuf, srcSize);
    m_port.write((const char*)dstBuf, endSize);
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
        m_guiLastUpdateTimeNs = 0;
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
    if (m_port.isOpen())
    {
        m_port.clear();
        m_port.close();
    }
}

void Port::write(const QByteArray &data)
{
    if (m_port.isOpen())
        m_port.write(data);
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
            if (static_cast<size_t>(m_rxData.count()) < sizeof(VectorProtocolData) - 1)
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
                    const VectorProtocolData* _pData = reinterpret_cast<VectorProtocolData*>(m_rxData.data());

                    ChartVar _chartVar;
                    _chartVar.data[0] = _pData->data[0];
                    _chartVar.data[1] = _pData->data[1];
                    _chartVar.data[2] = _pData->data[2];
                    _chartVar.data[3] = _pData->data[3];
                    _chartVar.timeNs = m_timerNs.nsecsElapsed();
                    m_rxRawData.push_back(_chartVar);
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

void Port::protocolParseMcdbg(const QByteArray &data)
{
    const size_t bufSize = 16 * 1024;
    static uint8_t buf[bufSize];
    int res = mcdbgDecode(buf, bufSize, (uint8_t*)data.data(), data.size());
    if (res > 0)
    {
        McDbgPacket_t* pMsg = (McDbgPacket_t*)buf;
        if (mcdbgCheckMsg(buf, res))
        {
            switch (pMsg->type)
            {
            case TYPE_CLI:
            {
                qDebug() << "Port: Cli rx data read";
                QByteArray ba((char*)&buf[5], pMsg->len);
                emit cliRx(ba);
            }   break;
            case TYPE_DBG_FRAME1:
            {
                for (size_t i = 0; i < pMsg->len / 14; i++)
                {
                    int16_t* pvars = (int16_t*)&buf[5 + i * 14];
                    updatePlot(pvars[0]&0xFF, pvars[1], pvars[2], pvars[3], pvars[4], pvars[5]);
                }
             }  break;
            default:
                break;
            }
        }
        else
        {
        }
    }
    if (res == -1)
        qDebug() << "Port: COBS_DECODE_NULL_POINTER";
    if (res == -2)
        qDebug() << "Port: COBS_DECODE_OUT_BUFFER_OVERFLOW";
    if (res == -3)
        qDebug() << "Port: COBS_DECODE_ZERO_BYTE_IN_INPUT";
}


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
    QByteArray array = m_port.readAll();
    if (m_currentProtocolType == TYPE_CLASSIC)
        protocolParseData(array);
    else
        protocolParseTdfp(array);
}

Port::Port(QObject *parent) :
    QObject(parent)
{
    m_rxData.clear();
    m_rxRawData.clear();
    m_rxRawData.reserve(1024 * 10);
}

Port::~Port()
{
    emit finished();
}

bool Port::openPort(long _baudrate, QString _name)
{
    m_port.setPortName(_name);
    m_port.setBaudRate(_baudrate);
    m_port.setStopBits(QSerialPort::StopBits::TwoStop);

    if (m_port.open(QIODevice::ReadWrite))
    {
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
            if (static_cast<size_t>(m_rxData.count()) < sizeof(ProtocolData_t) - 1)
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
                    const ProtocolData_t* _pData = reinterpret_cast<ProtocolData_t*>(m_rxData.data());

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

void Port::protocolParseTdfp(const QByteArray &data)
{
    for (int i = 0; i < data.size(); i++)
    {
        if (tlpParseByte(data.data()[i]))
        {
            const ProtocolData_t* _pData = reinterpret_cast<ProtocolData_t*>(m_tlpBuf.data());

            uint8_t _crc = 10;
            for (int k = 0; k < m_tlpBuf.count() - 1; k++)
            {
                _crc += static_cast<uint8_t>(m_tlpBuf.constData()[k]);
            }
            if (_crc == _pData->checkSum)
            {
                ChartVar _chartVar;
                _chartVar.data[0] = _pData->data[0];
                _chartVar.data[1] = _pData->data[1];
                _chartVar.data[2] = _pData->data[2];
                _chartVar.data[3] = _pData->data[3];

                _chartVar.timeNs = m_timerNs.nsecsElapsed();

                m_rxRawData.push_back(_chartVar);

                if (_chartVar.timeNs - m_timerNs_1 > 1000000 * 100)
                {

                    m_timerNs_1 = _chartVar.timeNs;
                    emit updatePlot(_chartVar.timeNs, _chartVar.data[0], _chartVar.data[1], _chartVar.data[2], _chartVar.data[3]);
                }
            }
            else
            {
                m_errorsCnt++;
            }
        }
    }
}

bool Port::tlpParseByte(uint8_t byte)
{
    static bool rxIsData = false;

    if (byte == TLP_BYTE_START)
    {
        m_tlpBuf.clear();
        rxIsData = true;
        return false;
    }
    if (byte == TLP_BYTE_STOP)
    {
        rxIsData = true;
        return true;
    }
    if (rxIsData)
    {
        if (byte == TLP_BYTE_ESC)
        {
            rxIsData = false;
            return false;
        }
        m_tlpBuf.push_back(byte);
        return false;
    }
    else
    {
        m_tlpBuf.push_back(byte + TLP_BYTE_ESC);
        rxIsData = true;
        return false;
    }
    // Oops how we get here?
    return false;
}

void Port::tlpPutData(uint8_t *pData, uint32_t size)
{
    if (!m_port.isOpen())
    {
        qDebug() << "FspLink: error, tlpPutData not connected";
        return;
    }

    QByteArray data;
    data.push_back(TLP_BYTE_START);
    while (size--)
    {
        if (*pData == TLP_BYTE_START || *pData == TLP_BYTE_STOP || *pData == TLP_BYTE_ESC)
        {
            data.push_back(TLP_BYTE_ESC);
            data.push_back(*pData - TLP_BYTE_ESC);
            pData++;
        }
        else
        {
            data.push_back(*pData++);
        }
    }
    data.push_back(TLP_BYTE_STOP);

    m_port.write(data);
    m_port.waitForBytesWritten(3000);
}

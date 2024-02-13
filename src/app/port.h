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
#pragma once

#include <QObject>
#include <QDebug>
#include <QElapsedTimer>
#include <QSerialPort>
#include <QSerialPortInfo>

#define PROTOCOL_START_BYTE     0x55
#define GUI_UPDATE_PERIOD_MS    100

#define TLP_BYTE_START  0xFF
#define TLP_BYTE_STOP   0xFE
#define TLP_BYTE_ESC    0xFD

class Port : public QObject
{
    Q_OBJECT

#pragma pack(push, 1)
    struct ProtocolData
    {
        int16_t    data[4];
        uint8_t    checkSum;
    };
#pragma pack( pop )

public:
    struct ChartVar
    {
        uint64_t    timeNs;
        int16_t     data[4];
    };

    enum ProcotolType
    {
        TYPE_CLASSIC = 0,
        TYPE_TDFP = 1
    };

    explicit Port(QObject *parent = 0);
    ~Port();

    bool openPort(long _baudrate, QString _name);
    void closePort();

    bool isOpenPort(void)
    {
        return m_port.isOpen();
    }

    QVector<ChartVar>* getChartVars(void)
    {
        return &m_rxRawData;
    }

    int getErrorsCnt(void)
    {
        return m_errorsCnt;
    }

    void setProtocolType(ProcotolType type)
    {
        m_currentProtocolType = type;
    }

    ProcotolType getProtocolType(void)
    {
        return m_currentProtocolType;
    }

public slots:
    void process();
    void write(const QByteArray &data);

private slots:
    void portReadyRead();

signals:
    void finished();
    void updatePlot(qint64 timeNs, short var1, short var2, short var3, short var4);

private:
    void protocolParseData(const QByteArray &data);
    void protocolParseTdfp(const QByteArray &data);

    bool tlpParseByte(uint8_t byte);
    void tlpPutData(uint8_t *pData, uint32_t size);

public:
    QSerialPort         m_port;

private:
    uint32_t            m_baudrate;
    QString             m_name;

    QVector<uint8_t>    m_tlpBuf;

    QByteArray          m_rxData;
    QVector<ChartVar>   m_rxRawData;
    QElapsedTimer       m_timerNs;
    uint64_t            m_timerNs_1;

    uint32_t            m_errorsCnt = 0;
    ProcotolType        m_currentProtocolType;
};

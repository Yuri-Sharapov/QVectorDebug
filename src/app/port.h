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

#define TELEMETRY_STD     0x0A
#define TELEMETRY_EXT     0x0B

class Port : public QObject
{
    Q_OBJECT

#pragma pack(push, 1)
    struct ProtocolData
    {
        int16_t    data[4];
        uint8_t    checkSum;
    };

    typedef struct TlStd
    {
        uint8_t     start;
        uint8_t     type;
        uint16_t    inputVoltage;
        uint16_t    inputCurrent;
        uint16_t    ppm;
        uint16_t    rpm;
        uint8_t     crc;
    } TlStd_t;

    typedef struct TlExt
    {
        uint8_t     start;
        uint8_t     type;
        uint16_t    inputVoltage;
        uint16_t    inputCurrent;
        uint16_t    ppm;
        uint16_t    rpm;
        uint16_t    position;
        int16_t     currentA;
        int16_t     currentB;
        uint8_t     crc;
    } TlExt_t;
#pragma pack( pop )

public:
    enum EscGraphs
    {
        TEMPERATURE,
        VOLTAGE,
        CURRENT,
        POWER,
        PPM,
        RPM,
        POS,
        CA,
        CB,
        CC
    };

    struct ChartVar
    {
        uint64_t    timeNs;
        int16_t     data[14];
    };

    enum ProcotolType
    {
        TYPE_VECTOR = 0,
        TYPE_FESC = 1
    };

    enum RxState
    {
        STATE_WAIT,
        STATE_DATA_STD,
        STATE_DATA_EXT,
        STATE_CRC
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

    QVector<ChartVar>* getChartExtVars(void)
    {
        return &m_rxRawExtData;
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
    void protocolParseFEsc(const QByteArray &data);

public:
    QSerialPort         m_port;

private:
    uint32_t            m_baudrate;
    QString             m_name;

    QByteArray          m_rxData;
    QVector<ChartVar>   m_rxRawData;
    QVector<ChartVar>   m_rxRawExtData;
    QElapsedTimer       m_timerNs;
    uint64_t            m_timerNs_1;
    uint32_t            m_errorsCnt = 0;
    ProcotolType        m_currentProtocolType;
};

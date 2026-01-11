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

#include "libs/mcdbg/mcdbg_pc.h"

#define PROTOCOL_START_BYTE     0x55
#define GUI_UPDATE_PERIOD_MS    100

class Port : public QObject
{
    Q_OBJECT

#pragma pack(push, 1)
    struct VectorProtocolData
    {
        int16_t    data[4];
        uint8_t    checkSum;
    };
#pragma pack( pop )

public:
    struct ChartVar
    {
        uint64_t    timeNs;
        int16_t     data[16];
    };

    enum ProcotolType
    {
        TYPE_V1 = 0,
        TYPE_V2 = 1
    };

    explicit Port(QObject *parent = 0);
    ~Port();

    bool openPort(long _baudrate, QString _name);
    void closePort();
    bool isPortOpen(void)
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
    void write(const QByteArray &data);
    void process() {}
    void cliTx(const QByteArray &array);
private slots:
    void portReadyRead();
signals:
    void finished();
    void updatePlot(int v1, int v2, int v3, int v4, int v5, int v6);
    void cliRx(const QByteArray &array);
private:
    void protocolParseData(const QByteArray &data);
    void protocolParseMcdbg(const QByteArray &data);
public:
    QSerialPort         m_port;
private:
    uint32_t            m_baudrate;
    QString             m_name;
    QByteArray          m_rxData;
    QVector<ChartVar>   m_rxRawData;
    QElapsedTimer       m_timerNs;
    uint64_t            m_guiLastUpdateTimeNs;
    uint32_t            m_errorsCnt = 0;
    ProcotolType        m_currentProtocolType;
};

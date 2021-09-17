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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QElapsedTimer>

#include "chart_widget.h"

#define PROTOCOL_START_BYTE     0x55

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class Port : public QObject
{
    Q_OBJECT

    typedef struct __attribute__ ((packed)) ProtocolData_struct
    {
        int16_t    data[4];
        uint8_t     checkSum;
    }ProtocolData_t;

private slots:
    void PortReadyRead();
public:
    struct ChartVar
    {
        qint64  timeNs;
        short data[4];
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

    QSerialPort m_port;
public slots:
    void process();
    void write(const QByteArray &data);
signals:
    void finished();
    void updatePlot(qint64 timeNs, short var1, short var2, short var3, short var4);
private:
    void protocolParseData(const QByteArray &data);

    long        m_baudrate;
    QString     m_name;

    QByteArray      m_rxData;
    QVector<ChartVar> m_rxRawData;
    QElapsedTimer   m_timerNs;
    qint64          m_timerNs_1;

    int             m_errorsCnt = 0;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_btnConnect_clicked();

    void on_cbEnabled_1_stateChanged(int arg1);
    void on_cbEnabled_2_stateChanged(int arg1);
    void on_cbEnabled_3_stateChanged(int arg1);
    void on_cbEnabled_4_stateChanged(int arg1);

    void on_btnSend_clicked();

    void on_actionExit_triggered();
    void on_actionSave_triggered();
    void on_actionOpen_triggered();

    void on_PortUpdatePlot(qint64 timeNs, short var1, short var2, short var3, short var4);
private:
    void showStatusMessage(const QString &message);

    void serialSetup(void);
    void serialConnect(void);
    void serialDisconnect(void);

    void openChart(QVector<Port::ChartVar>* pVars);

    Ui::MainWindow  *m_pUi;
    Port            *m_pPort;
    ChartWidget     *m_pChart = nullptr;
    QLabel          *m_pStatus = nullptr;
};
#endif // MAINWINDOW_H

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

#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_pUi(new Ui::MainWindow)
{
    m_pUi->setupUi(this);

    m_pChart = new ChartWidget();
    m_pChart->setObjectName(QString::fromUtf8("wgtChart"));
    m_pUi->horizontalLayout->addWidget(m_pChart);

    m_pUi->cbEnabled_1->setChecked(true);
    m_pUi->cbEnabled_2->setChecked(true);
    m_pUi->cbEnabled_3->setChecked(true);
    m_pUi->cbEnabled_4->setChecked(true);

    m_pPort = new Port;
    // create tight thread
    QThread *threadNew = new QThread;
    // move class to new thread
    m_pPort->moveToThread(threadNew);
    // move port to thread
    m_pPort->m_port.moveToThread(threadNew);
    connect(threadNew, SIGNAL(started()), m_pPort, SLOT(process()));
    connect(m_pPort, SIGNAL(finished()), threadNew, SLOT(quit()));
    connect(m_pPort, &Port::updatePlot, this, &MainWindow::on_PortUpdatePlot);
    // run new thread
    threadNew->start();

    serialSetup();

    m_pStatus = new QLabel(this);
    m_pUi->statusbar->addWidget(m_pStatus);

    // disable unused
    m_pUi->btnSend->setEnabled(false);
    m_pUi->cbSign_1->setEnabled(false);
    m_pUi->cbSign_2->setEnabled(false);
    m_pUi->cbSign_3->setEnabled(false);
    m_pUi->cbSign_4->setEnabled(false);
    m_pUi->leSendVar_1->setEnabled(false);
    m_pUi->leSendVar_2->setEnabled(false);
}

MainWindow::~MainWindow()
{
    delete m_pUi;
    delete m_pChart;
    delete m_pStatus;
    delete m_pPort;
}

void MainWindow::on_PortUpdatePlot(qint64 timeNs, short var1, short var2, short var3, short var4)
{
    m_pChart->appendData(timeNs, var1, var2, var3, var4);
    m_pChart->updateChart();
}

void MainWindow::on_btnConnect_clicked()
{
    if (m_pPort->isOpenPort())
    {
        serialDisconnect();
        openChart(m_pPort->getChartVars());
    }
    else
    {
        serialConnect();
        m_pChart->startChart();
    }
}

void MainWindow::on_cbEnabled_1_stateChanged(int arg1)
{
    if (arg1 == 2)
        m_pChart->changeVisablilty(0, true);
    else
        m_pChart->changeVisablilty(0, false);
}

void MainWindow::on_cbEnabled_2_stateChanged(int arg1)
{
    if (arg1 == 2)
        m_pChart->changeVisablilty(1, true);
    else
        m_pChart->changeVisablilty(1, false);
}

void MainWindow::on_cbEnabled_3_stateChanged(int arg1)
{
    if (arg1 == 2)
        m_pChart->changeVisablilty(2, true);
    else
        m_pChart->changeVisablilty(2, false);
}

void MainWindow::on_cbEnabled_4_stateChanged(int arg1)
{
    if (arg1 == 2)
        m_pChart->changeVisablilty(3, true);
    else
        m_pChart->changeVisablilty(3, false);
}

void MainWindow::on_btnSend_clicked()
{

}

void MainWindow::on_actionExit_triggered()
{
    close();
}

void MainWindow::on_actionSave_triggered()
{
    if (m_pPort->getChartVars()->count() && !m_pPort->isOpenPort())
    {
        QDateTime currentDateTime = QDateTime::currentDateTime();
        QString data = "QVector-";
        data.append(currentDateTime.toString( "yy.dd.mm-hh.mm.ss" ));

        QString nameFile = QFileDialog::getSaveFileName(this, tr("Save Log"), data,
                                                            tr("File Name (*.vct);;C++ File (*.cpp *.h)"));
            if (nameFile != "")
            {
                QFile file(nameFile);

                if (file.open(QIODevice::ReadWrite))
                {
                    for (Port::ChartVar var : *m_pPort->getChartVars())
                    {
                        file.write(reinterpret_cast<char*>(&var), sizeof(Port::ChartVar));
                    }
                    file.flush();
                    file.close();
                }
                else
                {
                    QMessageBox::critical(this, tr("Errore"), tr("Non posso salvare il file"));
                    return;
                }
            }
    }
}

void MainWindow::on_actionOpen_triggered()
{
    QString filename = QFileDialog::getOpenFileName(this, tr("Open Vector file"), "./",
                                                        tr("Vector Files (*.vct)"));

    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly))
        return;

    QByteArray fileRaw = file.readAll();
    file.close();

    const Port::ChartVar *pVars = reinterpret_cast<Port::ChartVar*>(fileRaw.data());
    QVector<Port::ChartVar> _vectVars;
    _vectVars.clear();

    for (size_t i = 0; i < fileRaw.size()/sizeof(Port::ChartVar); i++)
    {
        _vectVars.push_back(*pVars++);
    }
    openChart(&_vectVars);
}

void MainWindow::serialSetup(void)
{
    m_pUi->cbBaudrate->addItem(QStringLiteral("9600"), QSerialPort::Baud9600);
    m_pUi->cbBaudrate->addItem(QStringLiteral("19200"), QSerialPort::Baud19200);
    m_pUi->cbBaudrate->addItem(QStringLiteral("38400"), QSerialPort::Baud38400);
    m_pUi->cbBaudrate->addItem(QStringLiteral("57600"), QSerialPort::Baud57600);
    m_pUi->cbBaudrate->addItem(QStringLiteral("115200"), QSerialPort::Baud115200);
    m_pUi->cbBaudrate->addItem(tr("Custom"));
    m_pUi->cbBaudrate->setCurrentIndex(0);

    m_pUi->cbBaudrate->setInsertPolicy(QComboBox::NoInsert);
    connect(m_pUi->cbBaudrate,  QOverload<int>::of(&QComboBox::currentIndexChanged),
    [this](int idx)
    {
        QIntValidator   *m_intValidator = nullptr;
        const bool isCustomBaudRate = !m_pUi->cbBaudrate->itemData(idx).isValid();
        m_pUi->cbBaudrate->setEditable(isCustomBaudRate);

        if (isCustomBaudRate)
        {
            m_pUi->cbBaudrate->clearEditText();
            QLineEdit *edit = m_pUi->cbBaudrate->lineEdit();
            edit->setValidator(m_intValidator);
        }
    });

    QList<QSerialPortInfo> avaliablePorts = QSerialPortInfo::availablePorts();

    for (QSerialPortInfo port : avaliablePorts)
    {
        m_pUi->cbPort->addItem(port.portName());
    }
}

void MainWindow::serialConnect()
{
    if (m_pPort->openPort(m_pUi->cbBaudrate->itemData(m_pUi->cbBaudrate->currentIndex()).toInt(),
                        m_pUi->cbPort->currentText()))
    {
        showStatusMessage(tr("Connected to %1: %2").arg(m_pUi->cbPort->currentText())
                                                   .arg(m_pUi->cbBaudrate->currentText()));
        m_pUi->btnConnect->setText("Disconnect");
    }
    else
    {
        showStatusMessage(tr("Open error"));
    }
}

void MainWindow::serialDisconnect()
{
    m_pPort->closePort();
    showStatusMessage(("Disconnected | Errors: " + QString::number(m_pPort->getErrorsCnt())));
    m_pUi->btnConnect->setText("Connect");
}

void MainWindow::openChart(QVector<Port::ChartVar> *pVars)
{
    m_pChart->startChart();
    qint64 timePrevNs = 0;
    for(Port::ChartVar var : *pVars)
    {
        if (timePrevNs == 0)
        {
            timePrevNs = var.timeNs;
            m_pChart->appendData(var.timeNs, var.data[0], var.data[1], var.data[2], var.data[3]);
        }
        else
        {
            if (timePrevNs/1000000U != var.timeNs/1000000U)
            {
                m_pChart->appendData(var.timeNs, var.data[0], var.data[1], var.data[2], var.data[3]);
            }

            timePrevNs = var.timeNs;
        }

    }
    m_pChart->updateChart();
}

void MainWindow::showStatusMessage(const QString &message)
{
    m_pStatus->setText(message);
}

void Port::portReadyRead()
{
    QByteArray array = m_port.readAll();
    protocolParseData(array);
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

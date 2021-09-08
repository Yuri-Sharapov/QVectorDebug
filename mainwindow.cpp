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

    m_pSerial = new QSerialPort(this);

    serialSetup();

    m_pStatus = new QLabel(this);
    m_pUi->statusbar->addWidget(m_pStatus);

    m_rxData.clear();
    m_rxRawData.clear();
    m_rxRawData.reserve(1024 * 10);
}

MainWindow::~MainWindow()
{
    delete m_pUi;
    delete m_pChart;
    delete m_pStatus;

    if (m_pSerial->isOpen())
        m_pSerial->close();

    delete m_pSerial;
}

void MainWindow::on_btnConnect_clicked()
{
    if (m_pSerial->isOpen())
    {
        serialDisconnect();
        m_pChart->startChart();
        for(ChartVar var : m_rxRawData)
        {
            m_pChart->appendData(var.timeNs, var.data[0], var.data[1], var.data[2], var.data[3]);
        }
        m_pChart->updateChart();
    }
    else
    {
        serialConnect();
        m_timerNs.start();
        m_timerNs_1 = 0;
        m_pChart->startChart();
        m_rxRawData.clear();
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

}

void MainWindow::on_actionOpen_triggered()
{

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
    m_pSerial->setPortName(m_pUi->cbPort->currentText());
    m_pSerial->setBaudRate(m_pUi->cbBaudrate->itemData(m_pUi->cbBaudrate->currentIndex()).toInt());

    m_rxData.clear();
    // open serial port
    if (m_pSerial->open(QIODevice::ReadWrite))
    {
        showStatusMessage(tr("Connected to %1: %2").arg(m_pUi->cbPort->currentText())
                                                   .arg(m_pUi->cbBaudrate->currentText()));
        m_pUi->btnConnect->setText("Disconnect");

        connect(m_pSerial, &QSerialPort::readyRead, this, &MainWindow::serialReadyRead);
    }
    else
    {
        //QMessageBox::critical(this, tr("Error"), m_serial->errorString());
        showStatusMessage(tr("Open error"));
    }
}

void MainWindow::serialDisconnect()
{
    m_pSerial->close();
    showStatusMessage(tr("Disconnected"));
    m_pUi->btnConnect->setText("Connect");
}

void MainWindow::serialReadyRead()
{
    QByteArray array = m_pSerial->readAll();
    protocolParseData(array);
}

void MainWindow::protocolParseData(const QByteArray &data)
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

                    if (_chartVar.timeNs - m_timerNs_1 > 1000000 * 100)
                    {

                        m_timerNs_1 = _chartVar.timeNs;
                        m_pChart->appendData(_chartVar.timeNs, _chartVar.data[0], _chartVar.data[1], _chartVar.data[2], _chartVar.data[3]);
                        m_pChart->updateChart();
                    }
                }
                else
                {
                    m_errorsCnt++;
                    showStatusMessage("Errors Count: " + QString::number(m_errorsCnt));

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

void MainWindow::showStatusMessage(const QString &message)
{
    m_pStatus->setText(message);
}

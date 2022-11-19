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
    , m_pPort(new Port)
    , m_pChart(new ChartWidget)
    , m_pSettings(new QSettings("settings.ini",QSettings::IniFormat))
{
    m_pUi->setupUi(this);

    m_pChart->setObjectName(QString::fromUtf8("wgtChart"));
    m_pUi->layChart->addWidget(m_pChart);

    m_pUi->cbEnabled_1->setChecked(true);
    m_pUi->cbEnabled_2->setChecked(true);
    m_pUi->cbEnabled_3->setChecked(true);
    m_pUi->cbEnabled_4->setChecked(true);

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

    m_pStatus = new QLabel(this);
    m_pUi->statusbar->addWidget(m_pStatus);

    // disable unused
    m_pUi->btnSend->setEnabled(true);
    m_pUi->cbSign_1->setEnabled(false);
    m_pUi->cbSign_2->setEnabled(false);
    m_pUi->cbSign_3->setEnabled(false);
    m_pUi->cbSign_4->setEnabled(false);
    m_pUi->leSendVar_1->setEnabled(true);
    m_pUi->leSendVar_2->setEnabled(true);


    m_paletteWhite = qApp->palette();

    m_paletteBlack.setColor(QPalette::Window,QColor(53,53,53));
    m_paletteBlack.setColor(QPalette::WindowText,Qt::white);
    m_paletteBlack.setColor(QPalette::Disabled,QPalette::WindowText,QColor(127,127,127));
    m_paletteBlack.setColor(QPalette::Base,QColor(42,42,42));
    m_paletteBlack.setColor(QPalette::AlternateBase,QColor(66,66,66));
    m_paletteBlack.setColor(QPalette::ToolTipBase,Qt::white);
    m_paletteBlack.setColor(QPalette::ToolTipText,Qt::white);
    m_paletteBlack.setColor(QPalette::Text,Qt::white);
    m_paletteBlack.setColor(QPalette::Disabled,QPalette::Text,QColor(127,127,127));
    m_paletteBlack.setColor(QPalette::Dark,QColor(35,35,35));
    m_paletteBlack.setColor(QPalette::Shadow,QColor(20,20,20));
    m_paletteBlack.setColor(QPalette::Button,QColor(53,53,53));
    m_paletteBlack.setColor(QPalette::ButtonText,Qt::white);
    m_paletteBlack.setColor(QPalette::Disabled,QPalette::ButtonText,QColor(127,127,127));
    m_paletteBlack.setColor(QPalette::BrightText,Qt::red);
    m_paletteBlack.setColor(QPalette::Link,QColor(42,130,218));
    m_paletteBlack.setColor(QPalette::Highlight,QColor(42,130,218));
    m_paletteBlack.setColor(QPalette::Disabled,QPalette::Highlight,QColor(80,80,80));
    m_paletteBlack.setColor(QPalette::HighlightedText,Qt::white);
    m_paletteBlack.setColor(QPalette::Disabled,QPalette::HighlightedText,QColor(127,127,127));

    serialSetup();
    restoreSettings();
}

MainWindow::~MainWindow()
{
    saveSettings();

    delete m_pUi;
    delete m_pPort;
    delete m_pChart;
    delete m_pSettings;
    delete m_pStatus;
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
    QByteArray dataToSend;
    // start byte
    dataToSend.append(0x55);
    // data bytes
    int16_t data1 = static_cast<int16_t>(m_pUi->leSendVar_1->text().toInt());
    int16_t data2 = static_cast<int16_t>(m_pUi->leSendVar_2->text().toInt());
    // place new text into line edits to avoid empty lines
    m_pUi->leSendVar_1->setText(QString::number(data1));
    m_pUi->leSendVar_2->setText(QString::number(data2));

    dataToSend.append(data1 & 0xFF);
    dataToSend.append(data1 >> 8);

    dataToSend.append(data2 & 0xFF);
    dataToSend.append(data2 >> 8);

    // check sum
    uint8_t chkSum = 10;

    for (int i = 1; i < dataToSend.count() - 1; i++) {
        chkSum += dataToSend.at(i);
    }

    dataToSend.append(chkSum);

    m_pPort->write(dataToSend);
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
    QString filename = QFileDialog::getOpenFileName(this, tr("Open Vector file"), "./",tr("Vector Files (*.vct)"));

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
        QIntValidator   *m_intValidator =  new QIntValidator(0, 4000000, this);
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
    unsigned int baudrate;
    if (m_pUi->cbBaudrate->currentIndex() == m_pUi->cbBaudrate->count() - 1)
    {
        baudrate = m_pUi->cbBaudrate->currentText().toInt();
    }
    else
    {
        baudrate = m_pUi->cbBaudrate->itemData(m_pUi->cbBaudrate->currentIndex()).toInt();
    }

    if (m_pPort->openPort(baudrate,
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
                m_pChart->appendData(var.timeNs, var.data[0], var.data[1], var.data[2], var.data[3]);

            timePrevNs = var.timeNs;
        }

    }
    m_pChart->updateChart();
}

void MainWindow::showStatusMessage(const QString &message)
{
    m_pStatus->setText(message);
}

void MainWindow::restoreSettings()
{
    if (!m_pSettings->contains("wnd/height"))
        m_pSettings->setValue("wnd/high", this->height());
    if (!m_pSettings->contains("wnd/width"))
        m_pSettings->setValue("wnd/width", this->width());
    if (!m_pSettings->contains("wnd/maximized"))
        m_pSettings->setValue("wnd/maximized", 0);
    if (!m_pSettings->contains("wnd/left_panel"))
        m_pSettings->setValue("wnd/left_panel", m_pUi->splitter->saveState());
    if (!m_pSettings->contains("wnd/left_panel_tab"))
        m_pSettings->setValue("wnd/left_panel_tab", m_pUi->tabWidget->currentIndex());

    if (!m_pSettings->contains("wnd/theme"))
        m_pSettings->setValue("wnd/theme", static_cast<int>(m_uiTheme));

    if (!m_pSettings->contains("port/protocol"))
        m_pSettings->setValue("port/protocol", static_cast<int>(m_pPort->getProtocolType()));
    if (!m_pSettings->contains("port/name"))
        m_pSettings->setValue("port/name", m_pUi->cbPort->currentIndex());
    if (!m_pSettings->contains("port/baudrate"))
        m_pSettings->setValue("port/baudrate", m_pUi->cbBaudrate->currentIndex());

    m_pSettings->sync();
    // window
    this->resize(m_pSettings->value("wnd/width").toInt(), m_pSettings->value("wnd/high").toInt());
    if (m_pSettings->value("wnd/maximized").toInt() == 1)
    {
        showMaximized();
    }
    // left panel
    QByteArray splitterState = m_pSettings->value("wnd/left_panel").toByteArray();
    m_pUi->splitter->restoreState(splitterState);
    m_pUi->tabWidget->setCurrentIndex(m_pSettings->value("wnd/left_panel_tab").toInt());
    
    // theme
    m_uiTheme = static_cast<ThemeSelector>(m_pSettings->value("wnd/theme").toInt());
    updateTheme();
    // port type
    m_pPort->setProtocolType(static_cast<Port::ProcotolType>(m_pSettings->value("port/protocol").toInt()));
    if (m_pPort->getProtocolType() == Port::ProcotolType::TYPE_CLASSIC)
    {
        m_pUi->actionClassic->setChecked(true);
        m_pUi->actionTdfp->setChecked(false);
    }
    else
    {
        m_pUi->actionClassic->setChecked(false);
        m_pUi->actionTdfp->setChecked(true);
    }
    // port name and baudrate
    m_pUi->cbBaudrate->setCurrentIndex(m_pSettings->value("port/baudrate").toInt());
    m_pUi->cbPort->setCurrentIndex(m_pSettings->value("port/name").toInt());
}

void MainWindow::saveSettings()
{
    if (!isMaximized())
    {
        m_pSettings->setValue("wnd/high", this->height());
        m_pSettings->setValue("wnd/width", this->width());
        m_pSettings->setValue("wnd/maximized", 0);
    }
    else
    {
        m_pSettings->setValue("wnd/maximized", 1);
    }

    // left panel
    m_pSettings->setValue("wnd/left_panel", m_pUi->splitter->saveState());
    m_pSettings->setValue("wnd/left_panel_tab", m_pUi->tabWidget->currentIndex());

    m_pSettings->setValue("wnd/theme", static_cast<int>(m_uiTheme));
    m_pSettings->setValue("port/protocol", static_cast<int>(m_pPort->getProtocolType()));
    m_pSettings->setValue("port/name", m_pUi->cbPort->currentIndex());
    m_pSettings->setValue("port/baudrate", m_pUi->cbBaudrate->currentIndex());
    m_pSettings->sync();
}

void MainWindow::updateTheme()
{
    if (m_uiTheme == THEME_WHITE)
    {
        on_actionWhite_toggled(true);

        qApp->setStyle(QStyleFactory::create("Fusion"));
        qApp->setPalette(m_paletteWhite);

        m_pChart->setThemeBackground(Qt::white);
    }
    else
    {
        on_actionBlack_toggled(true);

        qApp->setStyle(QStyleFactory::create("Fusion"));
        qApp->setPalette(m_paletteBlack);

        m_pChart->setThemeBackground(Qt::black);
    }
}

void MainWindow::on_actionTdfp_toggled(bool arg1)
{
    if (arg1)
    {
        m_pUi->actionClassic->setChecked(!arg1);
        m_pPort->setProtocolType(Port::ProcotolType::TYPE_TDFP);
    }
}

void MainWindow::on_actionClassic_toggled(bool arg1)
{
    if (arg1)
    {
        m_pUi->actionTdfp->setChecked(!arg1);
        m_pPort->setProtocolType(Port::ProcotolType::TYPE_CLASSIC);
    }
}

void MainWindow::on_actionBlack_toggled(bool arg1)
{
    if (arg1)
    {
        m_uiTheme = THEME_BLACK;
        m_pUi->actionWhite->setChecked(!arg1);
        m_pUi->actionBlack->setChecked(arg1);
    }
}

void MainWindow::on_actionWhite_toggled(bool arg1)
{
    if (arg1)
    {
        m_uiTheme = THEME_WHITE;
        m_pUi->actionBlack->setChecked(!arg1);
        m_pUi->actionWhite->setChecked(arg1);
    }
}

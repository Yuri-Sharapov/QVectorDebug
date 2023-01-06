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
#include <QSettings>
#include <QStyleFactory>

#include "chart_widget.h"
#include "port.h"
#include "wave_generator.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class TxThread : public QThread
{
    Q_OBJECT
public:
    void startThread(void)
    {
        m_mutex.lock();
        m_isRun = true;
        m_generator.start();
        m_mutex.unlock();
    }
    void stopThread(void)
    {
        m_mutex.lock();
        m_isRun = false;
        m_generator.stop();
        m_mutex.unlock();
    }

    bool isActive(void) {return m_isRun;}

    void setGenerator(WaveGenerator& generator){m_generator = generator;}

    float getOutput(void)
    {
        float output = 0;
        m_mutex.lock();
        output = m_generator.getOutput();
        m_mutex.unlock();
        return output;
    }

    uint64_t getTime(void)
    {
        uint64_t output = 0;
        m_mutex.lock();
        output = m_generator.getTime();
        m_mutex.unlock();
        return output;
    }
signals:
    void newData(void);
protected:
    void run();
private:
    bool            m_isRun = false;
    WaveGenerator   m_generator;
    QMutex          m_mutex;
};


class MainWindow : public QMainWindow
{
    Q_OBJECT

    enum ThemeSelector
    {
        THEME_WHITE,
        THEME_BLACK
    };

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_PortUpdatePlot(qint64 timeNs, short var1, short var2, short var3, short var4);

    void on_btnConnect_clicked();
    void on_btnSend_clicked();

    void on_cbEnabled_1_stateChanged(int arg1);
    void on_cbEnabled_2_stateChanged(int arg1);
    void on_cbEnabled_3_stateChanged(int arg1);
    void on_cbEnabled_4_stateChanged(int arg1);

    void on_actionExit_triggered();
    void on_actionSave_triggered();
    void on_actionOpen_triggered();

    void on_actionTdfp_toggled(bool arg1);
    void on_actionClassic_toggled(bool arg1);
    void on_actionBlack_toggled(bool arg1);
    void on_actionWhite_toggled(bool arg1);

    void on_btnStart_clicked();

    void on_newTxData(void);

    void on_cbCursorEnable_stateChanged(int arg1);

    void on_spinCursorX_valueChanged(int arg1);

    void on_spinCursorY_valueChanged(int arg1);

private:
    void showStatusMessage(const QString &message);

    void serialSetup(void);
    void serialConnect(void);
    void serialDisconnect(void);

    void openChart(QVector<Port::ChartVar>* pVars);

    void updateWaveGenerator(void);

    void restoreSettings(void);
    void saveSettings(void);
    void updateTheme(void);

    Ui::MainWindow* m_pUi;
    Port*           m_pPort;
    ChartWidget*    m_pChart = nullptr;
    QLabel*         m_pStatus = nullptr;

    TxThread*       m_pTxThread;
    WaveGenerator*  m_pGenerator;

    QSettings*      m_pSettings;
    ThemeSelector   m_uiTheme = THEME_WHITE;

    QPalette        m_paletteWhite;
    QPalette        m_paletteBlack;

    uint64_t        m_lastTimeUpdate = 0;
};
#endif // MAINWINDOW_H

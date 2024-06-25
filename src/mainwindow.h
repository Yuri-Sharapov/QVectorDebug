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
#include <QElapsedTimer>

#include "platform.h"
#include "chart_widget.h"
#include "port.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE


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

    void on_actionBlack_toggled(bool arg1);
    void on_actionWhite_toggled(bool arg1);

    void on_actionUartVector_toggled(bool arg1);
    void on_actionFEsc_toggled(bool arg1);

    void on_actionC1_toggled(bool arg1);
    void on_actionC2_toggled(bool arg1);

    void on_cbCursor1_currentIndexChanged(int arg1);
    void on_cbCursor2_currentIndexChanged(int arg1);

    void on_cbEscTemperature_stateChanged(int arg1);
    void on_cbEscVoltage_stateChanged(int arg1);
    void on_cbEscCurrent_stateChanged(int arg1);
    void on_cbEscPower_stateChanged(int arg1);
    void on_cbEscRpm_stateChanged(int arg1);

private:
    void showStatusMessage(const QString &message);

    void serialSetup(void);
    void serialConnect(void);
    void serialDisconnect(void);

    void openChart(QVector<Port::ChartVar>* pVars);

    void restoreSettings(void);
    void saveSettings(void);
    void updateTheme(void);

    Ui::MainWindow* m_pUi;
    Port*           m_pPort;
    ChartWidget*    m_pChart = nullptr;
    QLabel*         m_pStatus = nullptr;

    QSettings*      m_pSettings;
    ThemeSelector   m_uiTheme = THEME_WHITE;
    Port::ProcotolType  m_protocol = Port::TYPE_VECTOR;

    QPalette        m_paletteWhite;
    QPalette        m_paletteBlack;

    uint64_t        m_lastTimeUpdate = 0;
};
#endif // MAINWINDOW_H
